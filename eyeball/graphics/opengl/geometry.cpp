#include "geometry.h"

#include <eyeball/app/appglobalstate.h>

void Geometry::createBuffers()
{ 
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);  
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Point3<float>), m_vertices.data(), GL_STATIC_DRAW);

  if (m_normals.size())
  {
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(Point3<float>), m_normals.data(), GL_STATIC_DRAW);
  }

  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geometry::preDraw()
{
  m_material->shader()->set("material.ambient", glUniform4fv, 1, glm::value_ptr(m_material->ambient()));
  m_material->shader()->set("material.diffuse", glUniform4fv, 1, glm::value_ptr(m_material->diffuse()));
  m_material->shader()->set("material.specular", glUniform4fv, 1, glm::value_ptr(m_material->specular()));
  m_material->shader()->set("material.emissive", glUniform4fv, 1, glm::value_ptr(m_material->emissive()));
  m_material->shader()->set("material.shininess", glUniform1f, m_material->shininess());

  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  if (normalBuffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  }
}

void Geometry::draw(const Camera& camera)
{  
  auto debugState = AppGlobalState::get().debugMode();

  bind();

  preDraw();  

  if (debugState & AppGlobalState::DM_SOLID_WIRE)
  {
    glEnable(GL_DEPTH_TEST);
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT : GL_FRONT_AND_BACK;
    glPolygonMode(mode, GL_FILL);
    m_material->shader()->set("wireframe", glUniform1i, 0);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-0.5f, 1.0f);
    m_material->shader()->set("wireframe", glUniform1i, 1);
    glPolygonMode(mode, GL_LINE);
    glColor3f(0.8f, 0.8f, 0.8f);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glPolygonOffset(0.0f, 1.0f);
    glDisable(GL_POLYGON_OFFSET_LINE);
  }
  else if (debugState & AppGlobalState::DM_WIREFRAME)
  {
    glDisable(GL_DEPTH_TEST);
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT : GL_FRONT_AND_BACK;
    glPolygonMode(mode, GL_LINE);    
    m_material->shader()->set("wireframe", glUniform1i, 1);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
  }    
  else /*AppGlobalState::DM_DISABLED*/    
  {
    glEnable(GL_DEPTH_TEST);
    m_material->shader()->set("wireframe", glUniform1i, 0);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
  }  

  postDraw();
}

void Geometry::postDraw()
{  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Geometry::drawImmediate()
{
  glPolygonMode(m_mode.faceMode, m_mode.fillMode);
  glBegin(m_mode.drawMode);
  glColor3f(1.0f, 0.0f, 0.0f);
  for (size_t i = 0; i < m_indices.size(); ++i)
  {
    glVertex3f(m_vertices[m_indices[i]].x, m_vertices[m_indices[i]].y, m_vertices[m_indices[i]].z);
  }
  glEnd();
}

Geometry::~Geometry()
{
  if (haveOpenGLContext())
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
  }
}

void PositionedGeometry::draw(const Camera& camera)
{
  m_material->shader()->set("modelMatrix", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(transform()));
  m_material->shader()->set("cameraPosition", glUniform3fv, 1, glm::value_ptr(camera.position()));

  Geometry::draw(camera);
}
