#include "geometry.h"

#include <eyeball/app/appglobalstate.h>

void Geometry::createBuffers()
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

void Geometry::draw()
{
  auto debugState = AppGlobalState::get().debugMode();

  bind();

  if (debugState & AppGlobalState::DM_SOLID_WIRE)
  {
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT : GL_FRONT_AND_BACK;
    glPolygonMode(mode, GL_FILL);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
    
    _material.detach();
    
    glPolygonMode(mode, GL_LINE);
    glColor3f(0.8f, 0.8f, 0.8f);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
  }
  else if (debugState & AppGlobalState::DM_WIREFRAME)
  {
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT : GL_FRONT_AND_BACK;
    glPolygonMode(mode, GL_LINE);    
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
  }    
  else /*AppGlobalState::DM_DISABLED*/    
  {
    glPolygonMode(_mode.faceMode, _mode.fillMode);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
  }
}

void Geometry::drawImmediate()
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

void Geometry::materialFromFiles(const char* vertexShader, const char* fragmentShader)
{
  _material = Shader::fromFiles(vertexShader, fragmentShader);
}

Geometry::~Geometry()
{
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
}
