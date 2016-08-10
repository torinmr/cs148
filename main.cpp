#include <bitset>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#define GLEW_STATIC
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "camera.h"
#include "model.h"
#include "portal.h"
#include "presentation.h"
#include "shader_loader.h"

using namespace std;

ShaderLoader *sl = nullptr;
ShaderLoader *portalSl = nullptr;
ShaderLoader *portalStencilSl = nullptr;
Camera *camera = nullptr;
bool keys[1024];
Portal *portals[2];
GLint lastPortal = -1;

Model *myModel;

const GLuint WIDTH = 1920, HEIGHT = 1080;

bool init_resources(void) {
  sl = new ShaderLoader("vertex.glsl",
                        "fragment.glsl");
  portalSl = new ShaderLoader("portal_vertex.glsl",
                              "portal_fragment.glsl");
  portalStencilSl = new ShaderLoader("portal_stencil_vertex.glsl",
                                     "portal_stencil_fragment.glsl");

  myModel = new Model("resources/sand/Sand_Final.obj");

  cout << "Loaded" << endl;

  return true;
}

void free_resources(void) {
  if (sl != nullptr) {
    delete sl;
  }
  if (portalSl != nullptr) {
    delete portalSl;
  }
  if (portalStencilSl != nullptr) {
    delete portalStencilSl;
  }
  if (camera != nullptr) {
    delete camera;
  }
  if (myModel != nullptr) {
    delete myModel;
  }
  for (int i = 0; i < 2; i++) {
    if (portals[i] != nullptr) {
      delete portals[i];
    }
  }
}

void renderScene(GLuint shader, glm::mat4 view,  glm::mat4 projection) {
  glUseProgram(shader);

  glm::mat4 model;

  GLuint modelLoc = glGetUniformLocation(sl->getProgram(), "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  GLuint viewLoc = glGetUniformLocation(sl->getProgram(), "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  GLuint projectionLoc = glGetUniformLocation(sl->getProgram(), "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Direction the light is shining in.
  glm::vec3 lightDir(0.2, -1, 0.2);
  lightDir = glm::normalize(lightDir);
  GLuint lightDirLoc = glGetUniformLocation(sl->getProgram(), "lightDir");
  glUniform3f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z);

  myModel->Draw(shader);
}

void recursiveRender(ViewInfo viewInfo, glm::mat4 projection,
                     GLuint depth, GLuint stencil) {
  if (depth > 4) {
    return;
  }

  GLuint oldStencilMask = (1<<(2*depth)) - 1;
  GLuint newStencilMask = (1<<(2*(depth+1))) - 1;
  GLuint stencilWriteMask = 3 << (depth*2);
  GLuint lastPortal = (stencil >> (2*(depth - 1)) & 3) - 1;
  GLuint newStencil[2] = {
    stencil | (1 << (depth*2)),
    stencil | (2 << (depth*2))
  };
  /*
  cout << endl;
  cout << "stencil: " << bitset<8>(stencil) << endl;
  cout << "oldStencilMask: " << bitset<8>(oldStencilMask) << endl;
  cout << "newStencilMask: " << bitset<8>(newStencilMask) << endl;
  cout << "stencilWriteMask: " << bitset<8>(stencilWriteMask) << endl;
  cout << "lastPortal: " << lastPortal << endl;
  cout << "newStencil0: " << bitset<8>(newStencil[0]) << endl;
  cout << "newStencil1: " << bitset<8>(newStencil[1]) << endl;
  */

  glStencilFunc(GL_EQUAL, stencil, oldStencilMask);
  glStencilMask(0x00);
  glClear(GL_DEPTH_BUFFER_BIT);
  renderScene(sl->getProgram(), viewInfo.transform, projection);

  for (GLuint i = 0; i < 2; i++) {
    if (depth > 0 && lastPortal != i) { continue; }
    if (portals[i] != nullptr) {
      portals[i]->Draw(portalSl->getProgram(), viewInfo.transform, projection, glfwGetTime());
    }
  }

  for (GLuint i = 0; i < 2; i++) {
    if (depth > 0 && lastPortal != i) { continue; }
    if (portals[i] != nullptr) {

      glStencilFunc(GL_EQUAL, newStencil[i], oldStencilMask);
      glStencilMask(stencilWriteMask);
      portals[i]->DrawStencil(portalStencilSl->getProgram(),
                              viewInfo.transform, projection, depth);
      glStencilMask(0x00);
    }
  }

  if (getPresentationStage() >= 1) {
    for (GLuint i = 0; i < 2; i++) {
      if (depth > 0 && lastPortal != i) { continue; }
      if (portals[i] != nullptr && portals[i]->IsLinked()) {
        ViewInfo newViewInfo = portals[i]->GetView(viewInfo);

        projection = projection;
        if (getPresentationStage() >= 3) {
          GLfloat distToPortal = distance(viewInfo.position,
                                          portals[i]->center);
          projection = glm::perspective(glm::radians(45.0f),
                                        (GLfloat) WIDTH / (GLfloat) HEIGHT,
                                        distToPortal+0.1f, 1000.0f);
        }

        if (getPresentationStage() >= 4) {
          recursiveRender(newViewInfo, projection, depth + 1, newStencil[i]);
        } else{
          glStencilFunc(GL_EQUAL, newStencil[i], newStencilMask);
          glStencilMask(0x00);
          glClear(GL_DEPTH_BUFFER_BIT);
          renderScene(sl->getProgram(), newViewInfo.transform, projection);
        }
      }
    }
  }
}

void render() {
  glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
  glStencilMask(0xFF);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  ViewInfo viewInfo;
  viewInfo.transform = camera->getView();
  viewInfo.position = camera->cameraPos;
  viewInfo.direction = camera->cameraFront;

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f),
                                (GLfloat) WIDTH / (GLfloat) HEIGHT,
                                0.1f, 1000.0f);

  recursiveRender(viewInfo, projection, 0, 0x00);
}

