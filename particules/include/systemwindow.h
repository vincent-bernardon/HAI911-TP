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
QOpenGLShaderProgram* computeProgram = nullptr;
    QOpenGLBuffer vbo;
GLuint ssbo;
    QOpenGLShaderProgram* program = nullptr;
    QOpenGLFunctions_4_3_Core* gl43 = nullptr;
    GLint matrixUniform = -1;

    const int numParticles = 2000;

};


#endif // SYSTEMWINDOW_H
