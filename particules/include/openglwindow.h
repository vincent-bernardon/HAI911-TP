// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include <QMouseEvent>

#include "camera.h"

QT_FORWARD_DECLARE_CLASS(QPainter)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)
QT_FORWARD_DECLARE_CLASS(QOpenGLPaintDevice)

//! [1]
class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = nullptr);
    ~OpenGLWindow();

    virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;

    void exposeEvent(QExposeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QOpenGLFunctions_4_3_Core *gl43 = nullptr;
    QOpenGLContext *m_context = nullptr;

    Camera m_camera;
    QPoint m_lastMousePos;



private:
    bool m_animating = false;

    QOpenGLPaintDevice *m_device = nullptr;
};
//! [1]

#endif // OPENGLWINDOW_H

