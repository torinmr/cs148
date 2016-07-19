#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <GL/glew.h>

#include "mesh.h"

using namespace std;

class Model
{
 public:
  Model(string path);
  void Draw(GLuint shader);
 private:
  std::vector<Mesh> meshes;
  std::string directory;
  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                       std::string typeName);
  std::vector<Texture> textures_loaded;
};

#endif /* MODEL_H */
