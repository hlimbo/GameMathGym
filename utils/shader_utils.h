#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include <string>

namespace ShaderUtils {
  std::string LoadShaderSource(const std::string& filePath);
  void VerifyShaderCompilationStatus(GLuint shaderHandle, const std::string& shaderFilePath);
  void VerifyShaderProgramLinkStatus(GLuint shaderProgram);
}


#endif

