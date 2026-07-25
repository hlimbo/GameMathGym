#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#include "math_utils/matrix4.h"
#include "math_utils/vector3.h"
using namespace MathUtils;

const char* WINDOW_NAME = "Draw Triangle Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

// Valid ranges in the x,y,z coordinates in OpenGL go between -1.0 and 1.0 for all 3 axes.
// It uses Normalized Device Coordinates. The coordinates below get processed by the vertex shader in the graphics pipeline.
float vertices[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.5f, 0.0f
};
// Vertex Buffer Object (VBO) - stores vertices in the GPU's memory
// It is slow to send vertices that live on the CPU to GPU and is preferred to batch send a ton of vertices at once
GLuint VBO;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;
// Vertex Array Object (VAO) - this will be used to draw the triangle
GLuint VAO;

/* Model View Projection Matrices */
float degrees = -55.0f;
float radians = degrees * (MathUtils::PI / 180.0f); // rotate triangle about x axis (-55 degrees initial)
Matrix4 modelMat(MathUtils::MAT4_IDENTITY);//MathUtils::rotate(MathUtils::MAT4_IDENTITY, 

float closeZ = -5.0f; // in opengl things move inverted fashion, this represents going forward which means the object goes further away
float farZ = -1.0f; // this means the object is getting closer
float z = -1.0f;
float zSpeed = 8.0f;
float zDir = -1.0f;
Matrix4 viewMat(MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, z)));

// timer - to keep track of delta time for the game loop
Uint64 lastTime = 0;

// float left = -5.0f, right = 5.0f;
// float bot = -5.0f, top = 5.0f;
// float near = -50.0f, far = 100.0f; // depth vision test anything outside this range means you don't see the triangle
// Matrix4 projMat(MathUtils::createOrthographicMatrix(left, right, bot, top, near, far));

float left = -0.5f, right = 0.5f;
float bot = -0.5f, top = 0.5f;
float near = 0.1f; // needs to be positive for perspective because it will cause one of the cells in the matrix to either go negative or do a division by zero which isn't supported for perspective projection
float far = 100.0f; // must be greater than near
Matrix4 projMat(MathUtils::createPerspectiveMatrix(left, right, bot, top, near, far));

// Determine shader version header based on target platform
#ifdef __EMSCRIPTEN__
  const char* versionHeader = "#version 300 es\n";
  const char* glslPrecision = "precision mediump float;\n"; // 16-bit floats are supported on web...
#else
  const char* versionHeader = "#version 330 core\n";
  const char* glslPrecision = "";
#endif

std::string loadShaderSource(const std::string& filePath) {
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
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
    return "";
  }
}

void verifyShaderCompilationStatus(GLuint shaderHandle,const std::string& shaderFilePath) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\nFile Path: " << shaderFilePath << std::endl; 
  }
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
    // GL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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

  /* Triangle Initialization */
  {
    // Load in vertex shader
    std::string vertexCodeStr(loadShaderSource("shaders/triangle.vert"));
    const char* vertexCodeArr[2] = { 
      versionHeader, 
      vertexCodeStr.c_str() 
    };
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 2, vertexCodeArr, NULL);
    glCompileShader(vertexShader);

    // verify vertex shader compiled successfully
    verifyShaderCompilationStatus(vertexShader, "shaders/triangle.vert");

    // Load in Fragment Shader
    std::string fragShaderStr(loadShaderSource("shaders/triangle.frag"));
    const char* fragShaderArr[3] = {
      versionHeader,
      glslPrecision,
      fragShaderStr.c_str()
    };
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 3, fragShaderArr, NULL);
    glCompileShader(fragmentShader);

    // verify frag shader compiled successfully
    verifyShaderCompilationStatus(fragmentShader, "shaders/triangle.frag");

    // Shader Program - link vertex and frag shaders together
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Verify Shader Program successfully linked
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl; 
    }

    // no longer need the shader objects once linked to program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO -- used to draw the triangle
    // Why use VAOs to draw? VAO act like pointers that let you assign the base address of VBOs to draw and can be reused
    // VAO bundles all vertex data configs so I don't ned to map them out line-by-line right before every single draw call
    // IMPORTANT: OpenGL requires to use VAO for drawing and must be initialized first followed by the VBO!
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // generates 1 or more buffer objects on GPU
    // VBO is an identifier or a handle that lets you access the Vertex Buffer Object
    glGenBuffers(1, &VBO);
    // binds current VBO to be an array buffer
    // can bind it to either GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // allocates memory and copies the data to the VBO -- copies vertices into the GPU
    // GL_STATIC_DRAW means the data is set once and is used many times -- when you do glBindBuffer, glBufferData knows it is copying the vertices data to VBO through glBindBuffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Linking Vertex Attributes
    // specifies how opengl should interpret Vertex Buffer Object
    // whenever a draw call is made
    // 1st arg specifies vertex index
    // 2nd arg specifies number of elements per grouping (3 for the 3 axes)
    // 3rd arg specifies the data type
    // 4th arg specifies if the coordinates are normalized or not
    // 5th arg specifies the stride or length of each data structure in the tightly packed float[] array
    // 6th arg specifies the offset for OpenGL to start from -- here it says start at the beginning
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind the VBO from previous call to glBindBuffer(GL_ARRAY_BUFFER, VBO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // this unbinds the VAO from previous call to glBindVertexArray(VAO)
    glBindVertexArray(0);

    // disable face culling
    glDisable(GL_CULL_FACE);
  }

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

  // code snippet to get delta time so triangle rotates and moves same rate no matter which computer hardware is used...
  Uint64 currentTime = SDL_GetPerformanceCounter();

  if (lastTime == 0) {
    lastTime = currentTime;
  }

  double deltaTime = (double)(currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();
  lastTime = currentTime;

  // Rotate Triangle about x axis
  float rotationSpeed = 120.0f;
  degrees = (degrees + rotationSpeed * deltaTime);
  //if (degrees > 360.0f) { degrees = 0.0f; }
  radians = degrees * (MathUtils::PI / 180.0f); // rotate triangle about x axis (-55 degrees initial)
  modelMat = MathUtils::rotate(MathUtils::MAT4_IDENTITY, radians, Vector3(1.0f, 0.0f, 0.0f));

  // Code Snippet to move triangle back and forth along the z-axis
  // if (z >= farZ || z <= closeZ) {
  //   zDir *= -1.0f;
  // }
  // z += zDir * zSpeed * (float)deltaTime;
  // std::cout << "z: " << z << std::endl;
  // viewMat = MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, z));


  // render background solid color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);

  // send model view projection matrices to shader program
  int modelLoc = glGetUniformLocation(shaderProgram, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMat.cells[0]);
  int viewLoc = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat.cells[0]);
  int projLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat.cells[0]);

  // draw the triangle
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);

  SDL_GL_SwapWindow(win);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}