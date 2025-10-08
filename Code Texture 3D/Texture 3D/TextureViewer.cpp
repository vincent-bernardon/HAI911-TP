#include "TextureViewer.h"
#include <cfloat>
#include <QFileDialog>
#include <QGLViewer/manipulatedCameraFrame.h>

using namespace std;
using namespace qglviewer;

TextureViewer::TextureViewer(QWidget *parent):QGLViewer(parent){
}


void TextureViewer::draw(){

    drawClippingPlane();

    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDisable(GL_BLEND);
    
    camera()->setSceneRadius(1000);

    texture->draw(camera());
}

void TextureViewer::drawClippingPlane(){
    
    
    glEnable(GL_LIGHTING);
    
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    

    glDisable(GL_LIGHTING);
    
    GLdouble equation[4];
    glGetClipPlane(GL_CLIP_PLANE0, equation);
    
    qreal p[] = {0.,-equation[3]/equation[1], 0.};
    qreal projP[3];
    camera()->getWorldCoordinatesOf(p, projP);
    
    
    qreal norm[] = {equation[0] + p[0], equation[1]+ p[1], equation[2]+ p[2]};
    qreal normResult[3];
    camera()->getWorldCoordinatesOf(norm, normResult);
    
    Vec3Df normal(normResult[0]-projP[0], normResult[1]-projP[1], normResult[2]-projP[2]);
    Vec3Df point(projP[0], projP[1],projP[2]);
    

}

void TextureViewer::drawMesh() {
    glBegin(GL_TRIANGLES);
    for (const auto& triangle : triangles) {
        for (size_t i = 0; i < 3; ++i) {
            const qglviewer::Vec& vertex = vertices[triangle[i]];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();
}

void TextureViewer::init()
{

    texture = new Texture ( QOpenGLContext::currentContext() );
    
    // The ManipulatedFrame will be used as the clipping plane
    setManipulatedFrame(new ManipulatedFrame());
    
    // Enable plane clipping
    glEnable(GL_CLIP_PLANE0);
    
    //Set background color
    setBackgroundColor(QColor(255,255,255));
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    //Set blend parameters
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    imageLoaded = false;
    
    cut = Vec3Df(0.,0.,0.),
    cutDirection = Vec3Df(1.,1.,1.);
    

}

void TextureViewer::clear(){
    texture->clear();
}


void TextureViewer::updateCamera(const qglviewer::Vec & center, float radius){
    camera()->setSceneCenter(center);
    camera()->setSceneRadius(radius);
    
    camera()->showEntireScene();
}


void TextureViewer::open3DImage(const QString & fileName){
    
    //Texture objet
    texture->clear();
    subdomain_indices.clear();
    std::vector<unsigned char> data;
    unsigned int nx, ny, nz;
    float dx, dy, dz;
    
    
    //Load the data from the 3D image
    if (fileName.endsWith(".dim"))
        openIMA(fileName,data,subdomain_indices, nx,ny,nz,dx,dy,dz);
    else
        return;
    
    for( unsigned int i = 0 ; i < subdomain_indices.size() ; i++ ){
        int currentLabel = subdomain_indices[i];
        
        std::map<unsigned char, QColor>::iterator it = iColorMap.find( currentLabel );
        if( it == iColorMap.end() ){
            if( currentLabel ==0 )
                iColorMap[currentLabel] = QColor(0,0,0);
            else
                iColorMap[currentLabel].setHsvF(0.98*double(i)/subdomain_indices.size(), 0.8,0.8);
        }

        iDisplayMap[currentLabel] = true;
    }
    //iColorMap[0].setAlpha(0);
    texture->build(data,subdomain_indices,nx,ny,nz,dx,dy,dz,iColorMap);
    
    imageLoaded = true;
    
    qglviewer::Vec maxTexture (texture->getXMax(), texture->getYMax() , texture->getZMax());
    
    updateCamera(maxTexture/2. , maxTexture.norm() );

    //Once the 3D image is loaded, grid size parameters are sent to the interface
    emit setMaxCutPlanes(texture->getWidth(), texture->getHeight(), texture->getDepth());
    emit setImageLabels();
}

void TextureViewer::openOffMesh(const QString &fileName) {
    std::cout << "Opening " << fileName.toStdString() << std::endl;

    // open the file
    std::ifstream myfile;
    myfile.open(fileName.toStdString());
    if (!myfile.is_open())
    {
        std::cout << fileName.toStdString() << " cannot be opened" << std::endl;
        return;
    }

    std::string magic_s;

    myfile >> magic_s;

    // check if it's OFF
    if( magic_s != "OFF" )
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        exit(1);
    }

    int n_vertices , n_faces , dummy_int;
    myfile >> n_vertices >> n_faces >> dummy_int;

    // Clear any verticies
    vertices.clear();

    // Read the verticies
    for( int v = 0 ; v < n_vertices ; ++v )
    {
        float x , y , z;
        myfile >> x >> y >> z ;
        vertices.push_back( Vec( x , y , z ) );
    }

    // Clear any triangles
    triangles.clear();

    // Read the triangles
    for( int f = 0 ; f < n_faces ; ++f )
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;
        if( n_vertices_on_face == 3 )
        {
            unsigned int _v1 , _v2 , _v3;
            myfile >> _v1 >> _v2 >> _v3;
            triangles.push_back( {_v1, _v2, _v3} );
        }
        else if( n_vertices_on_face == 4 )
        {
            unsigned int _v1 , _v2 , _v3 , _v4;

            myfile >> _v1 >> _v2 >> _v3 >> _v4;
            triangles.push_back({_v1, _v2, _v3});
            triangles.push_back({_v1, _v3, _v4});
        }
        else
        {
            std::cout << "We handle ONLY *.off files with 3 or 4 vertices per face" << std::endl;
            myfile.close();
            exit(1);
        }
    }

}


