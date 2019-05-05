#include "GLWindow.h"
#include <iostream>
#include <ngl/Vec3.h>
#include <ngl/Light.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Material.h>
#include <ngl/ShaderLib.h>
#include <QColorDialog>
#include<QFile>
#include<QGuiApplication>
#include<string>
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT = 0.05;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM = .5;
//----------------------------------------------------------------------------------------------------------------------
GLWindow::GLWindow(const QGLFormat _format, QWidget *_parent) : QGLWidget(_format, _parent)
{

  // set this widget to have the initial keyboard focus
  setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  m_rotate = false;
  // mouse rotation values set to 0
  m_spinXFace = 0;
  m_spinYFace = 0;
  m_wireframe = false;
  m_selectedObject = "";
  m_spinXFace = 0;
  m_spinYFace = 0;
  m_debugDisplay = true;

  m_animate = false;
  m_frameTime = 0.0;


  m_deformMesh = new SkinDeformer();
  m_sceneData = new SceneLoader();

}
GLWindow::~GLWindow()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
  Init->NGLQuit();
  delete m_deformMesh;
  delete m_sceneData;
}
// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::initializeGL()
{
  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);            // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  /// create our camera
  ngl::Vec3 eye(1, 1, 1);
  ngl::Vec3 look(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);

  m_camera = new ngl::Camera(eye, look, up);
  m_camera->setShape(45, float(1024 / 720), 0.1, 300);

//create the shader
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  std::string shaderName = "Diffuse";
  shader->createShaderProgram(shaderName);
  shader->attachShader(shaderName + "Vertex", ngl::VERTEX);
  shader->attachShader(shaderName + "Fragment", ngl::FRAGMENT);
  shader->loadShaderSource(shaderName + "Vertex", "shaders/" + shaderName + "Vertex.glsl");
  shader->loadShaderSource(shaderName + "Fragment", "shaders/" + shaderName + "Fragment.glsl");
  shader->compileShader(shaderName + "Vertex");
  shader->compileShader(shaderName + "Fragment");
  shader->attachShaderToProgram(shaderName, shaderName + "Vertex");
  shader->attachShaderToProgram(shaderName, shaderName + "Fragment");

  shader->bindAttribute(shaderName, 0, "inVert");
  shader->bindAttribute(shaderName, 1, "inUV");
  shader->bindAttribute(shaderName, 2, "inNormal");
  shader->linkProgramObject("Diffuse");
  shader->use("Diffuse");
  shader->setShaderParam4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
  shader->setShaderParam3f("camPos", m_camera->getEye().m_x,
                           m_camera->getEye().m_y,
                           m_camera->getEye().m_z);

  shaderName = "Surface";
  shader->createShaderProgram(shaderName);
  shader->attachShader(shaderName + "Vertex", ngl::VERTEX);
  shader->attachShader(shaderName + "Fragment", ngl::FRAGMENT);
  shader->loadShaderSource(shaderName + "Vertex", "shaders/" + shaderName + "Vertex.glsl");
  shader->loadShaderSource(shaderName + "Fragment", "shaders/" + shaderName + "Fragment.glsl");
  shader->compileShader(shaderName + "Vertex");
  shader->compileShader(shaderName + "Fragment");
  shader->attachShaderToProgram(shaderName, shaderName + "Vertex");
  shader->attachShaderToProgram(shaderName, shaderName + "Fragment");
  shader->bindAttribute(shaderName, 0, "inVert");
  shader->linkProgramObject("Surface");
  shader->use("Surface");
  shader->setShaderParam3f("color", 1.0f, 0.0f, 1.0f);

  shaderName = "Texture";
  shader->createShaderProgram(shaderName);
  shader->attachShader(shaderName + "Vertex", ngl::VERTEX);
  shader->attachShader(shaderName + "Fragment", ngl::FRAGMENT);
  shader->loadShaderSource(shaderName + "Vertex", "shaders/" + shaderName + "Vertex.glsl");
  shader->loadShaderSource(shaderName + "Fragment", "shaders/" + shaderName + "Fragment.glsl");
  shader->compileShader(shaderName + "Vertex");
  shader->compileShader(shaderName + "Fragment");
  shader->attachShaderToProgram(shaderName, shaderName + "Vertex");
  shader->attachShaderToProgram(shaderName, shaderName + "Fragment");

  shader->bindAttribute(shaderName, 0, "inVert");
  shader->bindAttribute(shaderName, 1, "inUV");
  shader->bindAttribute(shaderName, 2, "inNormal");
  shader->linkProgramObject("Texture");
  shader->use("Texture");
  shader->setShaderParam4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
  shader->setShaderParam3f("camPos", m_camera->getEye().m_x,
                           m_camera->getEye().m_y,
                           m_camera->getEye().m_z);
  // load and set a texture


  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  prim->createCylinder("cylinder", 1, 2, 4, 3);
  prim->createLineGrid("grid", 24, 24, 8);

  m_text = new ngl::Text(QFont("Arial", 14));
  m_text->setScreenSize(width(), height());

  startTimer(20);
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void GLWindow::resizeGL(int _w, int _h)
{
  glViewport(0, 0, _w, _h);
  m_camera->setShape(45, float(_w / _h), 0.1, 300);
}


