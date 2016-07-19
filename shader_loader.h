#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <GL/glew.h>

class ShaderLoader {
 public:
  ShaderLoader(std::string vertex_shader_file,
               std::string fragment_shader_file,
               bool verbose = false) {
    loadShaders(vertex_shader_file, fragment_shader_file, verbose);
  };

  ~ShaderLoader() { glDeleteProgram(program); };
  // TODO: Figure out how to disallow copy/assign.

  void useProgram() const { glUseProgram(program); };
  GLuint getProgram() const { return program; };
 private:
  GLuint program;
  void loadShaders(std::string vertex_shader_file,
                   std::string fragment_shader_file,
                   bool verbose);
};

#endif /* SHADER_LOADER_H */
