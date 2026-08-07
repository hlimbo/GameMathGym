#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <iostream>

#include "utils/shader_utils.h"
#include "math_utils/matrix4.h"
#include "math_utils/vector3.h"
#include "camera/camera.h"

const char* WINDOW_NAME = "Shapes Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

/* ----------------- Shapes --------------------- */
// Since I'm using Model View Projection Matrices to transform
// these points, these will be in local space coordinates initially
float quadVertices[] = {
  -1.0f, 1.0f, 0.0f,  // top-left corner
  -1.0f, -1.0f, 0.0f, // bottom-left corner
   1.0f, 1.0f, 0.0f,  // top-right corner
   1.0f, -1.0f, 0.0f, // bottom-right corner
};
// This reuses vertices to draw separate triangles to form the quad
unsigned int quadIndices[] = {
  0, 1, 2,
  2, 3, 1
};

float cubeVertices[] = {
  -1.0f, 1.0f, 1.0f,      // front top left
  0.0f, 0.0, 1.0f, 1.0f,  // blue
  1.0f, 1.0f, 1.0f,       // front top right
  0.0f, 0.0, 1.0f, 1.0f,  // blue
  -1.0f, -1.0f, 1.0f,     // front bottom left
  0.0f, 0.0, 1.0f, 1.0f,  // blue
  1.0f, -1.0f, 1.0f,      // front bottom right
  0.0f, 0.0, 1.0f, 1.0f,  // blue

  -1.0f, 1.0f, -1.0f,     // back top left
  1.0f, 0.0, 0.0f, 1.0f,  // red
  1.0f, 1.0f, -1.0f,      // back top right
  1.0f, 0.0, 0.0f, 1.0f,  // red
  -1.0f, -1.0f, -1.0f,    // back bottom left
  1.0f, 0.0, 0.0f, 1.0f,  // red
  1.0f, -1.0f, -1.0f,     // back bottom right
  1.0f, 0.0, 0.0f, 1.0f,  // red
};

// In OpenGL the vertex draw order should in counter-clockwise
// so that the faces remain visible. Drawing in a clockwise order results in back faces being culled
// Common Gotcha: these represent EBO indices or indices for each vertex datapoint. If new vertex attributes are introduced such as normals
// I don't need to update these values so that it maps to the correct index as vertexAttribPointer function already handles that for us behind the scenes
unsigned int cubeIndices[] = {
  // Front Face
  1, 0, 2,
  3, 1, 2,

  // Back Face
  4, 5, 6,
  6, 5, 7,

  // Left Face
  6, 0, 2,
  4, 6, 0,

  // Right Face
  7, 5, 1,
  3, 7, 1,

  // Top Face
  5, 4, 0,
  0, 1, 5,

  // Bot Face
  6, 7, 2,
  2, 7, 3
};

/* 24 vertices cube */
float cube24Vertices[] = {
  // front face
  -0.5f, 0.5f, 0.5f,
   1.0f, 0.0f, 0.0f, 1.0f, // red
   0.5f, 0.5f, 0.5f,
   1.0f, 0.0f, 0.0f, 1.0f, // red
  -0.5f, -0.5f, 0.5f,
   1.0f, 0.0f, 0.0f, 1.0f, // red
   0.5f, -0.5f, 0.5f,
   1.0f, 0.0f, 0.0f, 1.0f, //red

  // back face
  -0.5f, 0.5f, -0.5f,
  0.0f, 1.0f, 0.0f, 1.0f, // green
  0.5f, 0.5f, -0.5f,
  0.0f, 1.0f, 0.0f, 1.0f, // green
  -0.5f, -0.5f, -0.5f,
  0.0f, 1.0f, 0.0f, 1.0f, // green
  0.5f, -0.5f, -0.5f,
  0.0f, 1.0f, 0.0f, 1.0f, // green

  //left face
  -0.5f, 0.5f, 0.5f,
  0.0f, 0.0f, 1.0f, 1.0f, // blue
  -0.5f, 0.5f, -0.5f,
  0.0f, 0.0f, 1.0f, 1.0f, // blue
  -0.5f, -0.5f, 0.5f,
  0.0f, 0.0f, 1.0f, 1.0f, // blue
  -0.5f, -0.5f, -0.5f,
  0.0f, 0.0f, 1.0f, 1.0f, // blue

  // right face
  0.5f, 0.5f, 0.5f,
  1.0f, 0.0f, 1.0f, 1.0f, // purple
  0.5f, 0.5f, -0.5f,
  1.0f, 0.0f, 1.0f, 1.0f, // purple
  0.5f, -0.5f, 0.5f,
  1.0f, 0.0f, 1.0f, 1.0f, // purple
  0.5f, -0.5f, -0.5f,
  1.0f, 0.0f, 1.0f, 1.0f, // purple

  // top face
  -0.5f, 0.5f, 0.5f,
  1.0f, 1.0f, 0.0f, 1.0f, // yellow
  0.5f, 0.5f, 0.5f,
  1.0f, 1.0f, 0.0f, 1.0f, // yellow
  -0.5f, 0.5f, -0.5f,
  1.0f, 1.0f, 0.0f, 1.0f, // yellow
  0.5f, 0.5f, -0.5f,
  1.0f, 1.0f, 0.0f, 1.0f, // yellow

  // bottom face
  -0.5f, -0.5f, 0.5f,
  0.32f, 0.5f, 0.32f, 1.0f, // color
  0.5f, -0.5f, 0.5f,
  0.32f, 0.5f, 0.32f, 1.0f, // color
  -0.5f, -0.5f, -0.5f,
  0.32f, 0.5f, 0.32f, 1.0f, // color
  0.5f, -0.5f, -0.5f,
  0.32f, 0.5f, 0.32f, 1.0f, // color
};

