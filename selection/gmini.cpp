// -------------------------------------------
// gMini : a minimal OpenGL/GLUT application
// for 3D graphics.
// Copyright (C) 2006-2008 Tamy Boubekeur
// All rights reserved.
// -------------------------------------------

// -------------------------------------------
// Disclaimer: this code is dirty in the
// meaning that there is no attention paid to
// proper class attribute access, memory
// management or optimisation of any kind. It
// is designed for quick-and-dirty testing
// purpose.
// -------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <GL/glut.h>
#include "src/Vec3.h"
#include "src/Camera.h"
#include "src/Mesh.h"
#include "src/linearSystem.h"
#include "src/LaplacianWeights.h"
#include "src/AStar.h"
#include "extern/eigen3/Eigen/SVD"
#include "extern/eigen3/Eigen/Geometry"


using namespace std;
#define GLUT_KEY_ENTER 13
#define GLUT_KEY_ESCAPE 27

// -------------------------------------------
// OpenGL/GLUT application code.
// -------------------------------------------

static GLint window;
static unsigned int SCREENWIDTH = 640;
static unsigned int SCREENHEIGHT = 480;
static Camera camera;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static unsigned int FPS = 0;
static bool fullScreen = false;

enum ViewerState {
    ViewerState_NORMAL ,
    ViewerState_EDITINGHANDLE ,
    ViewerState_TRANSLATINGHANDLE ,
    ViewerState_ROTATINGHANDLE ,
    ViewerState_ASTAR_MODE
};
ViewerState viewerState;

enum SelectionToolState
{
    SelectionTool_Rectangle,
    SelectionTool_Sphere
};
SelectionToolState selectionToolState;

#include "src/RectangleSelectionTool.h"
RectangleSelectionTool rectangleSelectionTool;

#include "src/SphereSelectionTool.h"
SphereSelectionTool sphereSelectionTool;
float selectionRadius = 0.1f;

typedef struct {
    float r;       // ∈ [0, 1]
    float g;       // ∈ [0, 1]
    float b;       // ∈ [0, 1]
} RGB;



RGB scalarToRGB( float scalar_value ) //Scalar_value ∈ [0, 1]
{
    RGB rgb;
    float H = scalar_value*360., S = 1., V = 0.85,
            P, Q, T,
            fract;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        rgb = (RGB){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        rgb = (RGB){.r = Q, .g = V, .b = P};
    else if (2. <= H && H < 3.)
        rgb = (RGB){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        rgb = (RGB){.r = P, .g = Q, .b = V};
    else if (4. <= H && H < 5.)
        rgb = (RGB){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        rgb = (RGB){.r = V, .g = P, .b = Q};
    else
        rgb = (RGB){.r = 0., .g = 0., .b = 0.};

    return rgb;
}

// -------------------------------------------
// ARAP variables
// -------------------------------------------

Mesh mesh;
LaplacianWeights edgeAndVertexWeights;
linearSystem arapLinearSystem;
std::vector< Eigen::MatrixXd > vertexRotationMatrices;

int numberOfHandles = 0;
int activeHandle = 0;
bool handlesWereChanged = false; // if they are changed, we need to update the system for ARAP
std::vector< bool > verticesAreMarkedForCurrentHandle;
std::vector< int > verticesHandles;
double spheresSize = 0.01;

// -------------------------------------------
// A* variables
// -------------------------------------------
AStar aStar;
std::vector<int> currentPath;
int pathStartVertex = -1;
int pathEndVertex = -1;
bool showPath = false;

// Variables pour le mode A* interactif
bool astarModeEnabled = false;
int selectedAStarVertex = -1;
std::vector<float> triangleWeights;    // Poids pour chaque triangle (0.0 à 1.0)
bool weightsComputed = false;

// Variables pour le mode visualisation des distances
bool distanceVisualizationMode = false;
std::vector<float> vertexDistances;   // Distances normalisées pour chaque sommet
std::vector<RGB> triangleColors;      // Couleurs RGB pour chaque triangle
bool distancesComputed = false;

bool geodesicDistancesComputed = false;








//------------------------------------------------------------------------------------------------------//
//---------------------------------  EXAMPLE OF USE OF A LINEAR SYSTEM  --------------------------------//
//------------------------------------------------------------------------------------------------------//
void testlinearSystem() {
    // You can get inspiration from this piece of code :
    {
        linearSystem mySystem;
        mySystem.setDimensions(3 , 3);

        mySystem.A(0,0) = 1.0;  mySystem.A(0,1) = 1.0;  mySystem.A(0,2) = 0.0;
        mySystem.A(1,0) = 0.0;  mySystem.A(1,1) = 1.0;  mySystem.A(1,2) = 1.0;
        mySystem.A(2,0) = 1.0;  mySystem.A(2,1) = 0.0;  mySystem.A(2,2) = 1.0;
        // the values that are not set with mySystem.A(row,column) = value, are set to 0 by default.
        mySystem.b(0) = 1.0;
        mySystem.b(1) = 0.0;
        mySystem.b(2) = 0.0;

        mySystem.preprocess();
        Eigen::VectorXd X;
        mySystem.solve(X);

        std::cout << X[0] << "  " << X[1] << "  " << X[2] << std::endl;
    }
}
//------------------------------------------------------------------------------------------------------//
//---------------------------------  EXAMPLE OF USE OF A LINEAR SYSTEM  --------------------------------//
//------------------------------------------------------------------------------------------------------//







Eigen::MatrixXd getClosestRotation( Eigen::MatrixXd const & m ) {
    Eigen::JacobiSVD< Eigen::MatrixXd > svdStruct = m.jacobiSvd( Eigen::ComputeFullU | Eigen::ComputeFullV );
    return svdStruct.matrixU() * svdStruct.matrixV().transpose();
}







//nicolas.luciani@umontpellier.fr

//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//---------------------------------  CODE TO CHANGE  --------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
void updateSystem() {
    if(! handlesWereChanged) return;

    // TODO:
    // set the right values for the number or rows and number of columns
    // remember: number of colums = nb of variables
    // remember: number of rows = nb of equations

    unsigned int ncolumns = 3*mesh.V.size();

    unsigned int nrows = 0;

    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        unsigned int numberOfNeighbors = edgeAndVertexWeights.get_n_adjacent_edges(v);
        nrows += numberOfNeighbors*3;   // WHAT TO PUT HERE ??????? How to update the number of rows ?
    }
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        if(verticesHandles[v] != -1) {
            nrows += 3;  // WHAT TO PUT HERE ??????? How to update the number of rows ?
        }
    }

    // Once the number of rows and columns have been found, we can allocate the matrices:
    arapLinearSystem.setDimensions( nrows , ncolumns );

    // TODO:
    // set the right values for the matrix A in the linear system

    unsigned int equationIndex = 0;
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        for( std::map< unsigned int , double >::const_iterator it = edgeAndVertexWeights.get_weight_of_adjacent_edges_it_begin(v) ;
             it != edgeAndVertexWeights.get_weight_of_adjacent_edges_it_end(v) ; ++it) {

            unsigned int vNeighbor = it->first;

            // WHAT TO PUT HERE ??????? How to update the entries of A ?
            
            arapLinearSystem.A(equationIndex,3*v)=-1.0f;
            arapLinearSystem.A(equationIndex,3*vNeighbor)=1.0f;
            equationIndex++;

            arapLinearSystem.A(equationIndex,1+3*v)=-1.0f;
            arapLinearSystem.A(equationIndex,1+3*vNeighbor)=1.0f;
            equationIndex++;

            arapLinearSystem.A(equationIndex,2+3*v)=-1.0f;
            arapLinearSystem.A(equationIndex,2+3*vNeighbor)=1.0f;
            equationIndex++;

            
        }
    }
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        if(verticesHandles[v] != -1) {

            // WHAT TO PUT HERE ??????? How to update the entries of A ?
            arapLinearSystem.A(equationIndex,3*v)=1.0f;
            arapLinearSystem.A(equationIndex+1,1+3*v)=1.0f;
            arapLinearSystem.A(equationIndex+2,2+3*v)=1.0f;
            equationIndex+=3;

        }
    }

    arapLinearSystem.preprocess();
    handlesWereChanged = false;
}



