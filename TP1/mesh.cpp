#include "mesh.h"
#include <qmath.h>

Mesh::Mesh()
    : m_count(0)
{
    m_data.resize(2500 * 6);

    // Définition des sommets du cube centré en (0,0,0), de taille 1
    const GLfloat s = 0.08f;
    // 8 sommets
    QVector3D v0(-s, -s, -s);
    QVector3D v1(+s, -s, -s);
    QVector3D v2(+s, +s, -s);
    QVector3D v3(-s, +s, -s);
    QVector3D v4(-s, -s, +s);
    QVector3D v5(+s, -s, +s);
    QVector3D v6(+s, +s, +s);
    QVector3D v7(-s, +s, +s);

    // Face avant (z = +s)
    quad(v4, v5, v6, v7);
    // Face arrière (z = -s)
    quad(v0, v3, v2, v1);
    // Face gauche (x = -s)
    quad(v0, v4, v7, v3);
    // Face droite (x = +s)
    quad(v1, v2, v6, v5);
    // Face haut (y = +s)
    quad(v3, v7, v6, v2);
    // Face bas (y = -s)
    quad(v0, v1, v5, v4);
}

void Mesh::add(const QVector3D &v, const QVector3D &n)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_count += 6;
}

void Mesh::quad(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3, const QVector3D &v4)
{
    // Calcul de la normale pour la face (ordre trigonométrique)
    QVector3D n = QVector3D::normal(v2 - v1, v4 - v1);
    // Premier triangle
    add(v1, n);
    add(v2, n);
    add(v4, n);
    // Second triangle
    add(v2, n);
    add(v3, n);
    add(v4, n);
}
