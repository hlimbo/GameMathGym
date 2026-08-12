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
#include <cmath>

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

std::vector<float> generateUnitCircleCoordinates(int sectorCount);
std::vector<float> createCylinderVertices(int sectorCount, float topRadius, float bottomRadius, float height);
std::vector<unsigned int> createCylinderIndices(int sectorCount);
std::vector<float> createConeVertices(int sectorCount, int stackCount, float baseRadius, float height);
std::vector<unsigned int> createConeIndices(int sectorCount, int stackCount);

std::vector<float> createCircleVertices(int sectorCount, float radius, float zDepth);
std::vector<unsigned int> createCircleIndices(int sectorCount);


/* ----------------- End Shapes ----------------- */


MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -4.0f)));

const float ASPECT = (float)WIDTH / (float)HEIGHT;
const float FOV_DEGREES = 90.0f;
const float NEAR = 0.1f;
const float FAR = 1000.0f;
Core::Camera mainCamera((float)WIDTH, (float)HEIGHT, FOV_DEGREES, NEAR, FAR);

int sectorCount = 32;
float radius = 1.0f;
float height = 4.0f;
std::vector<float> cylinderVertices;
std::vector<unsigned int> cylinderIndices;
std::vector<float> coneVertices;
std::vector<unsigned int> coneIndices;

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

  /* Generate Cylinder Vertices and Indices */
  {
    cylinderVertices = createCylinderVertices(sectorCount, radius, radius, height);
    cylinderIndices = createCylinderIndices(sectorCount);
  }

  /* Generate Cone Vertices and Indices */
  {
    coneVertices = createConeVertices(sectorCount, 4, radius, height);
    coneIndices = createConeIndices(sectorCount, 4);
  }

  /* Shader Initialization */
  {
    std::string vertShaderSrc("shaders/cylinder.vert");
    std::string fragShaderSrc("shaders/cylinder.frag");
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
    
    /* Cylinder Buffer Data Initialization */
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cylinderVertices.size(), cylinderVertices.data(), GL_STATIC_DRAW);

    // GLsizei vertexPositionStride = 3 * sizeof(float);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cylinderIndices.size(), cylinderIndices.data(), GL_STATIC_DRAW);

    /* Cone Buffer Data Initialization */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coneVertices.size(), coneVertices.data(), GL_STATIC_DRAW);
    
    GLsizei vertexPositionStride = 3 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * coneIndices.size(), coneIndices.data(), GL_STATIC_DRAW);
    
    /* Cube Buffer Data Initialization */
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cube24Vertices), cube24Vertices, GL_STATIC_DRAW);
    
    // GLsizei vertexPositionStride = 7 * sizeof(float);
    // // Vertex Position at Location 0
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)0);

    // //Vertex Color at Location 1
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)(sizeof(float) * 3));

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube24Indices), cube24Indices, GL_STATIC_DRAW);


    // unbind VAO
    glBindVertexArray(0);
    // unbind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  /* Setup Keyboard Controls */
  keyStates = SDL_GetKeyboardState(NULL);

  // Draw vertices that are facing in front of camera and vertices behind other vertices are culled away
  glEnable(GL_DEPTH_TEST);
  // discards triangles facing away from camera
  glEnable(GL_CULL_FACE);
  // Enable Wireframe mode for both front and back faces
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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
MathUtils::Vector3 mainCamPosition(0.0f, 0.0f, 5.0f);
MathUtils::Vector3 camFront(0.0f, 0.0f, -1.0f);
MathUtils::Vector3 camUp(0.0f, 1.0f, 0.0f);
float speed = 2.0f;
// measured in milliseconds
Uint64 currentTime, lastTime = 0;

// Mouse Camera Controls
float mX = 0.0f, mY = 0.0f;
float lastMX = 0.0f, lastMY = 0.0f;
float yaw = 0.0f, pitch = 0.0f;
bool isFirstMouse = true;
float mouseSensitivity = 0.025f;

