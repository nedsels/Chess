#include "shaders.h"

Shader::Shader() { ID = -1; }

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
  // 1. retreive the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vertexShaderFile;
  std::ifstream fragmentShaderFile;

  // ensure ifstream objects can throw exceptions
  vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vertexShaderFile.open(vertexPath);
    fragmentShaderFile.open(fragmentPath);
    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;

    vertexShaderStream << vertexShaderFile.rdbuf();
    fragmentShaderStream << fragmentShaderFile.rdbuf();

    vertexShaderFile.close();
    fragmentShaderFile.close();

    vertexCode = vertexShaderStream.str();
    fragmentCode = fragmentShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  const char* vertexShaderCode = vertexCode.c_str();
  const char* fragmentShaderCode = fragmentCode.c_str();

  // 2. compile shaders
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
  glCompileShader(vertexShader);

  int success_vertex;
  char infoLog_vertex[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success_vertex);
  if (!success_vertex) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog_vertex);
    std::cout << "ERROR::" << vertexPath << "::COMPILATION_FAILED\n"
              << infoLog_vertex << std::endl;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
  glCompileShader(fragmentShader);

  int success_fragment;
  char infoLog_fragment[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success_fragment);
  if (!success_fragment) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog_fragment);
    std::cout << "ERROR::" << fragmentPath << "::COMPILATION_FAILED\n"
              << infoLog_fragment << std::endl;
  }

  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  int success_shaderProgram;
  char infoLog_shaderProgram[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &success_shaderProgram);
  if (!success_shaderProgram) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog_shaderProgram);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog_shaderProgram << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(ID); }

void Shader::set1b(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set1i(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set1f(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set2f(const std::string& name, float value1, float value2) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
}

void Shader::set2f(const std::string& name, glm::vec2 vector) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), vector.x, vector.y);
}

void Shader::set3f(const std::string& name, float value1, float value2,
                   float value3) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
}

void Shader::set3f(const std::string& name, glm::vec3 vector) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), vector.x, vector.y,
              vector.z);
}

void Shader::set4f(const std::string& name, float value1, float value2,
                   float value3, float value4) const {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3,
              value4);
}

void Shader::set1mat4f(const std::string& name, glm::mat4 matrix) {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}