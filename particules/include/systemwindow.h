#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include "openglwindow.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "particle.h"

class ParticleSystemWindow : public OpenGLWindow {
public:
    using OpenGLWindow::OpenGLWindow;

    void initialize() override;
    void render() override;
    ~ParticleSystemWindow()
    {
        delete program;
    }


private:
    std::vector<Particle> particles;
    std::vector<GLfloat> particlePositions;

    QOpenGLBuffer vbo;

    QOpenGLShaderProgram* program = nullptr;

    GLint matrixUniform = -1;

    const int numParticles = 2000;

};


#endif // SYSTEMWINDOW_H