SDL_AppResult SDL_AppIterate(void *appstate) {
  currentTime = SDL_GetTicks();
  if (lastTime == 0) {
    lastTime = currentTime;
  }

  // measured in seconds
  float deltaTime = (float)(currentTime - lastTime) / SDL_MS_PER_SECOND;
  lastTime = currentTime;

  /* Walking Around Camera */
  // Camera View Around based on pitch and yaw degree values
  // SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(&mX, &mY);
  // bool isLeftMouseClicked = mouseFlags & SDL_BUTTON_LMASK;
  // if (isLeftMouseClicked) {

  //   if (isFirstMouse) {
  //     lastMX = mX;
  //     lastMY = mY;
  //     isFirstMouse = false;
  //   }

  //   float xOffset = mX - lastMX;
  //   float yOffset = mY - lastMY;
  //   lastMX = mX;
  //   lastMY = mY;

  //   yaw += xOffset * mouseSensitivity;
  //   pitch += yOffset * mouseSensitivity;

  //   // make sure when pitch is out of bounds, screen doesn't flip
  //   if (pitch > 89.0f) {
  //     pitch = 89.0f;
  //   }
  //   if (pitch < -89.0f) {
  //     pitch = -89.0f;
  //   }
    
  //   // reorient which way camera front is facing based on pitch and yaw
  //   float deg2Rad = (MathUtils::PI / 180.0f);
  //   MathUtils::Vector3 front(
  //     std::cos(deg2Rad * yaw) * std::cos(deg2Rad * pitch),
  //     std::sin(deg2Rad * pitch),
  //     std::sin(deg2Rad * yaw) * std::cos(deg2Rad * pitch)
  //   );
  //   front.normalize();
  //   camFront = front;
  // } else {
  //   isFirstMouse = true;
  // }

  // // cross product is used here to ensure when the camera's orientation changes moving laterally remains consistent
  // dirInputs = MathUtils::Vector3(0.0f, 0.0f, 0.0f);
  // if (keyStates[SDL_SCANCODE_A]) {
  //   dirInputs -= camFront.cross(camUp);
  // } else if (keyStates[SDL_SCANCODE_D]) {
  //   dirInputs += camFront.cross(camUp);
  // }

  // if (keyStates[SDL_SCANCODE_W]) {
  //   dirInputs += camFront;
  // } else if (keyStates[SDL_SCANCODE_S]) {
  //   dirInputs -= camFront;
  // }

  // if (dirInputs.sqrMagnitude() != 0.0f) {
  //   dirInputs.normalize();
  // }

  // MathUtils::Vector3 velocity(dirInputs * speed * deltaTime);
  // mainCamPosition += velocity;

  // MathUtils::Matrix4 newViewMat(MathUtils::lookAt(mainCamPosition, mainCamPosition + camFront, camUp));

  // newViewMat = newViewMat * MathUtils::makeTranslationMatrix(velocity);
  // mainCamera.setViewMatrix(newViewMat);

  /* Rotate Around the cube in world space */
  MathUtils::Vector3 targetPosition(0.0f, 0.0f, -4.0f);
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
  glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);
  // glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT, 0);
  //glDrawElements(GL_TRIANGLES, sizeof(cube24Indices), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

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

std::vector<float> generateUnitCircleCoordinates(int sectorCount) {
  assert(sectorCount > 0);

  std::vector<float> vertices;

  float sectorStep = (2.0f * MathUtils::PI) / sectorCount;
  for (int i = 0;i < sectorCount; ++i) {
    float sectorAngle = i * sectorStep;
    float x = (float)std::cos(sectorAngle);
    float y = 0.0f;
    float z = (float)std::sin(sectorAngle);

    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);

  }

  return vertices;
}

