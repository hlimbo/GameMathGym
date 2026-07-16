#include "shader_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::string ShaderUtils::LoadShaderSource(const std::string& filePath) {
  std::ifstream shaderFile;
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    shaderFile.open(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
  }
  catch (std::ifstream::failure& e) {
    std::cerr << "Error: Shader File not successfully read at " << filePath << " with exception: " << e.what() << std::endl;
    return "";
  }
}

void ShaderUtils::VerifyShaderCompilationStatus(GLuint shaderHandle, const std::string& shaderFilePath) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
    std::cout << "Error Shader Compilation Failed at " << shaderFilePath << "\nWith Error Message: " << infoLog << std::endl;
  }
}

void ShaderUtils::VerifyShaderProgramLinkStatus(GLuint shaderProgram) {
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "Error Shader Program Link Failed: " << infoLog << std::endl;
  }
}