void updateMeshVertexPositionsFromARAPSolver() {
    // return; // TODO : COMMENT THIS LINE WHEN YOU START THE EXERCISE  (setup of the matrix A for the linear system A.X=B)
    updateSystem();

    unsigned int maxIterationsForArap = 5;


    // return; // TODO : COMMENT THIS LINE WHEN YOU CONTINUE THE EXERCISE  (setup of the vector B for the linear system A.X=B)
    // set the right values for the vector b in the linear system, solve the linear system and update the positions using the solution.


    for( unsigned int arapIteration = 0 ; arapIteration < maxIterationsForArap ; ++arapIteration ) {
        // 1 FIRST : SOLVE THE LINEAR SYSTEM TO UPDATE THE POSITIONS, GIVEN THE EXISTING ROTATION MATRICES
        unsigned int equationIndex = 0;
        for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
            for( std::map< unsigned int , double >::const_iterator it = edgeAndVertexWeights.get_weight_of_adjacent_edges_it_begin(v) ;
                 it != edgeAndVertexWeights.get_weight_of_adjacent_edges_it_end(v) ; ++it) {
                unsigned int vNeighbor = it->first;
                Eigen::VectorXd rotatedEdge(3);
                for( unsigned int coord = 0 ; coord < 3 ; ++coord )
                    rotatedEdge[coord] = mesh.V[vNeighbor].pInit[coord]  -  mesh.V[v].pInit[coord];
                rotatedEdge = vertexRotationMatrices[v] * rotatedEdge;

                // WHAT TO PUT HERE ??????? How to update the entries of b ?
                arapLinearSystem.b(equationIndex)=rotatedEdge[0];
                equationIndex++;
                arapLinearSystem.b(equationIndex)=rotatedEdge[1];
                equationIndex++;
                arapLinearSystem.b(equationIndex)=rotatedEdge[2];
                equationIndex++;

            }
        }
        for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
            if(verticesHandles[v] != -1) {

                // WHAT TO PUT HERE ??????? How to update the entries of b ?
                arapLinearSystem.b(equationIndex)=mesh.V[v].p[0];
                equationIndex++;
                arapLinearSystem.b(equationIndex)=mesh.V[v].p[1];
                equationIndex++;
                arapLinearSystem.b(equationIndex)=mesh.V[v].p[2];
                equationIndex++;

            }
        }

        // Once the matrix A and the vector B are correctly set, we obtain the position of the vertices by solving for A.X = B
        Eigen::VectorXd X_newPositions;
        arapLinearSystem.solve(X_newPositions);
        for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
            if(verticesHandles[v] == -1) {
                for( unsigned int coord = 0 ; coord < 3 ; ++coord )
                    mesh.V[v].p[coord] = X_newPositions[3*v + coord];
            }
        }



        // return; // TODO : COMMENT THIS LINE WHEN YOU CONTINUE THE EXERCISE (update of the rotation matrices -- auxiliary variables)



        // 2 SECOND : UPDATE THE ROTATION MATRICES
        for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
            Eigen::MatrixXd tensorMatrix = Eigen::MatrixXd::Zero(3,3);
            for( std::map< unsigned int , double >::const_iterator it = edgeAndVertexWeights.get_weight_of_adjacent_edges_it_begin(v) ;
                 it != edgeAndVertexWeights.get_weight_of_adjacent_edges_it_end(v) ; ++it) {
                unsigned int vNeighbor = it->first;
                Eigen::VectorXd initialEdge(3);
                Eigen::VectorXd rotatedEdge(3);
                for( unsigned int coord = 0 ; coord < 3 ; ++coord ) {
                    initialEdge[coord] = mesh.V[vNeighbor].pInit[coord]  -  mesh.V[v].pInit[coord];
                    rotatedEdge[coord] = mesh.V[vNeighbor].p[coord]  -  mesh.V[v].p[coord];
                }

                // WHAT TO PUT HERE ??????? How to update the entries of the tensor   ?
                // 1 build
                tensorMatrix += it->second * (rotatedEdge * initialEdge.transpose());
            }
            // 2 SVD 3 solution
            vertexRotationMatrices[v] = getClosestRotation( tensorMatrix );
        }
    }
}
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//---------------------------------  CODE TO CHANGE  --------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//



void translateActiveHandle( Vec3 const & translationVector ) {
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        if( verticesHandles[v] == activeHandle ) {
            mesh.V[v].p += translationVector;
        }
    }

    updateMeshVertexPositionsFromARAPSolver();
}

