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

const GLuint WIDTH = 1600, HEIGHT = 1000;

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
  glStencilMask(0x00);

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

void render() {
  glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
  glStencilMask(0xFF);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glm::mat4 view = camera->getView();

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f),
                                (GLfloat) WIDTH / (GLfloat) HEIGHT,
                                0.1f, 1000.0f);

  glStencilMask(0x00);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  renderScene(sl->getProgram(), view, projection);

  for (GLuint i = 0; i < 2; i++) {
    if (portals[i] != nullptr) {
      portals[i]->Draw(portalSl->getProgram(), view, projection);
    }
  }

  for (GLuint i = 0; i < 2; i++) {
    if (portals[i] != nullptr) {
      portals[i]->DrawStencil(portalStencilSl->getProgram(), view, projection, i+1);
    }
  }

  for (GLuint i = 0; i < 2; i++) {
    if (portals[i] != nullptr && portals[i]->IsLinked()) {
      view = portals[i]->GetView();
      glClear(GL_DEPTH_BUFFER_BIT);
      glStencilFunc(GL_EQUAL, i+1, 0xFF);
      renderScene(sl->getProgram(), view, projection);
    }
  }
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

  if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
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

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr,
                                        nullptr);
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
