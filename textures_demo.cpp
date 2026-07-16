#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION // this ensures the code can compile and successfully link -- without this, it will generate LNK2019 errors for stbi_load and stbi_image_free functions
#include <stb_image.h>

#include <iostream>

#include "utils/shader_utils.h"

#ifdef __EMSCRIPTEN__
  const char* versionHeader = "#version 300 es\n";
  const char* glslPrecision = "precision mediump float;\n"; // 16-bit floats supported on web
#else
  const char* versionHeader = "#version 330 core\n";
  const char* glslPrecision = "";
#endif

const char* WINDOW_NAME = "Textures Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

GLuint vertexShader, fragShader, shaderProgram;
GLuint VAO, VBO, EBO;

// triangle vertex coordinates on Normalized Device Coordinates
float vertices[] = {
// positions             // colors               // texture coords
   0.5f, 0.5f, 0.0f,     1.0, 0.0f, 0.0f,        1.0f, 1.0f,   // top right 
   0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,       1.0f, 0.0f,    // bottom right   
  -0.5f, -0.5f, 0.0f,    0.0, 0.0f, 1.0f,        0.0f, 0.0f,   //bottom left
  -0.5f, 0.5f, 0.0f,     1.0f, 1.0f, 0.0f,       0.0f, 1.0f    // top left
};

GLuint indices[] = {
  0, 1, 3, // first triangle
  1, 2, 3  // second triangle
};

// texture data;
int width, height, nrChannels;
unsigned char *data = nullptr;
GLuint texture;

const char* vertexSrc = "shaders/texture.vert";
const char* fragSrc = "shaders/texture.frag";

void InitializeTriangle() {
  // Load in Vertex and Fragment Shaders
  std::string vertexCodeStr(ShaderUtils::LoadShaderSource(vertexSrc));
  std::string  fragShaderStr(ShaderUtils::LoadShaderSource(fragSrc));

  const char* vertexCodeArr[2] { versionHeader, vertexCodeStr.c_str() };
  const char* fragShaderArr[3] { versionHeader, glslPrecision, fragShaderStr.c_str() };

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 2, vertexCodeArr,  NULL);
  glCompileShader(vertexShader);

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 3, fragShaderArr, NULL);
  glCompileShader(fragShader);

  ShaderUtils::VerifyShaderCompilationStatus(vertexShader, vertexSrc);
  ShaderUtils::VerifyShaderCompilationStatus(fragShader, fragSrc);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragShader);
  glLinkProgram(shaderProgram);

  ShaderUtils::VerifyShaderProgramLinkStatus(shaderProgram);

  // no longer needed as these shader objects are linked to the shader program
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // vertex coord attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);  

  // colors coord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // texture coord attribute
  // stride (specifies byte offset between consecutive generic vertex attributes) 
  //    --> 8 * sizeof(float) = 32 bits offset (think of this as the spacing between)
  // pointer ( specifies an offset of first component of first generic vertex attribute in array)
  //    --> (void*)(6 * sizeof(float)) = 24 bits offset where the first element begins
  //    --> it is casted as a (void*) pointer to denote this value is used to shift the starting address of this vertex attribute 
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // unbind VAO from previous call to glBindVertexArray(VAO)
  glBindVertexArray(0);
}

void DeleteRect() {
  std::cout << "GL VAO: " << VAO << std::endl;
  std::cout << "GL VBO: " << VBO << std::endl;
  std::cout << "GL EBO: " << EBO << std::endl;
  std::cout << "A VAO: " << &VAO << std::endl;
  std::cout << "A VBO: " << &VBO << std::endl;
  std::cout << "A EBO: " << &EBO << std::endl;

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void DrawRect() {
  glUseProgram(shaderProgram);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(VAO);
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // the error is happening here...
  glBindVertexArray(0);
}

void LoadImageTexture() {
  data = stbi_load("3d_models/Raiden/Tex_0302_0.png", &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
    return;
  }
  
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // set texture filtering parameters
  // https://docs.gl/gl3/glTexParameter
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // magnify glass filter
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // 1st - arg - specifies texture target which means it will generate a texture on bound texture object
  // 2nd - specifies mip map level for the texture, leave it at base level 0
  // 3rd - store texture in RGB format (no alpha)
  // 4th - sets width
  // 5th - sets height
  // 6th - always set to 0 due to legacy reasons I don't know
  // 7th - specify color format
  // 8th - specify the type
  // 9th - image data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  // automatically generate all required mipmaps for the bound texture for us
  glGenerateMipmap(GL_TEXTURE_2D);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

  // Select GL version
  #if defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #elif defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #else
    // GL 3.0 + generally GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY; 
  win = SDL_CreateWindow(WINDOW_NAME, (int)(WIDTH * mainScale), (int)(HEIGHT * mainScale), windowFlags);


  glContext = SDL_GL_CreateContext(win);
  SDL_GL_MakeCurrent(win, glContext);
  SDL_GL_SetSwapInterval(1); // enable vsync
  SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(win);

  #ifndef __EMSCRIPTEN__
  // Setup Glad Library
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    return SDL_APP_FAILURE;
  }
  #endif

  /* Verify hardware that GPU will be using */
  {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  }

  LoadImageTexture();
  InitializeTriangle();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    std::cout << "quitting app" << std::endl;
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

  // render background solid color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  DrawRect();

  SDL_GL_SwapWindow(win);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (data != nullptr) {
    stbi_image_free(data);
  }

  DeleteRect();

  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}