std::vector<float> createCylinderVertices(int sectorCount, float topRadius, float bottomRadius, float height) {
  std::vector<float> vertices;
  std::vector<float> unitVertices = generateUnitCircleCoordinates(sectorCount);

  // bottom center base coordinates
  float botX = 0.0f, botY = -height / 2.0f, botZ = 0.0f;
  // top center base coordinates
  float topX = 0.0f, topY = height / 2.0f, topZ = 0.0f;

  vertices.push_back(botX);
  vertices.push_back(botY);
  vertices.push_back(botZ);
  vertices.push_back(topX);
  vertices.push_back(topY);
  vertices.push_back(topZ);

  // bottom cap
  for (int i = 0;i < unitVertices.size(); i += 3) {
    float ux = unitVertices[i];
    float uy = -height / 2.0f;
    float uz = unitVertices[i+2];

    vertices.push_back(bottomRadius * ux);
    vertices.push_back(uy);
    vertices.push_back(bottomRadius * uz);
  }

  // top cap
  for (int i = 0;i < unitVertices.size(); i += 3) {
    float ux = unitVertices[i];
    float uy = height / 2.0f;
    float uz = unitVertices[i+2];

    vertices.push_back(topRadius * ux);
    vertices.push_back(uy);
    vertices.push_back(topRadius * uz);
  }

  // side vertices
  for (int i = 0;i < unitVertices.size(); i += 3) {
    vertices.push_back(bottomRadius * unitVertices[i]);
    vertices.push_back(-height / 2.0f);
    vertices.push_back(bottomRadius * unitVertices[i+2]);
  }

  for (int i = 0;i < unitVertices.size(); i += 3) {
    vertices.push_back(topRadius * unitVertices[i]);
    vertices.push_back(height / 2.0f);
    vertices.push_back(topRadius * unitVertices[i+2]);
  }

  return vertices;
}

std::vector<unsigned int> createCylinderIndices(int sectorCount) {
  std::vector<unsigned int> indices;

  unsigned int baseCenterIndex = 0;
  unsigned int topCenterIndex = 1;
  unsigned int i = topCenterIndex + 1;

  // bottom base
  for(int j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(baseCenterIndex);
      indices.push_back(i);
      indices.push_back(i + 1);
    } else {
      // last triangle
      indices.push_back(i);
      indices.push_back(baseCenterIndex + 2);
      indices.push_back(baseCenterIndex);
    }
  }

  // top base
  unsigned int firstIndexTop = i;
  for (int j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(i+1);
      indices.push_back(i);
      indices.push_back(topCenterIndex);
    } else {
      // last triangle
      indices.push_back(firstIndexTop);
      indices.push_back(i);
      indices.push_back(topCenterIndex);
    }
  }

  // side indices
  unsigned int sideBotStart = i;
  unsigned int sideTopStart = sideBotStart + sectorCount;
  for (int k = 0;k < sectorCount; ++k) {
    unsigned int b1 = sideBotStart + k;
    unsigned int b2 = sideBotStart + ((k+1) % sectorCount);
    unsigned int t1 = sideTopStart + k;
    unsigned int t2 = sideTopStart + ((k+1) % sectorCount);

    indices.push_back(t1);
    indices.push_back(b2);
    indices.push_back(b1);

    indices.push_back(t1);
    indices.push_back(t2);
    indices.push_back(b2);
  }

  return indices;
}


