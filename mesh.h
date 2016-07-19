#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Texture {
  GLuint id;
  std::string type;
  aiString path;
};

class Mesh {
 public:
  /* Mesh data */
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;
  /* Functions */
  Mesh(std::vector<Vertex> vertices,
       std::vector<GLuint> indices,
       std::vector<Texture> textures);
  void Draw(GLuint shader);
 private:
  /* Render data */
  GLuint VAO, VBO, EBO;
  void setupMesh();
};

#endif /* MESH_H */
