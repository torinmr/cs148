#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "mesh.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
  cout << "Creating mesh" << endl;
  cout << "Num vertices: " << vertices.size() << endl;
  cout << "Num indices: " << indices.size() << endl;
  cout << "Num textures: " << textures.size() << endl;
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  this->setupMesh();
}

void Mesh::setupMesh()
{
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);
  {
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
                 &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
                 &this->indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*) 0);
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*) offsetof(Vertex, Normal));
    // Vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*) offsetof(Vertex, TexCoords));
  }
  glBindVertexArray(0);
}

void Mesh::Draw(GLuint shader)
{
  GLuint diffuseNum = 1;
  GLuint specularNum = 1;

  for (GLuint i = 0; i < this->textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    stringstream ss;
    string number;
    string name = this->textures[i].type;
    if (name == "texture_diffuse") {
      ss << diffuseNum;
      diffuseNum++;
    } else if (name == "texture_specular") {
      ss << specularNum;
      specularNum++;
    }
    number = ss.str();

    string uniformName = name + number;
    GLuint uniformLocation = glGetUniformLocation(shader, uniformName.c_str());
    glUniform1i(uniformLocation, i);
    glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  for (GLuint i = 0; i < this->textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