std::vector<float> createConeVertices(int sectorCount, int stackCount, float baseRadius, float height) {
  std::vector<float> vertices;
  std::vector<float> unitVertices = generateUnitCircleCoordinates(sectorCount);

  float baseX = 0.0f, baseY = -height / 2.0f, baseZ = 0.0f;
  vertices.push_back(baseX);
  vertices.push_back(baseY);
  vertices.push_back(baseZ);

  // rings of the cone -- base and sides where base is the 0th ring and the sides are 1st rings to (stackCount - 1)th ring
  for (int i = 0;i < stackCount; ++i) {
    float y = (-height / 2.0f) + ((float)i / stackCount) * height;
    // as height increases, radius decreases to form the cone shape
    float radius = baseRadius * (1.0f - ((float)i / stackCount)); 

    for (int j = 0, k = 0; j < sectorCount; ++j, k += 3) {
      float x = unitVertices[k] * radius;
      float z = unitVertices[k+2] * radius;
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    }
  }

  // tip
  float tipX = 0.0f, tipY = height / 2.0f, tipZ = 0.0f;
  vertices.push_back(tipX);
  vertices.push_back(tipY);
  vertices.push_back(tipZ);

  return vertices;
}

std::vector<unsigned int> createConeIndices(int sectorCount, int stackCount) {
  std::vector<unsigned int> indices;
  
  unsigned int baseCenterIndex = 0;
  // stackCount * sectorCount is the total number of vertices that form all rings around
  // + 1 is the last vertex representing the cone's tip vertex position
  unsigned int tipCenterIndex = (stackCount * sectorCount) + 1;

  // base
  for(int i = 0; i < sectorCount; ++i) {
    unsigned int v1 = i + 1;
    unsigned int v2 = ((i+1) % sectorCount) + 1;

    indices.push_back(baseCenterIndex);
    indices.push_back(v1);
    indices.push_back(v2);
  }

  // sides
  for (int i = 0; i < stackCount - 1; ++i) {
    unsigned int beginStackIndex = (i * sectorCount) + 1;
    unsigned int nextStackIndex = ((i+1) * sectorCount) + 1;
    
    for (int j = 0; j < sectorCount; ++j) {
      unsigned int nextOffset = (j + 1) % sectorCount;

      unsigned int k1 = beginStackIndex + j;
      unsigned int k1Next = beginStackIndex + nextOffset;
      unsigned int k2 = nextStackIndex + j;
      unsigned int k2Next = nextStackIndex + nextOffset;

      indices.push_back(k1);
      indices.push_back(k2);
      indices.push_back(k1Next);

      indices.push_back(k2);
      indices.push_back(k2Next);
      indices.push_back(k1Next);
    }
  }

  // last stack to tip
  unsigned int lastStackIndex = ((stackCount - 1) * sectorCount) + 1;
  for (int i = 0;i < sectorCount; ++i) {
    unsigned int l1 = lastStackIndex + i;
    unsigned int l2 = lastStackIndex + ((i+1) % sectorCount);
    indices.push_back(tipCenterIndex);
    indices.push_back(l2);
    indices.push_back(l1);
  }

  return indices;
}

std::vector<float> createCircleVertices(int sectorCount, float radius, float zDepth) {
    std::vector<float> vertices;
    std::vector<float> unitVertices = generateUnitCircleCoordinates(sectorCount);

    vertices.push_back(0.0f);   // X
    vertices.push_back(zDepth);   // Y
    vertices.push_back(0.0f); // Z

    // 2. Perimeter vertices (remap unit X, Z -> mesh X, Y)
    for (size_t i = 0; i < unitVertices.size(); i += 3) {
        float unitX = unitVertices[i];     // X coordinate
        float unitZ = unitVertices[i + 2]; // Z coordinate from unit generator

        vertices.push_back(radius * unitX);
        vertices.push_back(zDepth); 
        vertices.push_back(radius * unitZ);         
    }

    return vertices;
}

std::vector<unsigned int> createCircleIndices(int sectorCount) {
  std::vector<unsigned int> indices;

  unsigned int baseCenterIndex = 0;
  unsigned int i = 1;

  for(unsigned int j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(baseCenterIndex);
      indices.push_back(i);
      indices.push_back(i + 1);
    } else {
      // last triangle
      indices.push_back(i);
      indices.push_back(baseCenterIndex + 1);
      indices.push_back(baseCenterIndex);
    }
  }

  
  return indices;
}