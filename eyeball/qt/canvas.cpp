#ifdef _WIN32
#include <windows.h>
#pragma comment (lib, "Opengl32.lib")
#endif

#include <gl/GL.h>

#include "canvas.h"
#include <eyeball/graphics/opengl/opengl_ext.h>


Canvas::Canvas(QWidget* parent)
{
  setParent(parent);
}

void Canvas::initializeGL()
{
  int a = 0;
}

void Canvas::paintGL()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5, 0.5, -0.5, 0.5, 0.0f, 1.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(-0.5, -0.5);
  glVertex2f(0.5, 0.5);
  glEnd();
}

void Canvas::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}