void rotateActiveHandle( Vec3 const & rotationAxis , double angle ) {
    Eigen::Vector3d centerOfRotation(0,0,0);
    double sumWeights = 0.0;
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        if( verticesHandles[v] == activeHandle ) {
            centerOfRotation += Eigen::Vector3d(mesh.V[v].p[0] , mesh.V[v].p[1] , mesh.V[v].p[2]);
            sumWeights += 1.0;
        }
    }
    centerOfRotation /= sumWeights;

    Eigen::Vector3d axisEigenType( rotationAxis[0] , rotationAxis[1] , rotationAxis[2] );
    Eigen::Matrix3d rotation;
    rotation = Eigen::AngleAxisd(angle, axisEigenType);

    // Apply rotation and translation, such that the center of mass is preserved: R * c + t = c    =>    t = c - R * c;
    Eigen::Vector3d translation = centerOfRotation - rotation * centerOfRotation;

    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        if( verticesHandles[v] == activeHandle ) {
            Eigen::Vector3d newPos = rotation * Eigen::Vector3d(mesh.V[v].p[0] , mesh.V[v].p[1] , mesh.V[v].p[2])  +  translation;
            mesh.V[v].p = Vec3(newPos[0] , newPos[1] , newPos[2]);
        }
    }

    updateMeshVertexPositionsFromARAPSolver();
}

//// ------------------------------- BONUS -----------------------------------------/////

void get3DPosFromMouseInput(int x, int y, float &posX, float &posY, float &posZ)
{
    // get the 3D position from the mouse input
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX3D, posY3D, posZ3D;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX3D, &posY3D, &posZ3D);

    posX = (float)posX3D;
    posY = (float)posY3D;
    posZ = (float)posZ3D;
}

void setTagForVerticesInSphere(bool tagToSet)
{
    if(geodesicDistancesComputed)
    {
        printf("Using geodesic distances for selection\n");
        Vec3 sphereCenter;
        sphereCenter = sphereSelectionTool.center;

        //trouver son indice dans le tableau des sommets
        int centerVertexIndex = -1;
        float minDist = 1e30;
        for(unsigned int v = 0; v < mesh.V.size(); ++v)
        {
            float dist = (mesh.V[v].p - sphereCenter).norm();
            if(dist < minDist)
            {
                minDist = dist;
                centerVertexIndex = v;
            }
        }

        for(unsigned int v = 0; v < mesh.V.size(); ++v)
        {
            Vec3 const & p = mesh.V[v].p;
            float geodesicDist = aStar.getGeodesicDistance(centerVertexIndex, v);
            if (sphereSelectionTool.contains(p) && geodesicDist <= selectionRadius){
                verticesAreMarkedForCurrentHandle[v] = tagToSet;
            }
        }

    }else{
        printf("Using euclidean distances for selection\n");
        // check if vertices are inside the sphere
        for(unsigned int v = 0; v < mesh.V.size(); ++v)
        {
            Vec3 const & p = mesh.V[v].p;
            if (sphereSelectionTool.contains(p))
                verticesAreMarkedForCurrentHandle[v] = tagToSet;
        }
    }


}

void updateSphereRadiusWithScroll(int button)
{
    if(button == 3) //scroll up
    {
        selectionRadius *= 1.1f;
        sphereSelectionTool.updateSphere(selectionRadius);
    }
    else if(button == 4) //scroll down
    {
        selectionRadius *= 0.9f;
        sphereSelectionTool.updateSphere(selectionRadius);
    }

}






//---------------------------------   YOU DO NOT NEED TO CHANGE THE FOLLOWING CODE  --------------------------------//
void glVertex(Vec3 const & p) {
    glVertex3f(p[0] , p[1] , p[2]);
}
void glNormal(Vec3 const & p) {
    glNormal3f(p[0] , p[1] , p[2]);
}

bool activeHandleIsValid() {
    return activeHandle >= 0  &&  activeHandle < numberOfHandles;
}



/*
from your modelview:
|lx,ux,vx,px|     -- lx,ly,lz is your left-vector
|ly,uy,vy,py|     -- ux,uy,uz is your up-vector
|lz,uz,vz,pz|     -- vx,vy,vz is your view-vector
|0 ,0 ,0 ,1 |     -- px,py,pz is your translation
*/
Vec3 getRightVector() {
    float modelview[16];  glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    return Vec3( modelview[0] , modelview[4] , modelview[8] );
}
Vec3 getUpVector() {
    float modelview[16];  glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    return Vec3( modelview[1] , modelview[5] , modelview[9] );
}
Vec3 getViewVector() {
    float modelview[16];  glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    return Vec3( modelview[2] , modelview[6] , modelview[10] );
}



void setTagForVerticesInRectangle( bool tagToSet ) {
    float modelview[16];  glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    float projection[16]; glGetFloatv(GL_PROJECTION_MATRIX , projection);

    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        Vec3 const & p = mesh.V[ v ].p;

        float x = modelview[0] * p[0] + modelview[4] * p[1] + modelview[8] * p[2] + modelview[12];
        float y = modelview[1] * p[0] + modelview[5] * p[1] + modelview[9] * p[2] + modelview[13];
        float z = modelview[2] * p[0] + modelview[6] * p[1] + modelview[10] * p[2] + modelview[14];
        float w = modelview[3] * p[0] + modelview[7] * p[1] + modelview[11] * p[2] + modelview[15];
        x /= w; y /= w; z /= w; w = 1.f;

        float xx = projection[0] * x + projection[4] * y + projection[8] * z + projection[12] * w;
        float yy = projection[1] * x + projection[5] * y + projection[9] * z + projection[13] * w;
        float ww = projection[3] * x + projection[7] * y + projection[11] * z + projection[15] * w;
        xx /= ww; yy /= ww;

        xx = ( xx + 1.f ) / 2.f;
        yy = ( yy + 1.f ) / 2.f;

        if( rectangleSelectionTool.contains( xx , yy ) ) verticesAreMarkedForCurrentHandle[ v ] = tagToSet;
    }
}

void addVerticesToCurrentHandle() {
    // look at the rectangle rectangleSelectionTool, and see which vertices fall into the region.
    if( activeHandle < 0 || activeHandle >= numberOfHandles)
        return;

    if(selectionToolState == SelectionTool_Rectangle) setTagForVerticesInRectangle( rectangleSelectionTool.isAdding );
    else if(selectionToolState == SelectionTool_Sphere) setTagForVerticesInSphere( sphereSelectionTool.isAdding);
}

