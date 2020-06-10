#ifdef _WIN32
#include <windows.h>
#pragma comment (lib, "Opengl32.lib")
#pragma comment (lib, "SDL2.lib")
#endif

#include <qopenglcontext.h>
#include <qevent.h>

#include <iostream>
#include <sstream>

#include "canvas.h"
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/utils/defines.h>
#include <eyeball/graphics/opengl/geometry.h>
#include <eyeball/graphics/camera.h>
#include <eyeball/graphics/fbx/fbxbuilder.h>
#include <eyeball/utils/palette.h>
#include <eyeball/graphics/opengl/typewriter.h>
#include <eyeball/app/appglobalstate.h>

Canvas::Canvas(QWidget* parent)
{
  setParent(parent);
}


PositionedGeometry myBall, del;
std::vector<PositionedGeometry*> fbxGeometry;
Camera camera;


enum class ShaderKeys 
{
  None = 0,
  Test,
  Material,

  Num
};
Palette<Shader> shaderPalette;


void Canvas::initializeGL()
{  
  double radius = 5.0E-2;
  const size_t numSegments = 32;
  const double pi = 3.1415926535897932384626433832795;
  const double pi2 = 2 * pi;

  const double invNumSegments = 1.0 / numSegments;

  myBall.vertices().resize(numSegments + 2);
  myBall.indices().resize(numSegments + 2);
  myBall.numTriangles() = numSegments + 2;

  FOR(i, numSegments)
  {
    const auto arg = -i * pi2 * invNumSegments;
    myBall.vertices()[i].x = radius * std::sin(arg);
    myBall.vertices()[i].y = radius * std::cos(arg);
    myBall.vertices()[i].z = 0;
  }  
  myBall.vertices()[numSegments] = myBall.vertices()[0];
  myBall.vertices()[numSegments + 1].x = 0.0f;
  myBall.vertices()[numSegments + 1].y = 0.0f;
  myBall.vertices()[numSegments + 1].z = 0.0f;

  myBall.indices()[0] = static_cast<int>(numSegments + 1);
  for(size_t i = 0; i < numSegments; ++i)
  {    
    myBall.indices()[i + 1] = static_cast<int>(i);
  }
  myBall.indices()[numSegments + 1] = 0;

  myBall.mode() = { GL_TRIANGLE_FAN, GL_FILL, GL_FRONT };

  myBall.createBuffers();

  shaderPalette.add(static_cast<int>(ShaderKeys::Test), Shader::fromFiles("shaders/compiled/test.vert", "shaders/compiled/test.frag"));

  myBall.material() = std::make_shared<Material>();
  myBall.material()->shader() = shaderPalette.get(static_cast<int>(ShaderKeys::Test));
  myBall.material()->shader()->set("a", glUniform1i, 128);



  camera.mode() = Camera::Mode::PERSPECTIVE;
  camera.position() = glm::vec3(0, 1, 200);  


  FbxBuilder b;
  
  //b.load("res/models/free/freida-mod.FBX", fbxGeometry, true);  
  b.load("res/models/chibi-avatar/Chibi-Avatar.FBX", fbxGeometry);

  shaderPalette.add(static_cast<int>(ShaderKeys::Material), Shader::fromFiles("shaders/compiled/material.vert", "shaders/compiled/material.frag"));

  for (auto* fbxg : fbxGeometry)
  {
    fbxg->material()->shader() = shaderPalette.get(static_cast<int>(ShaderKeys::Material));
  }

  // below thread will send a repaint signal for the canvas at 16 milliseconds
  // until the widget is closed.
  paintTrigger = std::thread([&](std::future<void> futureObj) {
    while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
      update();
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    }, std::move(exitSignal.get_future()));
  paintTrigger.detach();

  auto hWnd = reinterpret_cast<HWND>(effectiveWinId());
  m_hDC = ::GetDC(hWnd);

  AppGlobalState::get().typeWriterFont().create(m_hDC, "Courier New", 16, FW_NORMAL, 0, 0, 0);

  grabKeyboard();
}

void Canvas::paintGL()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  static double y = -1;
  y > 1 ? y = -1 : y;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  AppGlobalState::get().typeWriter().printf(&AppGlobalState::get().typeWriterFont(), Typewriter::Alignment::Whatever, Typewriter::SpecialEffects::None, 0, 0, 0, 0.1f, 0.1f, "Test");

  auto viewMatrix = camera.transform();

  // auto viewMatrix = camera.projectionMatrix() * camera.lookAt({ 0, 0, 0 });

  myBall.material()->shader()->set("projectionMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
  auto modelViewMatrix = camera.mvMatrix() * myBall.transform();
  myBall.material()->shader()->set("modelViewMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

  
  
  glDisable(GL_TEXTURE_2D);

  myBall.draw(camera);  

  for (auto* geometry : fbxGeometry)
  {
    geometry->material()->shader()->set("projectionMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
    
    //!! TODO: Remove the hard-coded transform
    // geometry->transform() = glm::mat4(1);// glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    auto transform = geometry->transform() * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    
    
    modelViewMatrix = camera.mvMatrix() * transform;
    geometry->material()->shader()->set("modelViewMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

    glColor3f(1.0f, 1.0f, 0.0f);
    geometry->draw(camera);
  }

  Shader::detach();

  

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf(glm::value_ptr(camera.projectionMatrix()));

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(glm::value_ptr(camera.mvMatrix()));  



  glBegin(GL_LINES);  
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0, 0.0);
  glVertex3f(1.0f, 0.0, 0.0);

  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.0, 0.0);
  glVertex3f(0.0f, 1.0, 0.0);

  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.0, 0.0);
  glVertex3f(0.0f, 0.0, 1.0);
  glEnd();

  OPENGL_CHECK_ERROR();
}

void Canvas::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}

void Canvas::closeEvent(QCloseEvent* event)
{
  UNREFERENCED_PARAMETER(event);

  ReleaseDC(reinterpret_cast<HWND>(effectiveWinId()), m_hDC);

  // set the signal to exit the clock thread
  exitSignal.set_value();
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
      camera.position().z -= 0.1f * cos(glm::radians(-camera.heading()));
      camera.position().x -= 0.1f * sin(glm::radians(-camera.heading()));
    break;   
    case Qt::Key_Down:
      camera.position().z += 0.1f * cos(glm::radians(-camera.heading()));
      camera.position().x += 0.1f * sin(glm::radians(-camera.heading()));
      break;
    case Qt::Key_Left:
      if (event->modifiers() & Qt::ShiftModifier)
      {
        camera.position().x -= 0.1f;
      }
      else 
      {
        camera.heading() -= 1.0f;
      }      
    break;   
    case Qt::Key_Right:
      if (event->modifiers() & Qt::ShiftModifier)
      {
        camera.position().x += 0.1f;
      }
      else
      {
        camera.heading() += 1.0f;
      }
    break;
    case Qt::Key_8:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.pitch() += 1.0f;
      }
    break;   
    case Qt::Key_2:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.pitch() -= 1.0f;
      }
    break;
    case Qt::Key_6:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.roll() += 1.0f;
      }
    break;   
    case Qt::Key_4:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.roll() -= 1.0f;
      }
    break;
    case Qt::Key_9:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.position().y += 0.1f;
      }
    break;   
    case Qt::Key_3:
      if (event->modifiers() & Qt::KeypadModifier)
      {
        camera.position().y -= 0.1f;
      }
    break;
  }
  int a = 0;
}

