#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <cmath>

void Mesh::loadOFF (const std::string & filename) {
    std::ifstream in (filename.c_str ());
    if (!in)
        exit (EXIT_FAILURE);
    std::string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    V.resize (sizeV);
    T.resize (sizeT);
    for (unsigned int i = 0; i < sizeV; i++)
        in >> V[i].p;
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        for (unsigned int j = 0; j < 3; j++)
            in >> T[i].v[j];
    }
    in.close ();
    recomputeNormals ();
}

void Mesh::recomputeNormals () {
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n = Vec3 (0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < T.size (); i++) {
        Vec3 e01 = V[T[i].v[1]].p -  V[T[i].v[0]].p;
        Vec3 e02 = V[T[i].v[2]].p -  V[T[i].v[0]].p;
        Vec3 n = Vec3::cross (e01, e02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            V[T[i].v[j]].n += n;
    }
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n.normalize ();
}

void Mesh::compute_skinning_weights( Skeleton & skeleton ) {
    //---------------------------------------------------//
    //---------------------------------------------------//
    // code to change :

    // Indications:
    // you should compute weights for each vertex w.r.t. the skeleton bones
    // so each vertex will have B weights (B = number of bones)
    // these weights shoud be stored in vertex.w:

    //pour chaque vertex on utilise la distance euclidienne aux os pour calculer les poids que l'on ajoute dans vertex.w
    //on initialise les poids a 0
    for( unsigned int i = 0 ; i < V.size() ; ++i ) {
        V[i].w.resize( skeleton.bones.size(), 0.0 );
    }

    float n = 8.0f;




    // pour chaque vertex on calcule la distance a l'os
    for( unsigned int i = 0 ; i < V.size() ; ++i ) {
        Vec3 p = V[i].p;

            // on parcourt chaque os
        for( unsigned int b = 0 ; b < skeleton.bones.size() ; ++b ) {
            Bone & bone = skeleton.bones[b];
            Vec3 joint1 = skeleton.articulations[ bone.joints[0] ].p;
            Vec3 joint2 = skeleton.articulations[ bone.joints[1] ].p;

            //moyenne entre les deux joints de l'os
            Vec3 boneDir;
            boneDir = (joint1 + joint2) / 2.0f;

            // Calculer la distance euclidienne du vertex p au segment d'os
            Vec3 dist = p - boneDir;

            float poid = std::pow(1.0f / dist.squareLength(), n);
            V[i].w[b] = poid;
        }

        //normalisation des poids
        float sum = 0.0f;
        for( unsigned int b = 0 ; b < skeleton.bones.size() ; ++b ) {
            sum += V[i].w[b];
        }
        for( unsigned int b = 0 ; b < skeleton.bones.size() ; ++b ) {
            V[i].w[b] /= sum;
        }
    }


    //---------------------------------------------------//
    //---------------------------------------------------//
    //---------------------------------------------------//
}

void Mesh::draw( int displayedBone ) const {

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < T.size (); i++)
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex & v = V[T[i].v[j]];
            if( displayedBone >= 0 && displayedBone < v.w.size() ) {
                Vec3 rgb = HSVtoRGB( v.w[displayedBone], 0.8,0.8 );
                glColor3f( rgb[0], rgb[1], rgb[2] );
            } else glColor3f( 0.6, 0.6, 0.6 );
            glNormal3f (v.n[0], v.n[1], v.n[2]);
            glVertex3f (v.p[0], v.p[1], v.p[2]);
        }

    glEnd ();
}

void Mesh::drawTransformedMesh( SkeletonTransformation & transfo ) const {


    std::vector< Vec3 > new_positions( V.size() );
    std::vector< Vec3 > new_normals( V.size() );

    //---------------------------------------------------//
    //---------------------------------------------------//
    // code to change :
    for( unsigned int i = 0 ; i < V.size() ; ++i ) {
        Vec3 p = V[i].p;
        Vec3 n = V[i].n;

        // Indications:
        // you should use the skinning weights to blend the transformations of the vertex position by the bones.
        
        new_positions[i] = Vec3(0.0f, 0.0f, 0.0f);
        
        // Calculer la transformation linéaire moyenne Ci = Σj wij * Mj
        Mat3 Ci = Mat3(); // Matrice nulle au départ
        
        for( unsigned int b = 0 ; b < transfo.bone_transformations.size() ; ++b ) {
            BoneTransformation & bt = transfo.bone_transformations[b];
            float w = V[i].w[b];

            //appliquer la transformation de l'os b sur le vertex p
            new_positions[i] += w * (bt.world_space_rotation * p + bt.world_space_translation);

            // Accumuler les matrices de rotation pondérées pour Ci
            Ci += w * bt.world_space_rotation;
        }
        
        // Calculer (Ci^-1)^T pour transformer la normale
        bool isRealInverse;
        Mat3 Ci_inverse = Mat3::pseudoInverse(Ci, isRealInverse);
        Mat3 Ci_inverse_transpose = Ci_inverse.getTranspose();
        
        // Transformer la normale par (Ci^-1)^T
        new_normals[i] = Ci_inverse_transpose*n;
        
        // Normaliser la normale transformée
        new_normals[i].normalize();

    }
    //---------------------------------------------------//
    //---------------------------------------------------//
    //---------------------------------------------------//

    glEnable(GL_LIGHTING);
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < T.size (); i++)
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex & v = V[T[i].v[j]];
            Vec3 p = new_positions[ T[i].v[j] ];
            Vec3 n = new_normals[ T[i].v[j] ];
            glColor3f( 0.6, 0.6, 0.6 );
            glNormal3f (n[0], n[1], n[2]);
            glVertex3f (p[0], p[1], p[2]);
        }
    glEnd ();
}

/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param fH Hue component, used as input, range: [0, 1]
  \param fS Hue component, used as input, range: [0, 1]
  \param fV Hue component, used as input, range: [0, 1]

  \param fR Red component, used as output, range: [0, 1]
  \param fG Green component, used as output, range: [0, 1]
  \param fB Blue component, used as output, range: [0, 1]

*/
Vec3 Mesh::HSVtoRGB( float fH, float fS, float fV) const {

    fH=(1.-fH)*0.65*360.;

    float fR, fG, fB;
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if(0 <= fHPrime && fHPrime < 1) {
        fR = fC;
        fG = fX;
        fB = 0;
    } else if(1 <= fHPrime && fHPrime < 2) {
        fR = fX;
        fG = fC;
        fB = 0;
    } else if(2 <= fHPrime && fHPrime < 3) {
        fR = 0;
        fG = fC;
        fB = fX;
    } else if(3 <= fHPrime && fHPrime < 4) {
        fR = 0;
        fG = fX;
        fB = fC;
    } else if(4 <= fHPrime && fHPrime < 5) {
        fR = fX;
        fG = 0;
        fB = fC;
    } else if(5 <= fHPrime && fHPrime < 6) {
        fR = fC;
        fG = 0;
        fB = fX;
    } else {
        fR = 0;
        fG = 0;
        fB = 0;
    }

    fR += fM;
    fG += fM;
    fB += fM;
    return Vec3(fR,fG,fB);
}
