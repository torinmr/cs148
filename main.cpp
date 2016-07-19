#include <cstdlib>
#include <iostream>
#include <string>

#define GLEW_STATIC
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "camera.h"
#include "model.h"
#include "shader_loader.h"

using namespace std;

ShaderLoader *sl = nullptr;
Camera *camera = nullptr;
bool keys[1024];

Model *myModel;

const GLuint WIDTH = 800, HEIGHT = 600;

bool init_resources(void) {
  sl = new ShaderLoader("vertex.glsl",
                        "fragment.glsl");

  myModel = new Model("nanosuit/nanosuit.obj");

  return true;
}

void free_resources(void) {
  if (sl != nullptr) {
    delete sl;
  }
  if (camera != nullptr) {
    delete camera;
  }
  if (myModel != nullptr) {
    delete myModel;
  }
}

void render() {
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model;
  //  model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
  //  model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

  glm::mat4 view = camera->getView();

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f),
                                (GLfloat) WIDTH / (GLfloat) HEIGHT,
                                0.1f, 100.0f);

  GLuint modelLoc = glGetUniformLocation(sl->getProgram(), "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  GLuint viewLoc = glGetUniformLocation(sl->getProgram(), "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  GLuint projectionLoc = glGetUniformLocation(sl->getProgram(), "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  glUseProgram(sl->getProgram());
  myModel->Draw(sl->getProgram());
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

  init_resources();

  mainLoop(window);

  free_resources();
  glfwTerminate();
  return EXIT_SUCCESS;
}
