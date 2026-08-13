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

#include "shapes/circle.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/line.h"
#include "shapes/quad.h"
#include "shapes/sphere.h"

const char* WINDOW_NAME = "Shapes Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -4.0f)));

const float ASPECT = (float)WIDTH / (float)HEIGHT;
const float FOV_DEGREES = 90.0f;
const float NEAR = 0.1f;
const float FAR = 1000.0f;
Core::Camera mainCamera((float)WIDTH, (float)HEIGHT, FOV_DEGREES, NEAR, FAR);

/* Start Shapes */

int sectorCount = 32;
float radius = 1.0f;
float height = 4.0f;

// Temp - use pointers for now so that they can be later initialized in the correct order
Shapes::Cube* cube = nullptr;
Shapes::Quad* quad = nullptr;
Shapes::Line* line = nullptr;
Shapes::Sphere* sphere = nullptr;
Shapes::Circle* circle = nullptr;
Shapes::Cone* cone = nullptr;
Shapes::Cylinder* cylinder = nullptr;

/* End Shapes */

GLuint shaderProgramId;

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
  
  /* Shapes Initialization */
  {
    // cube = new Shapes::Cube();
    //quad = new Shapes::Quad();
    // line = new Shapes::Line();
    sphere = new Shapes::Sphere(8, 8, 1.0f);
    // circle = new Shapes::Circle(8, 4.0f);
    //cylinder = new Shapes::Cylinder(8, 1.0f, 1.0f, 4.0f);
    // cone = new Shapes::Cone(32, 32, 1.0f, 4.0f);
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

  /* Setup Keyboard Controls */
  keyStates = SDL_GetKeyboardState(NULL);

  // Draw vertices that are facing in front of camera and vertices behind other vertices are culled away
  glEnable(GL_DEPTH_TEST);
  // discards triangles facing away from camera
  glEnable(GL_CULL_FACE);
  // Enable Wireframe mode for both front and back faces
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  std::cout << "sizeof vector3 " << sizeof(MathUtils::Vector3) << std::endl;


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
float yaw = -90.0f, pitch = 0.0f;
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

  if (cube != nullptr) {
    cube->Draw();
  }

  if (quad != nullptr) {
    quad->Draw();
  }

  if (line != nullptr) {
    line->Draw();
  }

  if (sphere != nullptr) {
    sphere->Draw();
  }

  if (circle != nullptr) {
    circle->Draw();
  }

  if (cylinder != nullptr) {
    cylinder->Draw();
  }

  if (cone != nullptr) {
    cone->Draw();
  }

  SDL_GL_SwapWindow(win);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  delete cube;
  delete quad;
  delete line;
  delete sphere;
  delete circle;
  delete cylinder;
  delete cone;

  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}