#include "mesh.h"
#include <qmath.h>
#include <fstream>
#include <sstream>

Mesh::Mesh()
    : m_count(0)
{
    m_data.resize(2500 * 6);
}

void Mesh::generateCube()
{
    // m_data.clear();
    m_count = 0;
    const GLfloat s = 0.1f;
    QVector3D v0(-s, -s, -s);
    QVector3D v1(+s, -s, -s);
    QVector3D v2(+s, +s, -s);
    QVector3D v3(-s, +s, -s);
    QVector3D v4(-s, -s, +s);
    QVector3D v5(+s, -s, +s);
    QVector3D v6(+s, +s, +s);
    QVector3D v7(-s, +s, +s);
    quad(v4, v5, v6, v7);
    quad(v0, v3, v2, v1);
    quad(v0, v4, v7, v3);
    quad(v1, v2, v6, v5);
    quad(v3, v7, v6, v2);
    quad(v0, v1, v5, v4);
    m_data.resize(m_count);
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


void Mesh::loadOFF(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        qWarning("Could not open the OFF file.");
        return;
    }

    std::string line;
    std::getline(file, line);
    if (line != "OFF") {
        qWarning("Not a valid OFF file.");
        return;
    }

    size_t numVertices, numFaces, numEdges;
    file >> numVertices >> numFaces >> numEdges;

    std::vector<QVector3D> positions(numVertices);
    std::vector<QVector3D> normals(numVertices, QVector3D(0, 0, 0));

    // Lire les sommets
    for (size_t i = 0; i < numVertices; ++i) {
        float x, y, z;
        file >> x >> y >> z;
        positions[i] = QVector3D(x, y, z);
    }

    // Lire les faces
    for (size_t i = 0; i < numFaces; ++i) {
        size_t n, v0, v1, v2;
        file >> n >> v0 >> v1 >> v2;
        if (n != 3) {
            qWarning("Only triangular faces are supported.");
            return;
        }
        // Calcul de la normale de la face
        QVector3D a = positions[v1] - positions[v0];
        QVector3D b = positions[v2] - positions[v0];
        QVector3D nrm = QVector3D::normal(a, b);

        normals[v0] += nrm;
        normals[v1] += nrm;
        normals[v2] += nrm;
    }

    // Normaliser les normales
    for (size_t i = 0; i < numVertices; ++i) {
        normals[i].normalize();
    }

    // Revenir au début des faces pour ajouter les triangles au mesh
    file.clear();
    file.seekg(0, std::ios::beg);
    std::getline(file, line); // OFF
    file >> numVertices >> numFaces >> numEdges;
    for (size_t i = 0; i < numVertices; ++i) {
        float x, y, z;
        file >> x >> y >> z;
    }
    m_count = 0;
    for (size_t i = 0; i < numFaces; ++i) {
        size_t n, v0, v1, v2;
        file >> n >> v0 >> v1 >> v2;
        add(positions[v0], normals[v0]);
        add(positions[v1], normals[v1]);
        add(positions[v2], normals[v2]);
    }
    m_data.resize(m_count);
}

