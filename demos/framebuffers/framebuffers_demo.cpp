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
#include "utils/texture_utils.h"

const char* WINDOW_NAME = "Framebuffers Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

const bool* keyStates = nullptr;

GLuint cubeShaderProgramId;
const std::string vertShaderSrc("shaders/cube.vert");
const std::string fragShaderSrc("shaders/cube.frag");
const std::string vertShaderSrc2("shaders/framebuffers_demo.vert");
const std::string fragShaderSrc2("shaders/framebuffers_demo.frag");
Shapes::Cube* cube = nullptr;
MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -1.0f)));

/* Framebuffer Variables */
GLuint fbo;
GLuint texture;
GLuint depthStencilTexture;
GLuint fboShaderProgramId;
GLuint quadVAO, quadVBO;
GLuint quadTexture;

// Contains Positions and texCoords
// Positions are in 2D NDC coordinates
// texCoords are the UV coordinates ranging between 0 to 1 for both the U and V axes.
float quadVertices[] = {
  -1.0f, 1.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f,
  
  -1.0f, 1.0f, 0.0f, 1.0f,
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
};

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

    vertShaderId = ShaderUtils::LoadAndCreateShaderSource(vertShaderSrc2, GL_VERTEX_SHADER);
    fragShaderId = ShaderUtils::LoadAndCreateShaderSource(fragShaderSrc2, GL_FRAGMENT_SHADER);

    ShaderUtils::VerifyShaderCompilationStatus(vertShaderId, vertShaderSrc2);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc2);

    fboShaderProgramId = ShaderUtils::CreateShaderProgram(vertShaderId, fragShaderId);
    ShaderUtils::VerifyShaderProgramLinkStatus(fboShaderProgramId);

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

  // draw as wireframe -- uncomment this line to verify its rendering the fbo on top of the camera
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  /* Load Quad Texture */
  {
    quadTexture = TextureUtils::loadTexture("textures/container.jpg");
  }

  /* Create Quad Buffers */
  {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    GLsizei strideSize = 4 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideSize, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideSize, (void*)(2 * sizeof(float)));
  }

  // set texture to shader
  glUseProgram(fboShaderProgramId);
  glUniform1i(glGetUniformLocation(fboShaderProgramId, "screenTexture"), 0);


  /* Creating a framebuffer */
  {
    glGenFramebuffers(1, &fbo);
    // this binds read and write operations to fbo - rendering logic is done offscreen
    // fbo ID = 0 is the default framebuffer that OpenGL comes with when you create an opengl context for the first time.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    /* 
      For a framebuffer to be complete it needs:
      1. Attach at least one buffer (could be color, depth, or stencil buffer)
      2. Have at least one color attachment
      3. All attachments should be complete
      4. Each buffer should have same number of samples
    */

    // Doing a Texture Attachment to FBO
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // allocates information about the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // attach texture to fbo
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Do a Depth and Stencil Attachment to FBO
    // 24 bits represent depth information and 8 bits represent stencil information
    glGenTextures(1, &depthStencilTexture);
    glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0,
      GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture, 0);

    // verify if framebuffer is complete
    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "framebuffer not complete!" << std::hex << framebufferStatus << "\n";
    }

    // bind back to the main framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


  /* First Render Pass */

  // bind framebuffer and draw scene to the fbo
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glUseProgram(cubeShaderProgramId);

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

  /* Second Pass */

  // bind back to the main framebuffer
  // and draw a quad plane with the attached framebuffer color texture
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // disable depth test so screen space quad isn't discarded due to depth test
  glDisable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(fboShaderProgramId);
  glBindVertexArray(quadVAO);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);


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