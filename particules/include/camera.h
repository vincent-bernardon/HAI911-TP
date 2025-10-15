#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera {
public:
    Camera();

    void setAspectRatio(float ratio);
    void zoom(float delta);
    void rotate(float dx, float dy);
    void pan(float dx, float dy);

    QMatrix4x4 projectionMatrix() const;
    QMatrix4x4 viewMatrix() const;


private:

    QVector3D up() { return orientation() * cUp; }

    QVector3D right() { return orientation() * cRight; }

    QVector3D forward() { return orientation() * cForward; }
    
    QQuaternion orientation() { return QQuaternion::fromEulerAngles(-pitch_, -yaw_, 0.0);}

    void updateView();

    float aspect_;
    float distance_ = 1.40292;

    float yaw_ = -0.32;
    float pitch_ = -107.6;

    QMatrix4x4 view;

    QVector3D target   = QVector3D(0.0126163, 0.0864498, 0.459742);

    QVector3D cUp       = QVector3D(0.0, 1.0, 0.0);
    QVector3D cRight    = QVector3D(1.0, 0.0, 0.0);
    QVector3D cForward  = QVector3D(0.0, 0.0, -1.0);

};
#endif
