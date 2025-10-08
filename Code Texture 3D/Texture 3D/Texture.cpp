#include <iostream>
#include <fstream>
#include <algorithm>
#include <QFile>
#include <QTextStream>


#include "Texture.h"

#include <complex>


Texture::Texture(QOpenGLContext* context)
{

    glContext = context;
    init();
    initGLSL();

}

Texture::~Texture(){
    if(textureCreated)
        deleteTexture();
}

void Texture::init(){

    /***********************************************************************/
    //Default values
    /***********************************************************************/
    //x, y, z cutting plane for the 3D texture
    xCutPosition = 1.;
    yCutPosition = 1.;
    zCutPosition = 1.;

    //x, y, z cut direction
    xCutDirection = 1.;
    yCutDirection = 1.;
    zCutDirection = 1.;

    xCutDisplay = false;
    yCutDisplay = false;
    zCutDisplay = false;

    //Set texture to cube of size 1.
    xMax = 1.;
    yMax = 1.;
    zMax = 1.;

    gridSize = 0;

    d[0] = 0;
    d[1] = 0;
    d[2] = 0;

    n[0] = 0;
    n[1] = 0;
    n[2] = 0;

    textureCreated = false;

}

void Texture::recompileShaders() {
    std::string path = "GLSL/shaders/";
    std::string vShaderPath = path + "volume.vert";
    std::string fShaderPath = path + "volume.frag";

    glFunctions = glContext->extraFunctions();
    glEnable( GL_DEBUG_OUTPUT );
    glFunctions->glDebugMessageCallback(&Texture::MessageCallback, 0 );

    // Create programs and link shaders
    this->programID = glFunctions->glCreateProgram();
    std::string content = readShaderSource(vShaderPath);
    if (!content.empty()) {
        this->vShader = glFunctions->glCreateShader(GL_VERTEX_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->vShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->vShader);
        glFunctions->glAttachShader(this->programID, this->vShader);
        printShaderErrors(this->vShader);
    }
    content = readShaderSource(fShaderPath);
    if (!content.empty()) {
        this->fShader = glFunctions->glCreateShader(GL_FRAGMENT_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->fShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->fShader);
        glFunctions->glAttachShader(this->programID, this->fShader);
        printShaderErrors(this->fShader);
    }

    glFunctions->glLinkProgram(this->programID);
    glFunctions->glUseProgram(programID);
    printProgramErrors(programID);
    checkOpenGLError();

    initTexture();
}

void Texture::initGLSL(){
    std::string path = "GLSL/shaders/";
    std::string vShaderPath = path + "volume.vert";
    std::string fShaderPath = path + "volume.frag";

    glFunctions = glContext->extraFunctions();
    glEnable( GL_DEBUG_OUTPUT );
    glFunctions->glDebugMessageCallback(&Texture::MessageCallback, 0 );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_3D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Create programs and link shaders
    this->programID = glFunctions->glCreateProgram();
    std::string content = readShaderSource(vShaderPath);
    if (!content.empty()) {
        this->vShader = glFunctions->glCreateShader(GL_VERTEX_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->vShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->vShader);
        glFunctions->glAttachShader(this->programID, this->vShader);
        printShaderErrors(this->vShader);
    }
    content = readShaderSource(fShaderPath);
    if (!content.empty()) {
        this->fShader = glFunctions->glCreateShader(GL_FRAGMENT_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->fShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->fShader);
        glFunctions->glAttachShader(this->programID, this->fShader);
        printShaderErrors(this->fShader);
    }

    glFunctions->glLinkProgram(this->programID);
    glFunctions->glUseProgram(programID);
    printProgramErrors(programID);
    checkOpenGLError();

    initTexture();
}

void /*GLAPIENTRY */Texture::MessageCallback( GLenum source, GLenum type,
                                              GLuint id, GLenum severity,
                                              GLsizei length, const GLchar* message,
                                              const void* userParam )
{
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_LOW) {
        std::string s_severity = (severity == GL_DEBUG_SEVERITY_HIGH ? "High" : severity == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" : "Low");
        std::cout << "Error " << id << " [severity=" << s_severity << "]: " << message << std::endl;
    }
}
bool Texture::checkOpenGLError()
{
    bool error = false;
    int glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
        std::cout << "[OpenGL] Error: " << glErr << std::endl;
        error = true;
        glErr = glGetError();
    }
    return !error;
}

