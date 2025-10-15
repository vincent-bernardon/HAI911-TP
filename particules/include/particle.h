#ifndef PARTICLE_H
#define PARTICLE_H
#include <QVector3D>
#include <cmath>
struct Particle {
    QVector3D pos;
    QVector3D speed;
    float age = 0;
    float ageMax = 100;


    void init() {
        pos = QVector3D(0.0, 0.0, 0.0);
        float angle = 2.0 * M_PI * rand() / RAND_MAX;
        float norm = 0.04 * rand() / RAND_MAX;
        speed = QVector3D(norm * cos(angle), norm * sin(angle),
                   rand() / static_cast<float>(RAND_MAX));
        age = 0.0f;
        ageMax = 50.0f + (100.0f * rand() / float(RAND_MAX));
    }

    void animate() {
        speed[2] -= 0.05f;
        pos += 0.1f * speed;

        if (pos[2] < 0.0) {
            speed[2] = -0.8 * speed[2];
            pos[2] = 0.0;
        }
        age += 1.0f;
        if(age >= ageMax) init();
    }
};

#endif // PARTICLE_H
