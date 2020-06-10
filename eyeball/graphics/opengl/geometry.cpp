#include "geometry.h"

#include <eyeball/app/appglobalstate.h>

void Geometry::createBuffers()
{ 
  // buffers
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);  
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Point3<float>), m_vertices.data(), GL_STATIC_DRAW);

  if (m_normals.size())
  {
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(Point3<float>), m_normals.data(), GL_STATIC_DRAW);
  }

  if (m_textureCoordinates.size())
  {
    glGenBuffers(1, &textureCoordinatesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_textureCoordinates.size() * sizeof(Point2<float>), m_textureCoordinates.data(), GL_STATIC_DRAW);
  }

  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if(m_material)
  {
    m_material->loadTextures();
  }  

}

void Geometry::preDraw()
{
  m_material->preDraw();

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

  if (textureCoordinatesBuffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  }  
}

void Geometry::draw(const Camera& camera)
{
  UNREFERENCED_PARAMETER(camera);
  auto debugState = AppGlobalState::get().debugMode();

  bind();

  preDraw();

  if (debugState & AppGlobalState::DM_RENDER_MONOCHROME)
  {
    m_material->shader()->set("drawMonochrome", glUniform1i, 1);
  }
  else
  {
    m_material->shader()->set("drawMonochrome", glUniform1i, 0);
  }

  if (debugState & AppGlobalState::DM_SOLID_WIRE)
  {
    glEnable(GL_DEPTH_TEST);
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT_AND_BACK : GL_FRONT;
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
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT_AND_BACK : GL_FRONT;
    glPolygonMode(mode, GL_LINE);
    m_material->shader()->set("wireframe", glUniform1i, 1);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
  }    
  else /*AppGlobalState::DM_DISABLED*/    
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    auto mode = debugState & AppGlobalState::DM_RENDER_BOTH_FACES ? GL_FRONT_AND_BACK : GL_FRONT;
    glPolygonMode(mode, GL_FILL);
    m_material->shader()->set("wireframe", glUniform1i, 0);
    glDrawElements(m_mode.drawMode, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
  }



  postDraw();
}

void Geometry::postDraw()
{  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableClientState(GL_VERTEX_ARRAY);

  m_material->postDraw();
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

void PositionedGeometry::drawNormals(const Camera& camera)
{
  if (m_normals.size() == 0)
  {
    return;
  }

  m_material->shader()->detach();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf(glm::value_ptr(camera.projectionMatrix()));

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  auto mvMatrix = camera.mvMatrix() * transform();
  glMultMatrixf(glm::value_ptr(mvMatrix));

  glEnable(GL_DEPTH_TEST);

  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);

  for (auto i = 0; i < m_normals.size(); ++i)
  {
    auto& vertex = m_vertices[i];
    auto& normal = m_normals[i];

    glm::vec3 p0 = { vertex.x, vertex.y, vertex.z };
    const float t = 0.05f;

    glm::vec3 tn = { t * normal.x, t * normal.y, t * normal.z };    
    glm::vec3 p1 = p0 + tn;

    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
  }

  glEnd();
}

void PositionedGeometry::draw(const Camera& camera)
{
  m_material->shader()->set("modelMatrix", glUniformMatrix4fv, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(transform()));
  m_material->shader()->set("cameraPosition", glUniform3fv, 1, glm::value_ptr(camera.position()));

  if(!(AppGlobalState::get().debugMode() & AppGlobalState::DM_RENDER_BOTH_FACES))
  {
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
  }
  else
  {
    glDisable(GL_CULL_FACE);
  }

  Geometry::draw(camera);

  auto debugState = AppGlobalState::get().debugMode();

  if (debugState & AppGlobalState::DM_DRAW_NORMALS)
  {
    drawNormals(camera);
  }

}