std::istream & operator>>(std::istream & stream, qglviewer::Vec & v)
{
    stream >>
            v.x >>
            v.y >>
            v.z;
    
    return stream;
}

void TextureViewer::selectIAll(){
    for(std::map<unsigned char, bool>::iterator it = iDisplayMap.begin() ; it != iDisplayMap.end(); ++it )
        iDisplayMap[it->first] = true;
    update();
}

void TextureViewer::discardIAll(){
    for(std::map<unsigned char, bool>::iterator it = iDisplayMap.begin() ; it != iDisplayMap.end(); ++it )
        iDisplayMap[it->first] = false;
    update();
}

void TextureViewer::setIVisibility(unsigned int i, bool visibility){
    if(iDisplayMap.find(i) != iDisplayMap.end())
        iDisplayMap[i] = visibility;
    update();
}


void TextureViewer::openIMA(const QString & fileName, std::vector<unsigned char> & data, std::vector<unsigned char> & labels,
                            unsigned int & nx , unsigned int & ny , unsigned int & nz, float & dx , float & dy , float & dz ){
    QString imaName = QString(fileName);
    
    imaName.replace(".dim", ".ima" );
    std::ifstream imaFile (imaName.toUtf8());
    if (!imaFile.is_open())
        return;
    
    std::ifstream dimFile (fileName.toUtf8());
    if (!dimFile.is_open())
        return;
    
    dimFile >> nx; dimFile >> ny; dimFile >> nz;
    
    string dummy, type;
    
    dimFile >> dummy;
    while (dummy.find("-type")==string::npos)
        dimFile >> dummy;
    
    dimFile >> type;
    
    while (dummy.find("-dx")==string::npos)
        dimFile >> dummy;
    
    dimFile >> dx;
    
    dimFile >> dummy;
    while (dummy.find("-dy")==string::npos)
        dimFile >> dummy;
    
    dimFile >> dy;
    
    dimFile >> dummy;
    while (dummy.find("-dz")==string::npos)
        dimFile >> dummy;
    
    dimFile >> dz;
    
    
    cout << "(nx,dx) = ( " << nx << " ; " << dx << " ) "<< endl;
    cout << "(ny,dy) = ( " << ny << " ; " << dy << " ) "<< endl;
    cout << "(nz,dz) = ( " << nz << " ; " << dz << " ) "<< endl;
    
    unsigned int size = nx*ny*nz;
    unsigned int sizeIn = size;
    
    if( type.find("S16")!=string::npos )
        sizeIn = size*2;
    if( type.find("FLOAT")!=string::npos )
        sizeIn = size*4;
    
    unsigned char * tempData = new unsigned char[sizeIn];
    
    imaFile.read((char*)tempData, sizeIn);
    
    data.clear();
    data.resize(size);
    
    if( type.find("S16")!=string::npos ){
        for(unsigned int i = 0, j=0 ; i < size ; i ++, j+=2){
            unsigned char value = (unsigned char)tempData[j];
            data[i] = value;
            if ( std::find(labels.begin(), labels.end(), value) == labels.end() )
                labels.push_back(value);
        }
    } else if( type.find("FLOAT")!=string::npos ){
        float * floatArray = (float*) tempData;
        
        for(unsigned int i = 0 ; i < size ; i ++){
            unsigned char value = (unsigned char)floatArray[i];
            data[i] = value;
            if ( std::find(labels.begin(), labels.end(), value) == labels.end() )
                labels.push_back(value);
        }
        delete [] floatArray;
    } else {
        for(unsigned int i = 0 ; i < size ; i ++){
            unsigned char value = (unsigned char)tempData[i];
            data[i] = value;
            if ( std::find(labels.begin(), labels.end(), value) == labels.end() )
                labels.push_back(value);
        }
    }
    
    delete [] tempData;
}

