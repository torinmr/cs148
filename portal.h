#ifndef PORTAL_H
#define PORTAL_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>

#include "camera.h"

using namespace std;

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct ViewInfo {
  glm::vec3 position;
  glm::vec3 direction;
  glm::mat4 transform;
};

class Portal {
 public:
  // Create portal at a particular location.
  Portal(glm::vec3 center, glm::vec3 normal, glm::vec3 up,
         GLint type, Portal *linked);

  // Create a portal in the direction of the given ray.
  Portal(const Ray &ray, glm::vec3 up, GLint type, Portal *linked);

  void DrawStencil(GLuint shader, glm::mat4 view, glm::mat4 projection, GLuint depth);
  void Draw(GLuint shader, glm::mat4 view, glm::mat4 projection, GLfloat time);
  ViewInfo GetView(ViewInfo view);
  bool IsLinked(void) { return linked != nullptr; }
  void Print(void);

  glm::vec3 center;
  glm::vec3 normal;
  glm::vec3 up;

 private:
  void DrawCommon(GLuint shader, glm::mat4 view, glm::mat4 projection,
                  glm::mat4 preModel);
  glm::vec3 ReflectThroughPortal(glm::vec3 incomingVector, bool verbose);
  void Setup(void);

  GLint type;
  Portal *linked;

  GLuint VAO, VBO, EBO;
};

#endif /* PORTAL_H */
