#include <iostream>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "portal.h"
#include "presentation.h"
#include "shader_loader.h"

using namespace glm;
using namespace std;

Portal::Portal(glm::vec3 center, glm::vec3 normal, glm::vec3 up,
               GLint type, Portal *linked)
  : center(center)
  , normal(normal)
  , up(up)
  , type(type)
  , linked(linked)
{
      Setup();
}

Portal::Portal(const Ray &ray, glm::vec3 up, GLint type, Portal *linked)
  : center(ray.origin + 3.0f*glm::normalize(ray.direction))
  , normal(-glm::normalize(ray.direction))
    , up(up)
  , type(type)
  , linked(linked)
{
  Setup();
}

void Portal::Print() {
  cout << "Center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")" << endl;
  cout << "Normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << endl;
  cout << "Up: (" << up[0] << ", " << up[1] << ", " << up[2] << ")" << endl;
  cout << "Type: " << type << endl;
  cout << "VAO: " << VAO << endl;
  cout << "VBO: " << VBO << endl;
  cout << "EBO: " << EBO << endl;
  cout << "Linked: " << linked << endl;
}

void Portal::DrawStencil(GLuint shader, mat4 view, mat4 projection, GLuint bit) {
  glStencilFunc(GL_ALWAYS, bit, 0xFF);
  glStencilMask(0xFF);

  mat4 preModel;
  // We bump it slightly off to avoid z-fighting.
  preModel = translate(preModel, vec3(0.0f, 0.0f, -0.0001f));
  preModel = scale(preModel, vec3(0.9f, 0.9f, 0.9f));

  DrawCommon(shader, preModel, view, projection);
}

void Portal::Draw(GLuint shader, mat4 view, mat4 projection) {
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  glUseProgram(shader);

  GLint typeLocation = glGetUniformLocation(shader, "type");
  glUniform1i(typeLocation, type);

  mat4 preModel;
  DrawCommon(shader, preModel, view, projection);
}

void Portal::DrawCommon(GLuint shader, mat4 preModel, mat4 view, mat4 projection) {
  glUseProgram(shader);

  mat4 model = lookAt(center, center + normal, up);
  model = affineInverse(model);
  model = model * preModel;


  GLuint modelLoc = glGetUniformLocation(shader, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

  GLuint viewLoc = glGetUniformLocation(shader, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

  GLuint projectionLoc = glGetUniformLocation(shader, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

mat4 Portal::GetView(glm::vec3 incomingDirection) {
  glm::vec3 outgoingDirection;
  if (getPresentationStage() >= 2) {

  } else {
    outgoingDirection = linked->normal;
  }

  return lookAt(linked->center, linked->center + outgoingDirection, linked->up);
}

void Portal::Setup() {
  if (linked != nullptr) {
    if (linked->linked != nullptr) {
      delete linked->linked;
    }
    linked->linked = this;
  }

  GLfloat vertices[] = {
    -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };
  GLuint indices[] = {
    3, 1, 0,
    3, 2, 1,
  };

  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);
  {
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),
                          (GLvoid*) 0);
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),
                          (GLvoid*)(3*sizeof(GLfloat)));
  }
  glBindVertexArray(0);
  Print();
}