void TextureViewer::setXCut(float _x){
    texture->setXCut(_x*texture->getWidth());
    cut[0] =_x*texture->dx()*texture->getWidth();
    update();
}

void TextureViewer::setYCut(float _y){
    texture->setYCut(_y*texture->getHeight());
    cut[1] =_y*texture->dy()*texture->getHeight();
    update();
}

void TextureViewer::setZCut(float _z){
    texture->setZCut(_z*texture->getDepth());
    cut[2] =_z*texture->dz()*texture->getDepth();
    update();
}

void TextureViewer::invertXCut(){
    texture->invertXCut();
    cutDirection[0] *= -1;
    update();
}

void TextureViewer::invertYCut(){
    texture->invertYCut();
    cutDirection[1] *= -1;
    update();
}

void TextureViewer::invertZCut(){
    texture->invertZCut();
    cutDirection[2] *= -1;
    update();
}

void TextureViewer::setXCutDisplay(bool _xCutDisplay){
    texture->setXCutDisplay(_xCutDisplay);
    update();
}

void TextureViewer::setYCutDisplay(bool _yCutDisplay){
    texture->setYCutDisplay(_yCutDisplay);
    update();
}

void TextureViewer::setZCutDisplay(bool _zCutDisplay){
    texture->setZCutDisplay(_zCutDisplay);
    update();
}

void TextureViewer::recompileShaders() {
    texture->recompileShaders();
    update();
}

void TextureViewer::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_R :update(); break;
    default : QGLViewer::keyPressEvent(e);
}
}

QString TextureViewer::helpString() const
{
    QString text("<h2>S i m p l e V i e w e r</h2>");
    text += "Use the mouse to move the camera around the object. ";
    text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
    text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
    text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
    text += "Simply press the function key again to restore it. Several keyFrames define a ";
    text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
    text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
    text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
    text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
    text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
    text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
    text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
    text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
    text += "Press <b>Escape</b> to exit the TextureViewer.";
    return text;
}
