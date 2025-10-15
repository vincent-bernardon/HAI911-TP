#ifndef PARTICLE_H
#define PARTICLE_H
#include <QVector3D>
#include <cmath>
struct Particle {
    QVector3D pos;
    QVector3D speed;
    float age = 0.0f;
    float ageMax = 100.0f;
    float padding[2];

    void init() {
        pos = QVector3D(0.0, 0.0, 0.0);
        float angle = 2.0f * M_PI * rand() / RAND_MAX;
        float norm = 0.5f * rand() / RAND_MAX;
        speed = QVector3D(norm * cos(angle), norm * sin(angle),
                   2.0f+(rand() / float(RAND_MAX))*1.5f);
        age = 0.0f;
        ageMax = 2.0f + (rand() / float(RAND_MAX))*3.0f;
    }

    // void animate() {
    //     speed[2] -= 0.05f;
    //     pos += 0.1f * speed;

    //     if (pos[2] < 0.0) {
    //         speed[2] = -0.8 * speed[2];
    //         pos[2] = 0.0;
    //     }
    //     age += 1.0f;
    //     if(age >= ageMax) init();
    // }

    void animate(float dt) {
    const QVector3D gravity(0.0f, 0.0f, -9.8f); // Gravité en m/s²
    speed += gravity * dt; // Mettre à jour la vitesse avec la gravité
    pos += speed * dt;     // Mettre à jour la position avec la vitesse

    if (pos.z() < 0.0f) {
        pos.setZ(0.0f);
        speed.setZ(-0.6 * speed[2]); // Rebondir sur le sol

    }
    age += dt; // Incrémenter l'âge en fonction de dt
    if (age >= ageMax) init();
}
};

#endif // PARTICLE_H