unsigned int cube24Indices[] = {
  // front face
  1, 0, 2,
  3, 1, 2,

  // Back Face
  4, 5, 6,
  6, 5, 7,

  // Left Face
  8, 9, 11,
  10, 8, 11,

  // Right Face
  13, 12, 14,
  15, 13, 14,
  //14, 13, 15,

  // Top Face
  19, 18, 16,
  19, 16, 17,

  // Bot Face
  20, 22, 23,
  20, 23, 21
};

// OpenGL default winding order is counter clockwise for the vertices
// unsigned int cube24Indices[] = {
//   // front face
//   0, 1, 2,
//   1, 2, 3,

//   // Back Face
//   4, 5, 6,
//   6, 5, 7,

//   // Left Face
//   8, 11, 9,
//   8, 10, 11,

//   // Right Face
//   12, 14, 13,
//   13, 14, 15,

//   // Top Face
//   16, 17, 18,
//   18, 17, 19,

//   // Bottom Face
//   20, 22, 21,
//   21, 22, 23
// };

/* ----------------- End Shapes ----------------- */


MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -2.0f)));

const float ASPECT = (float)WIDTH / (float)HEIGHT;
const float FOV_DEGREES = 90.0f;
const float NEAR = 0.1f;
const float FAR = 1000.0f;
Core::Camera mainCamera((float)WIDTH, (float)HEIGHT, FOV_DEGREES, NEAR, FAR);

GLuint shaderProgramId;

GLuint VAO;
GLuint VBO;
GLuint EBO;

// keyboard controls
const bool* keyStates = NULL;

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

  /* Shader Initialization */
  {
    std::string vertShaderSrc("shaders/cube.vert");
    std::string fragShaderSrc("shaders/cube.frag");
    GLuint vertexShaderId = ShaderUtils::LoadAndCreateShaderSource(vertShaderSrc, GL_VERTEX_SHADER);
    GLuint fragShaderId = ShaderUtils::LoadAndCreateShaderSource(fragShaderSrc, GL_FRAGMENT_SHADER);

    ShaderUtils::VerifyShaderCompilationStatus(vertexShaderId, vertShaderSrc);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc);

    shaderProgramId = ShaderUtils::CreateShaderProgram(vertexShaderId, fragShaderId);

    ShaderUtils::VerifyShaderProgramLinkStatus(shaderProgramId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragShaderId);
  }

  /* Shader Buffer Setup */
  {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube24Vertices), &cube24Vertices[0], GL_STATIC_DRAW);

    GLsizei vertexPositionStride = 7 * sizeof(float);
    // Vertex Position at Location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)0);

    //Vertex Color at Location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)(sizeof(float) * 3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube24Indices), &cube24Indices[0], GL_STATIC_DRAW);

    // unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind VAO
    glBindVertexArray(0);
    // unbind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  /* Setup Keyboard Controls */
  keyStates = SDL_GetKeyboardState(NULL);

  // Draw vertices that are facing in front of camera and vertices behind other vertices are culled away
  glEnable(GL_DEPTH_TEST);
  // discards triangles facing away from camera
  glEnable(GL_CULL_FACE);


  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    std::cout << "quitting app" << std::endl;
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

