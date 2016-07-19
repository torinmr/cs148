#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
 public:
  Camera(void)
    : cameraPos(glm::vec3(0.0f, 0.0f, 3.0f))
    , cameraFront(glm::vec3(0.0f, 0.0f, -1.0f))
    , cameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , lastX(400)
    , lastY(300)
    , pitch(0.0f)
    , yaw(-90.0f)
    , firstMouse(true)
    {}
  void processKeyboardInput(bool keys[], GLfloat deltaTime);
  void processMousePosition(GLfloat mouseX, GLfloat mouseY);
  glm::mat4 getView(void);

 private:
  glm::vec3 cameraPos;
  glm::vec3 cameraFront;
  glm::vec3 cameraUp;
  GLfloat lastX, lastY;
  GLfloat pitch, yaw;
  bool firstMouse;
};

#endif /* CAMERA_H */