void keyCallback(GLFWwindow* window, int key, int scancode,
                 int action, int mode)
{
  if (action == GLFW_PRESS) {
    keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    keys[key] = false;
  }

  if (keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_P: {
      Ray ray;
      ray.origin = camera->cameraPos;
      ray.direction = camera->cameraFront;

      if (lastPortal == -1) {
        cout << "Creating portal, no existing portal to link." << endl;
        portals[0] = new Portal(ray, camera->cameraUp, 0, nullptr);
        lastPortal = 0;
      } else {
        cout << "Creating portal, linked to portal " << lastPortal << "." << endl;
        GLint nextPortal = (lastPortal + 1) % 2;
        portals[nextPortal] = new Portal(ray, camera->cameraUp, nextPortal, portals[lastPortal]);
        lastPortal = nextPortal;
      }
      break;
    }
    case GLFW_KEY_0: {
      setPresentationStage(0);
      cout << "Setting presentation stage to 0" << endl;
      break;
    }
    case GLFW_KEY_1: {
      setPresentationStage(1);
      cout << "Setting presentation stage to 1" << endl;
      break;
    }
    case GLFW_KEY_2: {
      setPresentationStage(2);
      cout << "Setting presentation stage to 2" << endl;
      break;
    }
    case GLFW_KEY_3: {
      setPresentationStage(3);
      cout << "Setting presentation stage to 3" << endl;
      break;
    }
    case GLFW_KEY_4: {
      setPresentationStage(4);
      cout << "Setting presentation stage to 4" << endl;
      break;
    }
    case GLFW_KEY_5: {
      setPresentationStage(5);
      cout << "Setting presentation stage to 5" << endl;
      break;
    }
    case GLFW_KEY_6: {
      setPresentationStage(6);
      cout << "Setting presentation stage to 6" << endl;
      break;
    }
    case GLFW_KEY_7: {
      setPresentationStage(7);
      cout << "Setting presentation stage to 7" << endl;
      break;
    }
    case GLFW_KEY_8: {
      setPresentationStage(8);
      cout << "Setting presentation stage to 8" << endl;
      break;
    }
    case GLFW_KEY_9: {
      setPresentationStage(9);
      cout << "Setting presentation stage to 9" << endl;
      break;
    }
    default:
      break;
    }
  }
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
  if (camera) {
    camera->processMousePosition(xPos, yPos);
  }
}

void mainLoop(GLFWwindow* window)
{
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    camera->processKeyboardInput(keys, deltaTime);

    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
int main(int argc, char* argv[])
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL",
                                        glfwGetPrimaryMonitor(), nullptr);
  if (window == nullptr)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return EXIT_FAILURE;
    }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;

  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
    return EXIT_FAILURE;
  }

  camera = new Camera();

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mouseCallback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_REPLACE);

  init_resources();

  mainLoop(window);

  free_resources();
  glfwTerminate();
  return EXIT_SUCCESS;
}