void finalizeEditingOfCurrentHandle() {
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        Vec3 const & p = mesh.V[ v ].p;
        if(verticesAreMarkedForCurrentHandle[ v ]) {
            verticesHandles[v] = activeHandle;
            verticesAreMarkedForCurrentHandle[ v ] = false; // prepare next selection
        }
    }

    handlesWereChanged = true;
}

void printUsage () {
    cerr << endl
         << "Usage : ./gmini [<file.off>]" << endl
         << "Keyboard commands" << endl
         << "------------------" << endl
         << " ?: Print help" << endl
         << " w: Toggle Wireframe Mode" << endl
         << " f: Toggle full screen mode" << endl
         << " m: Toggle A* interactive mode" << endl
         << " d: Toggle distance visualization mode" << endl
         << " a: Compute A* path between random vertices" << endl
         << " A: Compute A* path between vertex 0 and middle vertex" << endl
         << " p: Toggle path display" << endl
         << " <drag>+<left button>: rotate model" << endl
         << " <drag>+<right button>: move model" << endl
         << " <drag>+<middle button>: zoom" << endl
         << "A* Mode:" << endl
         << " - Press 'm' to enter A* mode" << endl
         << " - Click on mesh to select vertex and compute weights" << endl
         << " - Triangles colored by distance (blue=far, red=close)" << endl
         << "Distance Mode:" << endl
         << " - Press 'd' to enter distance visualization mode" << endl
         << " - Click on mesh to select vertex as source" << endl
         << " - Triangles colored by geodesic distance using scalarToRGB" << endl << endl;
}

void usage () {
    printUsage ();
    exit (EXIT_FAILURE);
}



// ------------------------------------

void initLight () {
    GLfloat light_position1[4] = {22.0f, 16.0f, 50.0f, 0.0f};
    GLfloat direction1[3] = {-52.0f,-16.0f,-50.0f};
    GLfloat color1[4] = {1, 1,1, 1};
    GLfloat ambient[4] = {1, 1, 1, 1};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    viewerState = ViewerState_NORMAL;
    selectionToolState = SelectionTool_Rectangle;
    camera.resize (SCREENWIDTH, SCREENHEIGHT);
    initLight ();
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor (0.9f, 0.9f, 0.9f, 1.0f);

    testlinearSystem();
}





void calc_RGB( float val , float val_min , float val_max , float & r , float & g , float & b ) {
    // define uniform color intervalls [v0,v1,v2,v3,v4]
    float v0 = val_min ,
            v1 = val_min + 1.0/4.0 * (val_max - val_min),
            v2 = val_min + 2.0/4.0 * (val_max - val_min),
            v3 = val_min + 3.0/4.0 * (val_max - val_min),
            v4 = val_max ;

    if (val < v0) {
        r = 0.f;  g = 0.f;  b = 1.f; return;
    }
    else if (val > v4) {
        r = 1.f;  g = 0.f;  b = 0.f; return;
    }
    else if (val <= v2) {
        if (val <= v1) { // [v0, v1]
            r = 0.f;  g = (val - v0) / (v1 - v0);  b = 1.f; return;
        }
        else { // ]v1, v2]
            r = 0.f;   g = 1.f;  b = 1.f - (val - v1) / (v2 - v1); return;
        }
    }
    else {
        if (val <= v3) { // ]v2, v3]
            r = (val - v2) / (v3 - v2);  g = 1.f;  b = 0.f; return;
        }
        else { // ]v3, v4]
            r = 1.f;  g = 1.f - (val - v3) / (v4 - v3);  b = 0.f; return;
        }
    }
}
void drawSphere(float x,float y,float z,float radius,int slices,int stacks)
{
    if(stacks < 2){stacks = 2;}
    if(stacks > 30){stacks = 30;}
    if(slices < 3){slices = 3;}
    if(slices > 30){slices = 30;}
    //Pas essentiel ...

    int Nb = slices*stacks +2;
    std::vector< Vec3 > points(Nb);

    Vec3 centre(x,y,z);

    float sinP , cosP , sinT , cosT , Phi , Theta;
    points[0] = Vec3(0,0,1);
    points[Nb-1] = Vec3(0,0,-1);

    for(int i=1; i<=stacks; i++)
    {
        Phi = 90 - (float)(i*180)/(float)(stacks+1);
        sinP = sinf(Phi*3.14159265/180);
        cosP = cosf(Phi*3.14159265/180);

        for(int j=1; j<=slices; j++)
        {
            Theta = (float)(j*360)/(float)(slices);
            sinT = sinf(Theta*3.14159265/180);
            cosT = cosf(Theta*3.14159265/180);

            points[ j + (i-1)*slices ] = Vec3(cosT*cosP,sinT*cosP,sinP);
        }
    }

    int k1,k2;
    glBegin(GL_TRIANGLES);
    for(int i=1; i<=slices; i++)
    {
        k1 = i;
        k2 = (i%slices+1);
        glNormal(points[0]);
        glVertex((centre + radius*points[0]));
        glNormal(points[k1]);
        glVertex((centre + radius*points[k1]));
        glNormal(points[k2]);
        glVertex((centre + radius*points[k2]));

        k1 = (stacks-1)*slices+i;
        k2 = (stacks-1)*slices+(i%slices+1);
        glNormal(points[k1]);
        glVertex((centre + radius*points[k1]));
        glNormal(points[Nb-1]);
        glVertex((centre + radius*points[Nb-1]));
        glNormal(points[k2]);
        glVertex((centre + radius*points[k2]));
    }
    glEnd();

    glBegin(GL_QUADS);
    for(int j=1; j<stacks; j++)
    {
        for(int i=1; i<=slices; i++)
        {
            k1 = i + (j-1)*slices;
            k2 = (i%slices+1) + (j-1)*slices;
            glNormal(points[k2]);
            glVertex((centre + radius*points[k2]));
            glNormal(points[k1]);
            glVertex((centre + radius*points[k1]));

            k1 = i + (j)*slices;
            k2 = (i%slices+1) + (j)*slices;
            glNormal(points[k1]);
            glVertex((centre + radius*points[k1]));
            glNormal(points[k2]);
            glVertex((centre + radius*points[k2]));
        }
    }
    glEnd();
}

