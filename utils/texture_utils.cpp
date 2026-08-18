#include "texture_utils.h"

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

uint32_t TextureUtils::loadTexture(const char* path) {
  uint32_t textureId;
  glGenTextures(1, &textureId);

  int width, height, colorChannels;
  unsigned char* data = stbi_load(path, &width, &height, &colorChannels, 0);

  if (data) {
    GLenum format;
    switch (colorChannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Texture failed to load at path: " << path << "\n";
  }

  stbi_image_free(data);
  data = nullptr;

  return textureId;
}
