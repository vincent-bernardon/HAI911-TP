#include "camera.h"
#include <QtMath>

Camera::Camera()
    : aspect_(4.0f / 3.0f),
    distance_(5.0f)
{
    updateView();
}

void Camera::setAspectRatio(float ratio) {
    aspect_ = ratio;
}

void Camera::pan(float dx, float dy) {
    float speed = distance_ * 0.002f;
    target += -right() * dx * speed;
    target += up() * dy * speed;
    updateView();
}


void Camera::rotate(float dx, float dy) {
    float sign = up()[1] < 0.0 ? -1.0 : 1.0;
    yaw_ += sign * dx * 0.04;
    pitch_ += dy * 0.04;
    updateView();
}

void Camera::zoom(float delta) {
    distance_ *= (1.0f - delta * 0.0001f);
    distance_ = qBound(0.5f, distance_, 50.0f);
    updateView();
}

void Camera::updateView()
{
    QMatrix4x4 iden;
    iden.setToIdentity();
    QVector3D position = target - forward() * distance_;
    QMatrix4x4 rotation;
    rotation.setToIdentity();
    rotation.rotate(orientation().normalized());
    iden.translate(position);
    iden *= rotation;
    view = iden.inverted();
}


QMatrix4x4 Camera::viewMatrix() const {
    return view;
}

QMatrix4x4 Camera::projectionMatrix() const {
    QMatrix4x4 proj;
    proj.perspective(60.0f, aspect_, 0.01f, 1000.0f);
    return proj;
}