// Fonction pour dessiner le chemin A*
void drawPath() {
    if (!showPath || currentPath.size() < 2) return;
    
    glDisable(GL_LIGHTING);
    glLineWidth(5.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge pour le chemin
    
    glBegin(GL_LINE_STRIP);
    for (int vertexId : currentPath) {
        if (vertexId >= 0 && vertexId < (int)mesh.V.size()) {
            const Vec3& p = mesh.V[vertexId].p;
            glVertex3f(p[0], p[1], p[2]);
        }
    }
    glEnd();
    
    // Dessiner les points de départ et d'arrivée
    if (pathStartVertex >= 0 && pathStartVertex < (int)mesh.V.size()) {
        glColor3f(0.0f, 1.0f, 0.0f); // Vert pour le départ
        const Vec3& start = mesh.V[pathStartVertex].p;
        drawSphere(start[0], start[1], start[2], spheresSize * 2, 10, 10);
    }
    
    if (pathEndVertex >= 0 && pathEndVertex < (int)mesh.V.size()) {
        glColor3f(1.0f, 1.0f, 0.0f); // Jaune pour l'arrivée
        const Vec3& end = mesh.V[pathEndVertex].p;
        drawSphere(end[0], end[1], end[2], spheresSize * 2, 10, 10);
    }
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// Fonction pour initialiser A* avec le mesh
void initAStar() {
    aStar.buildFromMesh(mesh);
    aStar.printStats();
}

// Fonction pour trouver le sommet le plus proche d'un point 3D
int findNearestVertex(const Vec3& point) {
    if (mesh.V.empty()) return -1;
    
    int nearest = 0;
    float minDist = (mesh.V[0].p - point).length();
    
    for (unsigned int i = 1; i < mesh.V.size(); i++) {
        float dist = (mesh.V[i].p - point).length();
        if (dist < minDist) {
            minDist = dist;
            nearest = i;
        }
    }
    
    return nearest;
}

// Fonction pour calculer un chemin A*
void computeAStarPath(int start, int end) {
    if (start < 0 || end < 0 || start >= (int)mesh.V.size() || end >= (int)mesh.V.size()) {
        std::cout << "Invalid vertex indices for A* path!" << std::endl;
        return;
    }
    
    std::cout << "Computing A* path from vertex " << start << " to vertex " << end << "..." << std::endl;
    
    currentPath = aStar.findPath(start, end);
    pathStartVertex = start;
    pathEndVertex = end;
    showPath = true;
    
    if (currentPath.empty()) {
        std::cout << "No path found!" << std::endl;
    } else {
        std::cout << "Path found with " << currentPath.size() << " vertices" << std::endl;
        std::cout << "Path: ";
        for (int i = 0; i < (int)currentPath.size(); i++) {
            std::cout << currentPath[i];
            if (i < (int)currentPath.size() - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
}

// Fonction pour calculer les poids des triangles basés sur la distance depuis un sommet
void computeTriangleWeights(int sourceVertex) {
    if (sourceVertex < 0 || sourceVertex >= (int)mesh.V.size()) {
        return;
    }
    
    triangleWeights.clear();
    triangleWeights.resize(mesh.T.size(), 0.0f);
    
    // Calculer les distances depuis le sommet source vers tous les autres sommets
    std::vector<float> vertexDistances(mesh.V.size(), std::numeric_limits<float>::max());
    std::vector<bool> visited(mesh.V.size(), false);
    
    // Dijkstra simplifié pour calculer les distances géodésiques
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    
    vertexDistances[sourceVertex] = 0.0f;
    pq.push({0.0f, sourceVertex});
    
    while (!pq.empty()) {
        float dist = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        
        if (visited[u]) continue;
        visited[u] = true;
        
        // Parcourir les voisins dans le graphe A*
        std::vector<int> neighbors = aStar.getNeighbors(u);
        for (int v : neighbors) {
            float edgeWeight = (mesh.V[u].p - mesh.V[v].p).length();
            float newDist = dist + edgeWeight;
            
            if (newDist < vertexDistances[v]) {
                vertexDistances[v] = newDist;
                pq.push({newDist, v});
            }
        }
    }
    
    // Trouver la distance maximale pour normalisation
    float maxDistance = 0.0f;
    for (float d : vertexDistances) {
        if (d != std::numeric_limits<float>::max() && d > maxDistance) {
            maxDistance = d;
        }
    }
    
    // Calculer le poids de chaque triangle comme la moyenne des distances de ses sommets
    for (unsigned int i = 0; i < mesh.T.size(); i++) {
        float avgDistance = 0.0f;
        int validVertices = 0;
        
        for (int j = 0; j < 3; j++) {
            int vertexId = mesh.T[i].v[j];
            if (vertexDistances[vertexId] != std::numeric_limits<float>::max()) {
                avgDistance += vertexDistances[vertexId];
                validVertices++;
            }
        }
        
        if (validVertices > 0) {
            avgDistance /= validVertices;
            // Normaliser entre 0.0 et 1.0 (inversé pour que proche = poids élevé)
            triangleWeights[i] = maxDistance > 0 ? (1.0f - avgDistance / maxDistance) : 1.0f;
        } else {
            triangleWeights[i] = 0.0f;
        }
    }
    
    weightsComputed = true;
    std::cout << "Triangle weights computed from vertex " << sourceVertex << std::endl;
}

// Fonction pour calculer les distances normalisées et les couleurs des triangles
void computeDistanceVisualization(int sourceVertex) {
    if (sourceVertex < 0 || sourceVertex >= (int)mesh.V.size()) {
        return;
    }
    
    // Réinitialiser les conteneurs
    vertexDistances.clear();
    vertexDistances.resize(mesh.V.size(), std::numeric_limits<float>::max());
    triangleColors.clear();
    triangleColors.resize(mesh.T.size());
    
    // Calculer les distances géodésiques avec Dijkstra
    std::vector<bool> visited(mesh.V.size(), false);
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    
    vertexDistances[sourceVertex] = 0.0f;
    pq.push({0.0f, sourceVertex});
    
    while (!pq.empty()) {
        float dist = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        
        if (visited[u]) continue;
        visited[u] = true;
        
        // Parcourir les voisins
        std::vector<int> neighbors = aStar.getNeighbors(u);
        for (int v : neighbors) {
            float edgeWeight = (mesh.V[u].p - mesh.V[v].p).length();
            float newDist = dist + edgeWeight;
            
            if (newDist < vertexDistances[v]) {
                vertexDistances[v] = newDist;
                pq.push({newDist, v});
            }
        }
    }
    
    // Trouver la distance maximale pour normalisation
    float maxDistance = 0.0f;
    for (float d : vertexDistances) {
        if (d != std::numeric_limits<float>::max() && d > maxDistance) {
            maxDistance = d;
        }
    }
    
    // Normaliser les distances entre 0.0 et 1.0
    if (maxDistance > 0) {
        for (float& d : vertexDistances) {
            if (d != std::numeric_limits<float>::max()) {
                d = d / maxDistance;
            } else {
                d = 1.0f; // Distance maximale pour les sommets non connectés
            }
        }
    }
    
    // Calculer la couleur de chaque triangle basée sur la distance moyenne de ses sommets
    for (unsigned int i = 0; i < mesh.T.size(); i++) {
        float avgDistance = 0.0f;
        int validVertices = 0;
        
        for (int j = 0; j < 3; j++) {
            int vertexId = mesh.T[i].v[j];
            if (vertexDistances[vertexId] != std::numeric_limits<float>::max()) {
                avgDistance += vertexDistances[vertexId];
                validVertices++;
            }
        }
        
        if (validVertices > 0) {
            avgDistance /= validVertices;
            // Utiliser scalarToRGB pour obtenir la couleur
            triangleColors[i] = scalarToRGB(avgDistance);
        } else {
            // Couleur par défaut pour les triangles non connectés
            triangleColors[i] = (RGB){.r = 0.5f, .g = 0.5f, .b = 0.5f};
        }
    }
    
    distancesComputed = true;
    std::cout << "Distance visualization computed from vertex " << sourceVertex << std::endl;
    std::cout << "Max distance: " << maxDistance << std::endl;
}

// Fonction pour obtenir les coordonnées 3D à partir des coordonnées écran
Vec3 getWorldCoordinates(int x, int y) {
    // Conversion des coordonnées écran vers le monde 3D
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    winX = (float)x;
    winY = (float)viewport[3] - (float)y; // Inverser Y
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    
    return Vec3(posX, posY, posZ);
}

// Fonction pour sélectionner le sommet le plus proche du clic
int selectVertexFromClick(int x, int y) {
    Vec3 clickPos = getWorldCoordinates(x, y);
    return findNearestVertex(clickPos);
}

// Fonction pour dessiner le mesh avec les poids des triangles
void drawMeshWithWeights() {
    if (!weightsComputed || triangleWeights.size() != mesh.T.size()) {
        // Dessiner normalement si pas de poids
        glColor3f(0.4, 0.4, 0.8);
        mesh.draw();
        return;
    }
    
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < mesh.T.size(); i++) {
        // Utiliser le poids pour définir la couleur
        float weight = triangleWeights[i];
        
        // Gradient de couleur : bleu (poids faible) vers rouge (poids élevé)
        float r = weight;           // Rouge augmente avec le poids
        float g = 0.2f;            // Vert constant
        float b = 1.0f - weight;   // Bleu diminue avec le poids
        
        glColor3f(r, g, b);
        
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex& v = mesh.V[mesh.T[i].v[j]];
            glNormal3f(v.n[0], v.n[1], v.n[2]);
            glVertex3f(v.p[0], v.p[1], v.p[2]);
        }
    }
    glEnd();
}

// Fonction pour dessiner le mesh avec les couleurs de distance
void drawMeshWithDistanceColors() {
    if (!distancesComputed || triangleColors.size() != mesh.T.size()) {
        // Dessiner normalement si pas de couleurs calculées
        glColor3f(0.4, 0.4, 0.8);
        mesh.draw();
        return;
    }
    
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < mesh.T.size(); i++) {
        // Utiliser la couleur précalculée pour ce triangle
        RGB color = triangleColors[i];
        glColor3f(color.r, color.g, color.b);
        
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex& v = mesh.V[mesh.T[i].v[j]];
            glNormal3f(v.n[0], v.n[1], v.n[2]);
            glVertex3f(v.p[0], v.p[1], v.p[2]);
        }
    }
    glEnd();
}

void drawHandles() {
    glEnable(GL_LIGHTING);
    glColor3f(0.2,0.2,0.2);
    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        Vec3 const & p = mesh.V[ v ].p;
        if(verticesAreMarkedForCurrentHandle[ v ])
            drawSphere( p[0] , p[1] , p[2] , spheresSize , 10 , 10 );
    }

    for( unsigned int v = 0 ; v < mesh.V.size() ; ++v ) {
        Vec3 const & p = mesh.V[ v ].p;
        if(! verticesAreMarkedForCurrentHandle[ v ]) {
            int handleIdx = verticesHandles[v];
            if( handleIdx >= 0 ) {
                float r , g , b;
                calc_RGB( handleIdx , 0 , numberOfHandles , r , g  , b );
                if(handleIdx != activeHandle) {
                    r *= 0.5;  g *= 0.5;  b *= 0.5;
                }
                glColor3f(r,g,b);
                drawSphere( p[0] , p[1] , p[2] , spheresSize , 10 , 10 );
            }
        }
    }
}


