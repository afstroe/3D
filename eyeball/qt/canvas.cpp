#ifdef _WIN32
#include <windows.h>
#pragma comment (lib, "Opengl32.lib")
#pragma comment (lib, "SDL2.lib")
#endif

#include <qopenglcontext.h>

#include <iostream>
#include <sstream>

#include <gl/GL.h>

#include "canvas.h"
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/utils/defines.h>


Canvas::Canvas(QWidget* parent)
{
  setParent(parent);
}

class Geometry
{
public:
  template <typename Type>
  struct Point2 {
    Type x;
    Type y;
    Point2()
    {
      x = 0;
      y = 0;
    }
  };

  struct Mode 
  {
    GLuint drawMode = GL_TRIANGLES;
    GLuint fillMode = GL_FILL;
    GLuint faceMode = GL_FRONT;
  };

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<Point2<float>>, vertices);
   
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<int>, indices);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(size_t, numTriangles);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Mode, mode);

  GLuint vertexBuffer = 0;
  GLuint indexBuffer = 0;


  Geometry() :
    _numTriangles(0)  
  {
    ;
  }

  inline void bind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  }

  void createBuffers()
  {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Point2<float>), _vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(int), _indices.data(), GL_STATIC_DRAW);    
  }

  void draw()
  {
    bind();
    glPolygonMode(_mode.faceMode, _mode.fillMode);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
  }

  void drawImmediate()
  {
    glPolygonMode(_mode.faceMode, _mode.fillMode);
    glBegin(_mode.drawMode);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (size_t i = 0; i < _indices.size(); ++i)
    {
      glVertex2f(_vertices[_indices[i]].x, _vertices[_indices[i]].y);
    }
    glEnd();
  }
};

Geometry myBall;

void Canvas::initializeGL()
{  
  double radius = 0.2;
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

  // below thread will send a repaint signal for the canvas at 16 milliseconds
  // until the widget is closed.
  paintTrigger = std::thread([&](std::future<void> futureObj) {
    while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
      update();
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    }, std::move(exitSignal.get_future()));
  paintTrigger.detach();
}

void Canvas::paintGL()
{ 
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  static double y = -1;
  y > 1 ? y = -1 : y;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5, 0.5, -0.5, 0.5, 0.0f, 1.0f);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  

  
  
  glDisable(GL_TEXTURE_2D);
  //glBegin(GL_TRIANGLE_FAN);
  //glColor3f(1.0f, 0.0f, 0.0f);
  //for (size_t i = 0; i < myBall.indices.size(); ++i)
  //{
  //  glVertex2f(myBall.vertices[myBall.indices[i]].x, myBall.vertices[myBall.indices[i]].y);
  //}
  //glEnd();

  myBall.draw();


  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(-0.5, 0.0);
  glVertex2f(0.5, 0.0);
  glVertex2f(0, 0.5);
  glVertex2f(0, -0.5);
  glEnd();


  y += 0.01;
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