void GLWindow::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat4 M;
  M = m_transformStack.getCurrentTransform().getMatrix() * m_mouseGlobalTX;
  MV = M * m_camera->getViewMatrix();
  MVP = MV * m_camera->getProjectionMatrix() ;

  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->use("Diffuse");
  shader->setShaderParamFromMat4("MVP", MVP);
  shader->setShaderParamFromMat4("M", M);
  shader->use("Texture");
  shader->setShaderParamFromMat4("MVP", MVP);
  shader->setShaderParamFromMat4("M", M);
  shader->use("Surface");
  shader->setShaderParamFromMat4("MVP", MVP);

}

void GLWindow::loadObj(std::string _p, std::string _o)
{
  std::string meshPath = (_p + "/" + _o);
  unsigned found = _p.find_last_of("/");
  std::string rootPath = _p.substr(0, found);
  found = _o.find_last_of(".");
  std::string objName = _o.substr(0, found);
  std::string texturePath = (rootPath + "/textures/" + objName + ".jpg");
  if (!QFile::exists(QString::fromStdString(texturePath))) {
    texturePath = "textures/UV_Checker.jpg";
    ngl::Texture texture(texturePath);
    texture.setTextureGL();
  }
  if (m_selectedObject != "") {
    delete m_deformMesh;
    delete m_sceneData;
    m_boneTransfroms.clear();
    m_deformMesh = new SkinDeformer();
    m_sceneData = new SceneLoader();
  }
  // first we create a mesh from an obj passing in the obj file and texture
  m_sceneData->load(meshPath);
  m_deformMesh->setMeshData(m_sceneData);
  m_selectedObject = meshPath;
}
//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void GLWindow::paintGL()
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  if (m_wireframe == true) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX = rotY * rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  loadMatricesToShader();
  shader->use("Surface");
  shader->setShaderParam3f("color", 0.2f, 0.2f, 0.2f);
  prim->draw("grid");

  if (m_selectedObject != "") {

    if (m_animate) {
      QTime t = QTime::currentTime();
      float time = float(t.msec() / 1000.0) * m_sceneData->getDuration() / m_sceneData->getTicksPerSec();
      m_sceneData->boneTransform(time, m_boneTransfroms);
      m_deformMesh->update();
      m_frameTime = time;
    }

    //draw Textured mesh
    loadMatricesToShader();
    shader->use("Diffuse");
    shader->setShaderParam3f("color", 0.5f, 0.5f, 1.0f);
    m_deformMesh->drawDeformMesh();

  }

  if (m_debugDisplay == true) {
    QString text;
    m_text->setColour(1, 1, 1);
    text.sprintf("FPS ::");
    m_text->renderText(10, 50, text);
  }

}

void GLWindow::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key()) {
    // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
    // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
    // show full screen
  case Qt::Key_F :

      break;
  case Qt::Key_N : break;
  case Qt::Key_B :  break;
  case Qt::Key_P : break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  updateGL();
}
//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseMoveEvent(QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if (m_rotate && _event->buttons() == Qt::LeftButton) {
    int diffx = _event->x() - m_origX;
    int diffy = _event->y() - m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    updateGL();
  }
  // right mouse translate code
  else if (m_translate && _event->buttons() == Qt::RightButton) {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    updateGL();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mousePressEvent(QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if (_event->button() == Qt::LeftButton) {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate = true;
  }
  // right mouse translate mode
  else if (_event->button() == Qt::RightButton) {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseReleaseEvent(QMouseEvent * _event)
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton) {
    m_rotate = false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton) {
    m_translate = false;
  }

}

void GLWindow::wheelEvent(QWheelEvent *_event)
{
  // check the diff of the wheel position (0 means no change)
  if (_event->delta() > 0) {
    m_modelPos.m_z += ZOOM;
  } else if (_event->delta() < 0) {
    m_modelPos.m_z -= ZOOM;
  }
  updateGL();
}

void GLWindow::toggleWireframe(bool _mode)
{
  m_wireframe = _mode;
  updateGL();
}


void GLWindow::setColour()
{
  QColor colour = QColorDialog::getColor();
  if (colour.isValid()) {
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->use("diffuse");
    shader->setShaderParam4f("Color", colour.redF(), colour.greenF(), colour.blueF(), 1.0);
    updateGL();
  }
}

void GLWindow::timerEvent(QTimerEvent *_event)
{
  updateGL();
}

void GLWindow::toggleMainTimer(bool _t)
{
  if (m_animate) {
    killTimer(m_timerID);
    m_animate = false;
  } else {
    m_timerID = startTimer(20);
    m_animate = true;
  }

}

void GLWindow::incrementFrame()
{
  m_frameTime += (m_sceneData->getDuration() / m_sceneData->getTicksPerSec()) / 100;
  std::cout << "Frame Time=" << m_frameTime << std::endl;
  m_sceneData->boneTransform(m_frameTime, m_boneTransfroms);
  m_deformMesh->update();
}

void GLWindow::decrementFrame()
{
  m_frameTime -= (m_sceneData->getDuration() / m_sceneData->getTicksPerSec()) / 100;
  if (m_frameTime < 0) {
    m_frameTime = 0;
  }
  m_sceneData->boneTransform(m_frameTime, m_boneTransfroms);
  m_deformMesh->update();
}
