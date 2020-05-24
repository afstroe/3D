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

Canvas::Canvas(QWidget* parent)
{
  setParent(parent);
}


Geometry myBall;
Camera camera;


void Canvas::initializeGL()
{  
  double radius = 5.0E-2;
  const size_t numSegments = 32;
  const double pi = 3.1415926535897932384626433832795;
  const double pi2 = 2 * pi;

  const double invNumSegments = 1.0 / numSegments;

  myBall.vertices().resize(3 * numSegments);
  myBall.indices().resize(numSegments + 2);
  myBall.numTriangles() = numSegments + 2;

  FOR(i, numSegments)
  {
    const auto arg = -i * pi2 * invNumSegments;
    myBall.vertices()[i].x = radius * std::sin(arg);
    myBall.vertices()[i].y = radius * std::cos(arg);
  }  
  myBall.vertices()[numSegments] = myBall.vertices()[0];
  myBall.vertices()[numSegments + 1].x = 0.0f;
  myBall.vertices()[numSegments + 1].y = 0.0f;

  myBall.indices()[0] = static_cast<int>(numSegments + 1);
  for(size_t i = 0; i < numSegments; ++i)
  {    
    myBall.indices()[i + 1] = static_cast<int>(i);
  }
  myBall.indices()[numSegments + 1] = 0;

  myBall.mode() = { GL_TRIANGLE_FAN, GL_FILL, GL_FRONT };

  myBall.createBuffers();

  myBall.materialFromFiles("shaders/test.vert", "shaders/test.frag");
  myBall.material().set("a", glUniform1i, 128);

  camera.mode() = Camera::Mode::PERSPECTIVE;
  camera.position() = glm::vec3(0, 0, -2);  

  // below thread will send a repaint signal for the canvas at 16 milliseconds
  // until the widget is closed.
  paintTrigger = std::thread([&](std::future<void> futureObj) {
    while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
      update();
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    }, std::move(exitSignal.get_future()));
  paintTrigger.detach();

  grabKeyboard();
}

void Canvas::paintGL()
{ 
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  static double y = -1;
  y > 1 ? y = -1 : y;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //auto viewMatrix = camera.transform();

  auto viewMatrix = camera.projectionMatrix() * camera.lookAt({ 0, 0, 0 });

  auto a = glm::value_ptr(viewMatrix);  

  myBall.material().set("mvpMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(viewMatrix));  

  //glOrtho(-0.5, 0.5, -0.5, 0.5, 0.0f, 1.0f);
  
  glDisable(GL_TEXTURE_2D);

  myBall.draw();


  Shader::detach();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(glm::value_ptr(viewMatrix));

  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-0.5, 0.0, 0.0);
  glVertex3f(0.5, 0.0, 0.0);
  glVertex3f(0, 0.5, 0.0);
  glVertex3f(0, -0.5, 0.0);
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

  // set the signal to exit the clock thread
  exitSignal.set_value();
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
      camera.position().z += 0.1f;
    break;   
    case Qt::Key_Down:
      camera.position().z -= 0.1f;
    break;
    case Qt::Key_Left:
      if (event->modifiers() & Qt::ShiftModifier)
      {
        camera.position().x += 0.1f;
      }
      else 
      {
        camera.heading() -= 1.0f;
      }      
    break;   
    case Qt::Key_Right:
      if (event->modifiers() & Qt::ShiftModifier)
      {
        camera.position().x -= 0.1f;
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
  }
  int a = 0;
}