void draw () {
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    
    // Dessiner le mesh selon le mode actif
    if (distanceVisualizationMode && distancesComputed) {
        drawMeshWithDistanceColors();
    } else if (astarModeEnabled && weightsComputed) {
        drawMeshWithWeights();
    } else {
        glColor3f(0.4,0.4,0.8); //color mesh
        mesh.draw();
    }
    
    //si on veux voir les points sélectionnées
    // if ((astarModeEnabled || distanceVisualizationMode) && selectedAStarVertex >= 0 && selectedAStarVertex < (int)mesh.V.size()) {
    //     glDisable(GL_LIGHTING);
    //     glColor3f(1.0f, 1.0f, 0.0f); // Jaune pour le sommet sélectionné
    //     const Vec3& pos = mesh.V[selectedAStarVertex].p;
    //     drawSphere(pos[0], pos[1], pos[2], spheresSize * 3, 10, 10);
    //     glEnable(GL_LIGHTING);
    // }
    
    drawHandles();
    drawPath();  // Dessiner le chemin A*
    rectangleSelectionTool.draw();
    sphereSelectionTool.draw();
}

void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    glFlush ();
    glutSwapBuffers ();
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [64];
        sprintf (winTitle, "gmini - FPS: %d", FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay ();
}


void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:
        if( viewerState == ViewerState_TRANSLATINGHANDLE ) {
            Vec3 rv = getUpVector();
            translateActiveHandle( - 0.05 * rv );
        }
        else if( viewerState == ViewerState_ROTATINGHANDLE ) {
            Vec3 vv = getViewVector();
            rotateActiveHandle( vv , - M_PI / 30 );
        }
        else if( viewerState == ViewerState_NORMAL ) {
            if(activeHandle >= 0) {
                --activeHandle;
            }
        }
        break;
    case GLUT_KEY_UP:
        if( viewerState == ViewerState_TRANSLATINGHANDLE ) {
            Vec3 rv = getUpVector();
            translateActiveHandle( 0.05 * rv );
        }
        else if( viewerState == ViewerState_ROTATINGHANDLE ) {
            Vec3 vv = getViewVector();
            rotateActiveHandle( vv , M_PI / 30 );
        }
        else if( viewerState == ViewerState_NORMAL ) {
            if(activeHandle < numberOfHandles - 1) {
                ++activeHandle;
            }
        }
        break;
    case GLUT_KEY_LEFT:
        if( viewerState == ViewerState_TRANSLATINGHANDLE ) {
            Vec3 rv = getRightVector();
            translateActiveHandle( - 0.05 * rv );
        }
        else if( viewerState == ViewerState_ROTATINGHANDLE ) {
            Vec3 vv = getViewVector();
            rotateActiveHandle( vv , - M_PI / 30 );
        }
        else if( viewerState == ViewerState_NORMAL ) {
            if(activeHandle >= 0) {
                --activeHandle;
            }
        }
        break;
    case GLUT_KEY_RIGHT:
        if( viewerState == ViewerState_TRANSLATINGHANDLE ) {
            Vec3 rv = getRightVector();
            translateActiveHandle( 0.05 * rv );
        }
        else if( viewerState == ViewerState_ROTATINGHANDLE ) {
            Vec3 vv = getViewVector();
            rotateActiveHandle( vv , M_PI / 30 );
        }
        else if( viewerState == ViewerState_NORMAL ) {
            if(activeHandle < numberOfHandles - 1) {
                ++activeHandle;
            }
        }
        break;
    }
}