bool Texture::printShaderErrors(GLuint shader)
{
    int state = 0;
    glFunctions->glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetShaderInfoLog(shader, len, &chWritten, log);
        std::cout << "[OpenGL] Shader error: " << log << std::endl;
        free(log);
    }
    return false;
}
bool Texture::printProgramErrors(int program)
{
    int state = 0;
    glFunctions->glGetProgramiv(program, GL_LINK_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetProgramInfoLog(program, len, &chWritten, log);
        std::cout << "[OpenGL] Program error: " << log << std::endl;
        free(log);
    }
    return false;
}

std::string Texture::readShaderSource(std::string filename)
{
    std::string content = "";
    QString qFilename = QString::fromStdString(filename);
    if (!QFile::exists(qFilename))
        qFilename = ":" + qFilename;
    if (!QFile::exists(qFilename)) {
        std::cerr << "The shader " << filename << " doesn't exist!" << std::endl;
        return "";
    }
    QFile file(qFilename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    std::string line;
    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine().toStdString();
        content += line + " \n";
    }
    file.close();
    return content;
}


void Texture::initTexture(){


    if(textureId != 0){
        glDeleteTextures(1, &textureId);
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_3D, textureId);

	//TODO complete texture options




    textureCreated = true;
}


void Texture::deleteTexture(){
    glDeleteTextures( 1, &textureId);
}

void Texture::draw( const qglviewer::Camera * camera ){
    glFunctions->glUseProgram(programID);

    if(!textureCreated)
        return;

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_3D);

    glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );

    // GPU start
    // Récuperation des matrices de projection / vue-modèle

    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix(pMatrix);
    camera->getModelViewMatrix(mvMatrix);
    glFunctions->glUniformMatrix4fv(glFunctions->glGetUniformLocation(programID, "proj_matrix"),
                                    1, GL_FALSE, pMatrix);
    glFunctions->glUniformMatrix4fv(glFunctions->glGetUniformLocation(programID, "mv_matrix"),
                                    1, GL_FALSE, mvMatrix);


    ///***********************************************************************/
    ////Parameters to given to the shader // TODO complete
    /***********************************************************************/





    /***********************************************************************/

    drawBoundingBox(false);
    drawCutPlanes();

}

void Texture::drawCube(){
    glBegin(GL_QUADS);

    glVertex3f(0.0f, 0.0f, 0.0f);	// Bottom Right Of The Texture and Quad
    glVertex3f(0.0f, yMax, 0.0f);	// Top Right Of The Texture and Quad
    glVertex3f(xMax, yMax, 0.0f);	// Top Left Of The Texture and Quad
    glVertex3f(xMax, 0.0f, 0.0f);	// Bottom Left Of The Texture and Quad
    // Bottom Face
    glVertex3f(0.0f, 0.0f, 0.0f);	// Top Right Of The Texture and Quad
    glVertex3f(xMax, 0.0f, 0.0f);	// Top Left Of The Texture and Quad
    glVertex3f(xMax, 0.0f, zMax);	// Bottom Left Of The Texture and Quad
    glVertex3f(0.0f, 0.0f, zMax);	// Bottom Right Of The Texture and Quad
    // Left Face
    glVertex3f(0.0f, 0.0f, 0.0f);	// Bottom Left Of The Texture and Quad
    glVertex3f(0.0f, 0.0f, zMax);	// Bottom Right Of The Texture and Quad
    glVertex3f(0.0f, yMax, zMax);	// Top Right Of The Texture and Quad
    glVertex3f(0.0f, yMax, 0.0f);	// Top Left Of The Texture and Quad
    // Right face
    glVertex3f(xMax, 0.0f, 0.0f);	// Bottom Right Of The Texture and Quad
    glVertex3f(xMax, yMax, 0.0f);	// Top Right Of The Texture and Quad
    glVertex3f(xMax, yMax, zMax);	// Top Left Of The Texture and Quad
    glVertex3f(xMax, 0.0f,  zMax);	// Bottom Left Of The Texture and Quad

    // Front Face
    glVertex3f(0.0f, 0.0f, zMax);	// Bottom Left Of The Texture and Quad
    glVertex3f(xMax, 0.0f, zMax);	// Bottom Right Of The Texture and Quad
    glVertex3f(xMax, yMax, zMax);	// Top Right Of The Texture and Quad
    glVertex3f(0.0f,  yMax,  zMax);	// Top Left Of The Texture and Quad

    // Top Face
    glVertex3f(0.0f,  yMax, 0.0f);	// Top Left Of The Texture and Quad
    glVertex3f(0.0f, yMax,  zMax);	// Bottom Left Of The Texture and Quad
    glVertex3f(xMax, yMax, zMax);	// Bottom Right Of The Texture and Quad
    glVertex3f(xMax, yMax, 0.0f);	// Top Right Of The Texture and Quad
    glEnd();
}


