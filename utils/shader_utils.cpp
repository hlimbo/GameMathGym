#include "shader_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __EMSCRIPTEN__
// Handle Shader Specific version headers for web
  const char* versionHeader = "#version 300 es\n";
  // 16-bit floats supported on web
  const char* glslPrecision = "precision mediump float;\n";
#else
// Handle Shader Specific version headers for windows pc desktop
  const char* versionHeader = "#version 330 core\n";
  const char* glslPrecision = "";
#endif

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

GLuint ShaderUtils::LoadAndCreateShaderSource(const std::string& filePath, GLenum shaderType) {
  std::string shaderSourceCode(ShaderUtils::LoadShaderSource(filePath));

  const char* shaderCodeArr[3] = {
    versionHeader,
    glslPrecision,
    shaderSourceCode.c_str()
  };

  GLuint shaderId = glCreateShader(shaderType);
  if (shaderId == 0) {
    std::cout << "LoadAndCreateShaderSource error! Invalid shaderType parameter passed in. Can only use GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, or GL_GEOMETRY_SHADER as possible enums." << std::endl;
  }

  glShaderSource(shaderId, 3, shaderCodeArr, NULL);
  glCompileShader(shaderId);

  std::cout << "Shader Code Loaded: " << filePath << "\n";
  std::cout << versionHeader << "\n";
  std::cout << glslPrecision << "\n";
  std::cout << shaderSourceCode << "\n";

  return shaderId;
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

GLuint ShaderUtils::CreateShaderProgram(GLuint vertexShaderId, GLuint fragShaderId) {
  GLuint shaderProgramId = glCreateProgram();
  glAttachShader(shaderProgramId, vertexShaderId);
  glAttachShader(shaderProgramId, fragShaderId);
  glLinkProgram(shaderProgramId);

  return shaderProgramId;
}