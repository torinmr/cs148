#include <iostream>

#include "portal.h"
#include "shader_loader.h"
using namespace glm;
using namespace std;

Portal::Portal(const Ray &ray, vec3 up, bool type) {
  Portal(ray.origin + normalize(ray.direction),
         -normalize(ray.direction),
         up, type);
}

void Portal::Draw(GLuint shader) {
  cout << "Draw!" << endl;
}
