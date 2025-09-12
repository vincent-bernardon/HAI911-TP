#ifndef MESH_H
#define MESH_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QOpenGLBuffer>

class Mesh
{
public:
    Mesh();
    const GLfloat *constData() const { return m_data.constData(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }

private:
    void quad(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3, const QVector3D &v4);
    void add(const QVector3D &v, const QVector3D &n);

    QVector<GLfloat> m_data;
    int m_count;

};

#endif