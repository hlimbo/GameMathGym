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

#include "camera/camera.h"
#include "math_utils/matrix4.h"
#include "math_utils/vector3.h"

#include "shapes/cube.h"
#include "utils/shader_utils.h"

/* 
  This code sample shows how to do Object Outlining through
  Stencil Testing:

  Steps:
  1. enable stencil writing via glEnable(GL_STENCIL_TEST);
  2. Set the stencil op to GL_ALWAYS before drawing to the outlined objects, updating the stencil buffer with 1s wherever the objects' fragments are rendered
  3. render the objects
  4. disable stencil writing and depth testing
  5. scale each of the objects by a small amount
  6. Use a different fragment shader that outputs a single border color
  7. Draw objects again, but only if their fragments' stencil values are not equal to 1.
  8. Enable depth testing again and restor stencil func to GL_KEEP

*/

const char* WINDOW_NAME = "Stencils Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

const bool* keyStates = nullptr;

GLuint cubeShaderProgramId;
GLuint stencilShaderId;
const std::string vertShaderSrc("shaders/cube.vert");
const std::string fragShaderSrc("shaders/cube.frag");
const std::string stencilShaderSrc("shaders/stencil.frag");
Shapes::Cube* cube = nullptr;
MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -1.0f)));

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
    GLuint vertShaderId = ShaderUtils::LoadAndCreateShaderSource(vertShaderSrc, GL_VERTEX_SHADER);
    GLuint fragShaderId = ShaderUtils::LoadAndCreateShaderSource(fragShaderSrc, GL_FRAGMENT_SHADER);

    ShaderUtils::VerifyShaderCompilationStatus(vertShaderId, vertShaderSrc);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc);

    cubeShaderProgramId = ShaderUtils::CreateShaderProgram(vertShaderId, fragShaderId);

    ShaderUtils::VerifyShaderProgramLinkStatus(cubeShaderProgramId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    vertShaderId = ShaderUtils::LoadAndCreateShaderSource(vertShaderSrc, GL_VERTEX_SHADER);
    fragShaderId = ShaderUtils::LoadAndCreateShaderSource(stencilShaderSrc, GL_FRAGMENT_SHADER);

    ShaderUtils::VerifyShaderCompilationStatus(vertShaderId, vertShaderSrc);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc);

    stencilShaderId = ShaderUtils::CreateShaderProgram(vertShaderId, fragShaderId);

    ShaderUtils::VerifyShaderProgramLinkStatus(cubeShaderProgramId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
  }

  cube = new Shapes::Cube();

  /* obtain keyboard inputs from SDL */
  keyStates = SDL_GetKeyboardState(NULL);

  // Draw vertices that are facing in front of camera and vertices behind other vertices are culled away
  glEnable(GL_DEPTH_TEST);
  // discards triangles facing away from camera
  glEnable(GL_CULL_FACE);

  /* Enable Stencil Test */
  /*
    Stencil Buffer is a rectangle of 1s initialized where:
    - 1 means render that part of the image in screen space
    - 0 means do not render that part of the image in screen space

    - glStencilMask(0xFF); // each bit is written to stencil buffer as is
    - glStencilMask(0x00); // each bit ends up as 0 in stencil buffer disabling writes

    What does glStencilFunc do?

    glStencilFunc(GLenum func, GLint ref, GLuint mask)
    - func - sets stencil test function that determines whether a fragment passes or is discarded. This test is applied to the stored stencil value and the glSTencilFunc's ref value. Possible options are:
      - GL_NEVER
      - GL_LESS
      - GL_LEQUAL
      - GL_GREATER
      - GL_GEQUAL
      - GL_EQUAL
      - GL_NOTEQUAL
      - GL_ALWAYS
    - ref specifies reference value for stencil test. The stencil buffer's content is compared to this value
    - mask specifies a mask that is ANDed with both the reference value and the stored stencil value before the test compares them. Initially set to all 1s.

    For example a simple stencil would be:
    * glStencilFunc(GL_EQUAL, 1, 0xFF);
        - this says that whatever the stencil value of the fragment is equal to 1 will be passed and drawn; otherwise discard it
    * glStencilFunc describes whether OpenGL should pass or discard fragments based on stencil buffer's content.

    glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
    - sfail - action to take if stencil test fails
    - dpfail - action to take if stencil test passes but depth test fails
    - dppass - action to take if both stencil and depth test pass

    By default glStencilOp does glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

  */
  glEnable(GL_STENCIL_TEST);
  // - keep values when stencil test fails
  // - keep values when depth test fails
  // - replace stencil values when depth test and stencil test pass
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    std::cout << "quitting app" << std::endl;
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}


