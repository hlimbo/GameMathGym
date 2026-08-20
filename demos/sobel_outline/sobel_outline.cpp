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

const char* WINDOW_NAME = "Sobel Outline Post-Processing Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

const bool* keyStates = nullptr;

GLuint cubeShaderProgramId;
const std::string vertShaderSrc("shaders/cube.vert");
const std::string fragShaderSrc("shaders/cube.frag");
Shapes::Cube* cube = nullptr;
MathUtils::Matrix4 modelMat(MathUtils::makeTranslationMatrix(MathUtils::Vector3(0.0f, 0.0f, -1.0f)));

/* Framebuffer Object Variables */
GLuint fbo;
GLuint fboShaderProgramId;
GLuint fboColorTexture, fboNormalTexture, fboDepthTexture;
const std::string vertShaderSrc2("shaders/sobel_outline.vert");
const std::string fragShaderSrc2("shaders/sobel_outline.frag");
GLuint quadVAO;
GLuint quadVBO;
GLuint quadEBO;

// position followed by texCoords
// position is a 2d coordinate in Normalized Device Coordinates
// texCoords is 2D in UV coordinates which represents the texture units used to paint the quad
float quadVertices[] = {
  -1.0f, 1.0f, 0.0f, 1.0f,  // top-left
  -1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
  1.0f, -1.0f, 1.0f, 0.0f, // bottom-right
  1.0f, 1.0f, 1.0f, 1.0f, // top-right
};

// counter-clockwise direction so it reamains visible
unsigned int quadIndices[] = {
  3, 0, 1,
  3, 1, 2
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

  /* Create FBO Quad Buffer onto GPU */
  {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);

    GLsizei quadStride = sizeof(float) * 4;
    // location = 0, vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, quadStride, (void*)0);
    // location = 1, uv texture coordinate
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, quadStride, (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
  }

  /* Create Framebuffer */
  {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboColorTexture);
    glGenTextures(1, &fboNormalTexture);
    glGenTextures(1, &fboDepthTexture);

    // color texture attachment
    glBindTexture(GL_TEXTURE_2D, fboColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTexture, 0);

    // normals texture attachment
    glBindTexture(GL_TEXTURE_2D, fboNormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // clamp UV textures to 0 to 1 when they either underflow or overflow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fboNormalTexture, 0);

    // depth texture attachment
    glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // clamp UV textures to 0 to 1 when they either underflow or overflow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture, 0);

    // write to the color and normal texture attachments which would be made available in the fragment shader glsl code
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "framebuffer not complete: " << std::hex << framebufferStatus << "\n";
    }

    // bind back to default framebuffer 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  /* assign texture unit slots for the shader program that will project its color, normal, depth texture info onto a quad */
  {
    glUseProgram(fboShaderProgramId);
    glUniform1i(glGetUniformLocation(fboShaderProgramId, "sceneColor"), 0);
    glUniform1i(glGetUniformLocation(fboShaderProgramId, "sceneNormal"), 1);
    glUniform1i(glGetUniformLocation(fboShaderProgramId, "sceneDepth"), 2);
  }

  /* Send in screen width and height into shader program */
  {
    glUseProgram(fboShaderProgramId);
    glUniform2f(glGetUniformLocation(fboShaderProgramId, "screenDimensions"), (GLfloat)(WIDTH), (GLfloat)(HEIGHT));
  }

  /* obtain keyboard inputs from SDL */
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


  /* Render First Pass onto FBO which renders the scene offscreen */
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // render background solid color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
  }

  /* Render Second Pass which renders the textures created in the FBO projected onto the quad */
  {
    // bind back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // disable depth test to ensure quad doesn't get discarded
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(fboShaderProgramId);

    // Activate Each Texture  for fboShaderProgramId to use in the fragment shader by activating a texture unit slot and binding it to it
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboColorTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboNormalTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fboDepthTexture);

    // draw the quad that will have the color and normal textures projected onto it
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

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