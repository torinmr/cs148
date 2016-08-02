#include <iostream>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "portal.h"
#include "shader_loader.h"

using namespace glm;
using namespace std;

void Portal::Print() {
  cout << "Center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")" << endl;
  cout << "Normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << endl;
  cout << "Up: (" << up[0] << ", " << up[1] << ", " << up[2] << ")" << endl;
  cout << "Type: " << type << endl;
  cout << "Size: " << size << endl;
  cout << "VAO: " << VAO << endl;
  cout << "VBO: " << VBO << endl;
  cout << "EBO: " << EBO << endl;
}

void Portal::Draw(GLuint shader, mat4 view, mat4 projection) {
  glUseProgram(shader);

  mat4 model;
  model = translate(model, center);

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

void Portal::Setup(void) {
  GLfloat vertices[] = {
    -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };
  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
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

  cout << "Created portal" << endl;
}