void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if( viewerState == ViewerState_NORMAL ) {
            if (fullScreen == true) {
                glutReshapeWindow (SCREENWIDTH, SCREENHEIGHT);
                fullScreen = false;
            } else {
                glutFullScreen ();
                fullScreen = true;
            }
        }
        break;

    case 'w':
        if( viewerState == ViewerState_NORMAL ) {
            GLint polygonMode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygonMode);
            if(polygonMode[0] != GL_FILL)
                glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        }
        break;

    case GLUT_KEY_ENTER:
        if( viewerState == ViewerState_EDITINGHANDLE ) {
            viewerState = ViewerState_NORMAL;
            finalizeEditingOfCurrentHandle();
        }
        break;

    case GLUT_KEY_ESCAPE:
        if( viewerState == ViewerState_TRANSLATINGHANDLE   ||   viewerState == ViewerState_ROTATINGHANDLE ) {
            viewerState = ViewerState_NORMAL;
        }
        break;

    case 'n':
        if( viewerState == ViewerState_NORMAL ) {
            viewerState = ViewerState_EDITINGHANDLE;
            ++numberOfHandles;
            activeHandle = numberOfHandles - 1; // last handle
            printf("        Editing handle %d\n",activeHandle);
        }
        break;

    case 'g':
        if( viewerState == ViewerState_NORMAL   ||   viewerState == ViewerState_ROTATINGHANDLE ) {
            if( activeHandleIsValid() ) {
                viewerState = ViewerState_TRANSLATINGHANDLE;
                printf("    Translating handle %d\n",activeHandle);
            }
        }
        break;

    case 'r':
        if( viewerState == ViewerState_NORMAL   ||   viewerState == ViewerState_TRANSLATINGHANDLE ) {
            if( activeHandleIsValid() ) {
                viewerState = ViewerState_ROTATINGHANDLE;
                printf("    Rotating handle %d\n",activeHandle);
            }
        }
        break;

    case 's':
        if(selectionToolState == SelectionTool_Rectangle)
        {
            selectionToolState = SelectionTool_Sphere;
            printf("Sphere selection tool activated (scroll to change radius)\n");
        }
        else if(selectionToolState == SelectionTool_Sphere)
        {
            selectionToolState = SelectionTool_Rectangle;
            printf("Rectangle selection tool activated\n");
        }
        break;

    case 'm':
        // Toggle mode A* interactif
        astarModeEnabled = !astarModeEnabled;
        if (astarModeEnabled) {
            viewerState = ViewerState_ASTAR_MODE;
            printf("A* interactive mode ENABLED - Click on a vertex to compute weights\n");
        } else {
            viewerState = ViewerState_NORMAL;
            selectedAStarVertex = -1;
            weightsComputed = false;
            printf("A* interactive mode DISABLED\n");
        }
        break;

    case 'd':
        // Toggle mode visualisation des distances
        distanceVisualizationMode = !distanceVisualizationMode;
        if (distanceVisualizationMode) {
            // Désactiver le mode A* s'il était actif
            astarModeEnabled = false;
            viewerState = ViewerState_NORMAL;
            printf("Distance visualization mode ENABLED - Click on a vertex to see distances\n");
        } else {
            selectedAStarVertex = -1;
            distancesComputed = false;
            printf("Distance visualization mode DISABLED\n");
        }
        break;

    case 'D':
        // fusion de d et de la sphère afin de sélectionner uniquement les handles géodésiques
        //donc il faut que la disantance des handles soit plus petite que radius au point centre de la sphère
        geodesicDistancesComputed = !geodesicDistancesComputed;
        printf("Geodesic distances for handle selection: %s\n", geodesicDistancesComputed ? "ON" : "OFF");
        break;

    case 'a':
        // Test A* entre deux sommets aléatoires
        if (viewerState == ViewerState_NORMAL && mesh.V.size() > 1) {
            int start = rand() % mesh.V.size();
            int end = rand() % mesh.V.size();
            while (end == start && mesh.V.size() > 1) {
                end = rand() % mesh.V.size();
            }
            computeAStarPath(start, end);
            printf("A* path computed from vertex %d to vertex %d\n", start, end);
        }
        break;

    case 'A':
        // Test A* spécifique (sommets 0 et N/2)
        if (viewerState == ViewerState_NORMAL && mesh.V.size() > 1) {
            int start = 0;
            int end = mesh.V.size() / 2;
            computeAStarPath(start, end);
            printf("A* path computed from vertex %d to vertex %d\n", start, end);
        }
        break;

    case 'p':
        // Toggle affichage du chemin
        showPath = !showPath;
        printf("Path display: %s\n", showPath ? "ON" : "OFF");
        break;

    default:
        printUsage ();
        break;
    }
    idle ();
}


