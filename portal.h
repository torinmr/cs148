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
  Portal(glm::vec3 center, glm::vec3 normal, glm::vec3 up, GLint type, Portal *linked)
    : center(center)
    , normal(normal)
    , up(up)
    , type(type)
    , linked(linked)
    {
      Setup();
    }

  // Create a portal in the direction of the given ray.
   Portal(const Ray &ray, glm::vec3 up, GLint type, Portal *linked)
    : center(ray.origin + 3.0f*glm::normalize(ray.direction))
    , normal(-glm::normalize(ray.direction))
    , up(up)
    , type(type)
    , linked(linked)
    {
      Setup();
    }

  void Draw(GLuint shader, glm::mat4 view, glm::mat4 projection);
  void Print(void);

 private:
  void Setup(void);

  glm::vec3 center;
  glm::vec3 normal;
  glm::vec3 up;
  GLint type;
  Portal *linked;

  GLfloat size;
  GLuint VAO, VBO, EBO;
};

#endif /* PORTAL_H */
