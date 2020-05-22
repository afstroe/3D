#include "geometry.h"

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

void Geometry::draw(bool wireSolid)
{
  bind();
  if (!wireSolid)
  {
    glPolygonMode(_mode.faceMode, _mode.fillMode);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
  }
  else
  {
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(_mode.drawMode, static_cast<GLsizei>(_numTriangles), GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT, GL_LINE);
    glColor3f(0.8f, 0.8f, 0.8f);
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