void mouse (int button, int state, int x, int y) {
    // Gestion du mode A* interactif
    if (astarModeEnabled && state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        int clickedVertex = selectVertexFromClick(x, y);
        if (clickedVertex >= 0) {
            selectedAStarVertex = clickedVertex;
            computeTriangleWeights(clickedVertex);
            std::cout << "Selected vertex " << clickedVertex << " for A* mode" << std::endl;
            idle();
            return;
        }
    }
    
    // Gestion du mode visualisation des distances
    if (distanceVisualizationMode && state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        int clickedVertex = selectVertexFromClick(x, y);
        if (clickedVertex >= 0) {
            selectedAStarVertex = clickedVertex;
            computeDistanceVisualization(clickedVertex);
            std::cout << "Selected vertex " << clickedVertex << " for distance visualization" << std::endl;
            idle();
            return;
        }
    }
    
    if( glutGetModifiers() & GLUT_ACTIVE_CTRL    ||   rectangleSelectionTool.isActive ) { // we can activate the selection only with ctrl pressed
        if( viewerState == ViewerState_EDITINGHANDLE ) {
            if (state == GLUT_UP) {
                // then the mouse is released, confirm rectangle editing
                rectangleSelectionTool.isActive = false;
                addVerticesToCurrentHandle();
            } else {
                if (button == GLUT_LEFT_BUTTON) {
                    if(selectionToolState == SelectionTool_Rectangle)
                    {
                        rectangleSelectionTool.initRectangle(x,y);
                        rectangleSelectionTool.isAdding = true;
                        rectangleSelectionTool.isActive = true;
                    }
                    else if(selectionToolState == SelectionTool_Sphere)
                    {
                       float posX,posY,posZ;
                        get3DPosFromMouseInput(x,y, posX, posY, posZ);
                        Vec3 pos(posX, posY, posZ);
                        sphereSelectionTool.initSphere(pos, selectionRadius);
                        sphereSelectionTool.isAdding = true;
                        sphereSelectionTool.isActive = true;
                    }
                } else if (button == GLUT_RIGHT_BUTTON) {
                    if(selectionToolState == SelectionTool_Rectangle)
                    {
                        rectangleSelectionTool.initRectangle(x,y);
                        rectangleSelectionTool.isAdding = false;
                        rectangleSelectionTool.isActive = true;
                    }
                    else if(selectionToolState == SelectionTool_Sphere)
                    {
                        float posX,posY,posZ;
                        get3DPosFromMouseInput(x,y, posX, posY, posZ);
                        Vec3 pos(posX, posY, posZ);
                        sphereSelectionTool.initSphere(pos, selectionRadius);
                        sphereSelectionTool.isAdding = false;
                        sphereSelectionTool.isActive = true;
                    }
                }
            }
        }
    }
    else {
        // moving the camera:
        if (state == GLUT_UP) {
            mouseMovePressed = false;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else {
            if (button == GLUT_LEFT_BUTTON) {
                camera.beginRotate (x, y);
                mouseMovePressed = false;
                mouseRotatePressed = true;
                mouseZoomPressed = false;
            } else if (button == GLUT_RIGHT_BUTTON) {
                lastX = x;
                lastY = y;
                mouseMovePressed = true;
                mouseRotatePressed = false;
                mouseZoomPressed = false;
            } else if (button == GLUT_MIDDLE_BUTTON) {
                if (mouseZoomPressed == false) {
                    lastZoom = y;
                    mouseMovePressed = false;
                    mouseRotatePressed = false;
                    mouseZoomPressed = true;
                }
            }
            updateSphereRadiusWithScroll(button);
        }
    }
    idle ();
}

void motion (int x, int y) {
    if( viewerState == ViewerState_EDITINGHANDLE  &&  rectangleSelectionTool.isActive ) {
        rectangleSelectionTool.updateRectangle(x,y);
    }
    else {
        // moving the camera:
        if (mouseRotatePressed == true) {
            camera.rotate (x, y);
        }
        else if (mouseMovePressed == true) {
            camera.move ((x-lastX)/static_cast<float>(SCREENWIDTH), (lastY-y)/static_cast<float>(SCREENHEIGHT), 0.0);
            lastX = x;
            lastY = y;
        }
        else if (mouseZoomPressed == true) {
            camera.zoom (float (y-lastZoom)/SCREENHEIGHT);
            lastZoom = y;
        }
    }
}


void reshape(int w, int h) {
    camera.resize (w, h);
}


int main (int argc, char ** argv) {
    if (argc > 2) {
        printUsage ();
        exit (EXIT_FAILURE);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCREENWIDTH, SCREENHEIGHT);
    window = glutCreateWindow ("arap tp");

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    glutSpecialFunc(SpecialInput);
    key ('?', 0, 0);

    mesh.loadOFF(argc == 2 ? argv[1] : "models/arma.off");
    verticesAreMarkedForCurrentHandle.resize( mesh.V.size() , false );
    verticesHandles.resize( mesh.V.size() , -1 );
    edgeAndVertexWeights.buildCotangentWeightsOfTriangleMesh( mesh);
    Eigen::MatrixXd idMatrix(3,3);
    idMatrix(0,0) = 1.0;   idMatrix(1,1) = 1.0;   idMatrix(2,2) = 1.0;
    vertexRotationMatrices.resize( mesh.V.size() , idMatrix );
    
    // Initialiser A* avec le mesh
    initAStar();

    glutMainLoop ();
    return EXIT_SUCCESS;
}

