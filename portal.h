#ifndef PORTAL_H
#define PORTAL_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
using namespace std;

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

class Portal {
 public:
  // Create portal at a particular location.
  Portal(glm::vec3 center, glm::vec3 normal, glm::vec3 up, GLint type, Portal *linked);

  // Create a portal in the direction of the given ray.
  Portal(const Ray &ray, glm::vec3 up, GLint type, Portal *linked);

  void DrawStencil(GLuint shader, glm::mat4 view, glm::mat4 projection, GLuint bit);
  void Draw(GLuint shader, glm::mat4 view, glm::mat4 projection);
  glm::mat4 GetView(glm::vec3 incomingView);
  bool IsLinked(void) { return linked != nullptr; }
  void Print(void);

 private:
  void DrawCommon(GLuint shader, glm::mat4 view, glm::mat4 projection, glm::mat4 preModel);
  void Setup(void);

  glm::vec3 center;
  glm::vec3 normal;
  glm::vec3 up;
  GLint type;
  Portal *linked;

  GLuint VAO, VBO, EBO;
};

#endif /* PORTAL_H */
