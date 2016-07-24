#ifndef PORTAL_H
#define PORTAL_H

#include <glm/glm.hpp>
#include <GL/glew.h>

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

class Portal {
 public:
  // Create portal at a particular location.
   Portal(glm::vec3 center, glm::vec3 normal, glm::vec3 up, bool type)
    : center(center)
    , normal(normal)
    , up(up)
    , type(type)
    , size(1.0f)
    {}

  // Create a portal on the nearest surface found by ray casting.
  Portal(const Ray &ray, glm::vec3 up, bool type);

  void Draw(GLuint shader);

 private:
  glm::vec3 center;
  glm::vec3 normal;
  glm::vec3 up;
  bool type;
  GLfloat size;
};

#endif /* PORTAL_H */
