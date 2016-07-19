#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "shader_loader.h"

using namespace std;

string read_file(string filename) {
  ifstream infile(filename.c_str());
  string s;
  stringstream ss;
  while (getline(infile, s)) {
    ss << s << endl;
  }
  return ss.str();
}

void ShaderLoader::loadShaders(string vertex_shader_file,
                               string fragment_shader_file,
                               bool verbose)
{
  GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

  if (verbose) {
    cout << "Reading vertex shader from file: " << vertex_shader_file << endl;
  }
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  string vs_source_str = read_file(vertex_shader_file);
  const char *vs_source = vs_source_str.c_str();
  if (verbose) {
    cout << "Shader source:" << endl;
    cout << vs_source_str << endl;
  }
  glShaderSource(vs, 1, &vs_source, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    throw domain_error("Error in vertex shader");
  }

  if (verbose) {
    cout << "Reading fragment shader from file: " << fragment_shader_file << endl;
  }
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  string fs_source_str = read_file(fragment_shader_file);
  const char *fs_source = fs_source_str.c_str();
  if (verbose) {
    cout << "Shader source:" << endl;
    cout << fs_source_str << endl;
  }
  glShaderSource(fs, 1, &fs_source, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    throw domain_error("Error in fragment shader");
  }

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    throw domain_error("Error in glLinkProgram");
  }

  glDeleteShader(fs);
  glDeleteShader(vs);
}