MathUtils::Vector3 dirInputs;
MathUtils::Vector3 mainCamPosition(0.0f, 0.0f, -2.0f);
float speed = 2.0f;
// measured in milliseconds
Uint64 currentTime, lastTime = 0;

SDL_AppResult SDL_AppIterate(void *appstate) {
  currentTime = SDL_GetTicks();
  if (lastTime == 0) {
    lastTime = currentTime;
  }

  // measured in seconds
  float deltaTime = (float)(currentTime - lastTime) / SDL_MS_PER_SECOND;
  lastTime = currentTime;

  /* Keyboard Controls */
  if (keyStates[SDL_SCANCODE_A]) {
    dirInputs.x = 1.0f;
  } else if (keyStates[SDL_SCANCODE_D]) {
    dirInputs.x = -1.0f;
  } else {
    dirInputs.x = 0.0f;
  }

  if (keyStates[SDL_SCANCODE_W]) {
    dirInputs.z = 1.0f;
  } else if (keyStates[SDL_SCANCODE_S]) {
    dirInputs.z = -1.0f;
  } else {
    dirInputs.z = 0.0f;
  }

  if (dirInputs.sqrMagnitude() != 0.0f) {
    dirInputs.normalize();
  }

  MathUtils::Vector3 velocity(dirInputs * speed * deltaTime);
  mainCamPosition += velocity;

  /* Mouse Controls */
  // float mouseX, mouseY;
  // SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(&mouseX, &mouseY);
  // bool isLeftMouseClick = mouseFlags & SDL_BUTTON_LMASK;
  // MathUtils::Matrix4 newLookAtMatrix(mainCamera.getViewMatrix());
  // if (isLeftMouseClick) {
  //   MathUtils::Vector3 viewCoords = MathUtils::screenSpaceToViewSpace(mouseX, mouseY, (float)WIDTH, (float)HEIGHT, mainCamera.getProjectionMatrix());
  //   MathUtils::Vector3 worldCoords = MathUtils::screenSpaceToWorldSpace(mouseX, mouseY, 0.1f, (float)WIDTH, (float)HEIGHT, NEAR, FAR, mainCamera.getProjectionMatrix(), mainCamera.getViewMatrix(), true);


  //   MathUtils::Vector3 up(0.0f, 1.0f, 0.0f);
  //   newLookAtMatrix = MathUtils::lookAt(mainCamPosition, mainCamPosition + viewCoords, up);
  // }

  // MathUtils::Matrix4 translationMat(MathUtils::makeTranslationMatrix(velocity));
  // newLookAtMatrix = newLookAtMatrix * translationMat;
  // mainCamera.setViewMatrix(newLookAtMatrix);

  /* Rotate Around the cube in world space */
  MathUtils::Vector3 targetPosition(0.0f, 0.0f, -2.0f);
  float radius = 4.0f;
  float angle = std::fmod(static_cast<float>(currentTime) / SDL_MS_PER_SECOND, 6.28318530718f);
  float camX = static_cast<float>(std::sin(angle) * radius) + targetPosition.x;
  float camZ = static_cast<float>(std::cos(angle) * radius) + targetPosition.z;
  MathUtils::Vector3 srcPosition(camX, camX, camZ);
  MathUtils::Vector3 worldUp(0.0f, 1.0f, 0.0f);
  MathUtils::Matrix4 newViewMat = MathUtils::lookAt(srcPosition, targetPosition, worldUp);
  mainCamera.setViewMatrix(newViewMat);

  // render background solid color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shaderProgramId);

  auto viewMat = mainCamera.getViewMatrix();
  auto projMat = mainCamera.getProjectionMatrix();

  // send model view projection matrices to vertex shader
  int modelLoc = glGetUniformLocation(shaderProgramId, "model");
  int viewLoc = glGetUniformLocation(shaderProgramId, "view");
  int projLoc = glGetUniformLocation(shaderProgramId, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMat.cells[0]);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat.cells[0]);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat.cells[0]);


  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, sizeof(cube24Indices), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  SDL_GL_SwapWindow(win);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}