void Texture::drawCutPlanes(){

    double x = xCutPosition + xCutDirection*.001;
    double y = yCutPosition + yCutDirection*.001;
    double z = zCutPosition + zCutDirection*.001;

    glColor4f(1.0,0.,0.,0.25);
    glBegin(GL_QUADS);

    if(xCutDisplay){
        // Right face
        glVertex3f( x, 0.0f, 0.0f);	// Bottom Right Of The Texture and Quad
        glVertex3f( x, yMax, 0.0f);	// Top Right Of The Texture and Quad
        glVertex3f( x, yMax, zMax);	// Top Left Of The Texture and Quad
        glVertex3f( x, 0.0f, zMax);	// Bottom Left Of The Texture and Quad
    }

    if(zCutDisplay){
        // Front Face
        glVertex3f(0.0f, 0.0f, z);	// Bottom Left Of The Texture and Quad
        glVertex3f(xMax, 0.0f, z);	// Bottom Right Of The Texture and Quad
        glVertex3f(xMax, yMax, z);	// Top Right Of The Texture and Quad
        glVertex3f(0.0f, yMax, z);	// Top Left Of The Texture and Quad
    }

    if(yCutDisplay){
        // Top Face
        glVertex3f(0.0f, y, 0.0f);	// Top Left Of The Texture and Quad
        glVertex3f(0.0f, y, zMax);	// Bottom Left Of The Texture and Quad
        glVertex3f(xMax, y, zMax);	// Bottom Right Of The Texture and Quad
        glVertex3f(xMax, y, 0.0f);	// Top Right Of The Texture and Quad
    }
    glEnd();


}

void Texture::drawBoundingBox(bool fill){

    glPolygonMode (GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
    glColor3f(1.f,0.f,0.f);
    drawCube();
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

}

void Texture::build(const std::vector<unsigned char> & data, const std::vector<unsigned char> & labels,
                    unsigned int & nx , unsigned int & ny , unsigned int & nz,
                    float & dx , float & dy , float & dz,
                    std::map<unsigned char, QColor> & labelsToColor){

    if(textureCreated)
        deleteTexture();

    n[0] = nx; n[1] = ny; n[2] = nz;
    d[0] = dx; d[1] = dy; d[2] = dz;
    std::cout << "(nx,dx) = ( " << n[0] << " ; " << d[0] << " ) "<< std::endl;
    std::cout << "(ny,dy) = ( " << n[1] << " ; " << d[1] << " ) "<< std::endl;
    std::cout << "(nz,dz) = ( " << n[2] << " ; " << d[2] << " ) "<< std::endl;

    xMax = double(n[0])*d[0];
    yMax = double(n[1])*d[1];
    zMax = double(n[2])*d[2];

    minD = dx;
    minD = std::min(dy, minD); minD = std::min(dz, minD);

    gridSize = n[0]*n[1]*n[2];

    std::cout <<"(xMax, yMax, zMax) =  ( " << xMax << " ; " << yMax << " ; " << zMax << " ) "<< std::endl;

    xCutPosition = xMax;
    yCutPosition = yMax;
    zCutPosition = zMax;


    unsigned int max_id = 0 ;

    for(unsigned int i = 0 ; i < labels.size() ; i ++){
        max_id = std::max((unsigned int)labels[i], max_id);
    }

    Vmin[0] = 3000; Vmin[1] = 3000; Vmin[2] = 3000;
    Vmax[0] = 0; Vmax[1] = 0; Vmax[2] = 0;


    //TODO fill texels with data

    rgbTexture = new unsigned char[n[0]*n[1]*n[2]*4];

    //int greyValue = data[i];
    //QColor color = labelsToColor[greyValue];











    initTexture();
}


void Texture::setXCut(int _xCut){
    xCut = 1.-double(_xCut)/n[0];
    xCutPosition = xMax*xCut;
}

void Texture::setYCut(int _yCut){
    yCut = 1.- double(_yCut)/n[1];
    yCutPosition = yMax*yCut;
}

void Texture::setZCut(int _zCut){
    zCut = 1.0-double(_zCut)/n[2];
    zCutPosition = zMax*zCut;
}

void Texture::clear(){

    if( textureCreated )
        glDeleteTextures(1, &textureId);

    init();

}

