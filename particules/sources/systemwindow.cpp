#include "systemwindow.h"
#include <QMatrix4x4>
#include <QOpenGLShader>
#include <QScreen>
#include <QtMath>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include <QDateTime>
#include <QOpenGLTexture>
#include <QElapsedTimer>

QElapsedTimer frameTimer;

void ParticleSystemWindow::initialize() {
    frameTimer.start();

    m_context = new QOpenGLContext(this);
    m_context->create();

    //Switch to OpenGL context
    m_context->makeCurrent(this);
    gl43 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>();
    if (!gl43) {
        qFatal("Impossible to initialize OpenGLFunction 4.3 Core.");
    }
    gl43->initializeOpenGLFunctions();
    srand(static_cast<unsigned int>(QDateTime::currentMSecsSinceEpoch() & 0xFFFFFFFF));

    particles.resize(numParticles);
    for (auto& p : particles) p.init();
    // Créer le Storage Buffer Object (SSBO)
    gl43->glGenBuffers(1, &ssbo);
    gl43->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    // Allouer et remplir le SSBO avec les données des particules
    gl43->glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    // Lier le SSBO au binding point 0 (doit correspondre au shader)
    gl43->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // Désactiver le buffer
    gl43->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    vbo.create();
    m_camera.setAspectRatio(width() / float(height()));

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/particle.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Geometry, "../shaders/particle.geom");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/particle.frag");
    program->link();
    program->bind();

    unsigned char * data = nullptr;
    int imgWidth, imgHeight, imgChannels;
    // data = stbi_load("../data/smoke.png", &imgWidth, &imgHeight, &imgChannels, 0);

    QImage img("../data/smoke.png");
    if(img.isNull()) {
        qDebug() << "Failed to load texture";
    } else {
        img = img.convertToFormat(QImage::Format_RGBA8888);
        QOpenGLTexture* texture = nullptr;
        texture = new QOpenGLTexture(img);
        texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
        texture->bind(0);
        program->setUniformValue("particleTexture", 0);
    }
    

    matrixUniform = program->uniformLocation("mvp");

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);

    // Charger et lier le compute shader
    computeProgram = new QOpenGLShaderProgram(this);
    computeProgram->addShaderFromSourceFile(QOpenGLShader::Compute, "../shaders/particle.comp");
    if (!computeProgram->link()) {
        qDebug() << "Failed to link compute shader:" << computeProgram->log();
    }
}

void ParticleSystemWindow::render() {
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.f, 1.0f, 1.0f, 1.0f);

    // Calculer dt en secondes
    float dt = frameTimer.elapsed() / 1000.0f;
    frameTimer.restart(); // Redémarrer le timer pour la prochaine frame

    // particlePositions.clear();
    // particlePositions.reserve(numParticles * 3);
    // for (auto& p : particles) {
    //     p.animate(dt);
    //     particlePositions.push_back(p.pos.x());
    //     particlePositions.push_back(p.pos.y());
    //     particlePositions.push_back(p.pos.z());
    // }

    // vbo.bind();
    // vbo.allocate(particlePositions.data(), particlePositions.size() * sizeof(GLfloat) * 3);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // glEnableVertexAttribArray(0);

    // program->bind();
    
    // QMatrix4x4 mvp = m_camera.projectionMatrix() * m_camera.viewMatrix();
    // program->setUniformValue(matrixUniform, mvp);

    // glDrawArrays(GL_POINTS, 0, numParticles);

    // Utiliser le compute shader pour mettre à jour les particules
    computeProgram->bind();
    glUniform1f(glGetUniformLocation(computeProgram->programId(), "dt"), dt);

    // Lancer les calculs avec glDispatchCompute
    gl43->glDispatchCompute((numParticles + 255) / 256, 1, 1);

    // Synchroniser les accès mémoire pour s'assurer que les calculs sont terminés
    gl43->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    computeProgram->release();

    // Rendre les particules
    program->bind();
    QMatrix4x4 mvp = m_camera.projectionMatrix() * m_camera.viewMatrix();
    program->setUniformValue(matrixUniform, mvp);

    glBindBuffer(GL_ARRAY_BUFFER, ssbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, numParticles);

    program->release();
}