const float ASPECT = (float)WIDTH / (float)HEIGHT;
const float FOV_DEGREES = 90.0f;
const float NEAR = 0.1f;
const float FAR = 1000.0f;
Core::Camera mainCamera((float)WIDTH, (float)HEIGHT, FOV_DEGREES, NEAR, FAR);

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
  SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(&mX, &mY);
  bool isLeftMouseClicked = mouseFlags & SDL_BUTTON_LMASK;
  if (isLeftMouseClicked) {

    if (isFirstMouse) {
      lastMX = mX;
      lastMY = mY;
      isFirstMouse = false;
    }

    float xOffset = mX - lastMX;
    float yOffset = mY - lastMY;
    
    lastMX = mX;
    lastMY = mY;

    yaw += xOffset * mouseSensitivity;
    pitch += yOffset * mouseSensitivity;

    // make sure when pitch is out of bounds, screen doesn't flip
    if (pitch > 89.0f) {
      pitch = 89.0f;
    }
    if (pitch < -89.0f) {
      pitch = -89.0f;
    }
    
    // reorient which way camera front is facing based on pitch and yaw
    float deg2Rad = (MathUtils::PI / 180.0f);
    MathUtils::Vector3 front(
      std::cos(deg2Rad * yaw) * std::cos(deg2Rad * pitch),
      std::sin(deg2Rad * pitch),
      std::sin(deg2Rad * yaw) * std::cos(deg2Rad * pitch)
    );
    front.normalize();
    camFront = front;
  } else {
    isFirstMouse = true;
  }

  // cross product is used here to ensure when the camera's orientation changes moving laterally remains consistent
  dirInputs = MathUtils::Vector3(0.0f, 0.0f, 0.0f);
  if (keyStates[SDL_SCANCODE_A]) {
    dirInputs -= camFront.cross(camUp);
  } else if (keyStates[SDL_SCANCODE_D]) {
    dirInputs += camFront.cross(camUp);
  }

  if (keyStates[SDL_SCANCODE_W]) {
    dirInputs += camFront;
  } else if (keyStates[SDL_SCANCODE_S]) {
    dirInputs -= camFront;
  }

  if (dirInputs.sqrMagnitude() != 0.0f) {
    dirInputs.normalize();
  }

  MathUtils::Vector3 velocity(dirInputs * speed * deltaTime);
  mainCamPosition += velocity;

  MathUtils::Matrix4 newViewMat(MathUtils::lookAt(mainCamPosition, mainCamPosition + camFront, camUp));

  newViewMat = newViewMat * MathUtils::makeTranslationMatrix(velocity);
  mainCamera.setViewMatrix(newViewMat);


  // render background solid color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // need to clear the stencil buffer for each time the render loop updates
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glUseProgram(cubeShaderProgramId);

  // all fragments should pass the stencil test
  // 0xFF = 255
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  // enable writing to stencil buffer
  glStencilMask(0xFF);

  /* 1st render pass, draw objects as normal and write to stencil buffer */
  auto viewMat = mainCamera.getViewMatrix();
  auto projMat = mainCamera.getProjectionMatrix();

  GLint modelLoc = glGetUniformLocation(cubeShaderProgramId, "model");
  GLint viewLoc = glGetUniformLocation(cubeShaderProgramId, "view");
  GLint projLoc = glGetUniformLocation(cubeShaderProgramId, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.cells);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMat.cells);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, projMat.cells);

  if (cube != nullptr) {
    cube->Draw();
  }

  /* 2nd render pass */
  // draw the upscaled cube and disable stencil writing
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  // disable writing to stencil buffer
  glStencilMask(0x00);
  glDisable(GL_DEPTH_TEST);

  // upscale the cube
  MathUtils::Matrix4 upscaleModelMat(modelMat * MathUtils::makeScaleMatrix(1.1f));

  // use the stencil frag shader
  glUseProgram(stencilShaderId);
  modelLoc = glGetUniformLocation(stencilShaderId, "model");
  viewLoc = glGetUniformLocation(stencilShaderId, "view");
  projLoc = glGetUniformLocation(stencilShaderId, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, upscaleModelMat.cells);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMat.cells);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, projMat.cells);
  if (cube != nullptr) {
    cube->Draw();
  }

  // disable stencil mask
  glStencilMask(0xFF);
  // always draw the stencil
  glStencilFunc(GL_ALWAYS, 1, 0XFF);
  // re-enable depth testing for the next frame
  glEnable(GL_DEPTH_TEST);


  SDL_GL_SwapWindow(win);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  delete cube;
  
  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}