#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION // this ensures the code can compile and successfully link -- without this, it will generate LNK2019 errors for stbi_load and stbi_image_free functions
#include <stb_image.h>


#include <iostream>

#include "camera/camera.h"
#include "utils/shader_utils.h"
#include "math_utils/vector3.h"
#include "math_utils/matrix4.h"

/* Shapes */
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/cone.h"
#include "shapes/sphere.h"

const char* WINDOW_NAME = "Lighting Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

/* vertices for the cube */
// contains vertex positions in NDC and normal vectors for lighting
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
/* end vertices for the cube */

const bool* keyStates = nullptr;

const std::string vertShaderSrc("shaders/1.colors.vert");
const std::string fragShaderSrc("shaders/1.colors.frag");
const std::string vertShaderSrc2("shaders/1.light_cube.vert");
const std::string fragShaderSrc2("shaders/1.light_cube.frag");
const std::string vertShaderSrc3("shaders/outline_shader.vert");
const std::string fragShaderSrc3("shaders/outline_shader.frag");

GLuint cubeShaderProgramId;
GLuint lightShaderProgramId;
GLuint outlineShaderProgramId;

GLuint cubeVAO, lightCubeVAO;
GLuint VBO;

/* Shapes */
Shapes::Cube* cube = nullptr;
Shapes::Cylinder* cylinder = nullptr;
Shapes::Cone* cone = nullptr;
Shapes::Sphere* sphere = nullptr;
/* End Shapes*/

GLuint toonRampTex;

GLuint loadTexture(const char* path);

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
   
    ShaderUtils::VerifyShaderCompilationStatus(vertShaderId, vertShaderSrc);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc);

    lightShaderProgramId = ShaderUtils::CreateShaderProgram(vertShaderId, fragShaderId);
    ShaderUtils::VerifyShaderProgramLinkStatus(lightShaderProgramId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    vertShaderId = ShaderUtils::LoadAndCreateShaderSource(vertShaderSrc3, GL_VERTEX_SHADER);
    fragShaderId = ShaderUtils::LoadAndCreateShaderSource(fragShaderSrc3, GL_FRAGMENT_SHADER);
   
    ShaderUtils::VerifyShaderCompilationStatus(vertShaderId, vertShaderSrc);
    ShaderUtils::VerifyShaderCompilationStatus(fragShaderId, fragShaderSrc);

    outlineShaderProgramId = ShaderUtils::CreateShaderProgram(vertShaderId, fragShaderId);
    ShaderUtils::VerifyShaderProgramLinkStatus(outlineShaderProgramId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
  }

  /* Vertex Buffers Initialization */
  {

    //cube = new Shapes::Cube();
    uint32_t sectorCount = 16;
    float radius = 1.0f;
    float height = 1.0f;
    // cylinder = new Shapes::Cylinder(sectorCount, radius, radius, height);
    // cone = new Shapes::Cone(sectorCount, sectorCount, radius, height);
    sphere = new Shapes::Sphere(sectorCount, sectorCount, radius);


    // setup light's VAO (VBO stays the same, the vertices are the same for light object which is also a 3D cube)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // for this cube, we reuse its vertex positioning but don't use its normals at all for the lighting...
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  }

  toonRampTex = loadTexture("textures/ToonRamp.png");

  /* Setup Keyboard Controls */
  keyStates = SDL_GetKeyboardState(NULL);

  // Draw vertices that are facing in front of camera and vertices behind other vertices are culled away
  glEnable(GL_DEPTH_TEST);

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

MathUtils::Vector3 cubePos(0.0f, 0.0f, 0.0f);
MathUtils::Vector3 lightPos(1.2f, 1.0f, 2.0f);
// angle relative to the cube -- y position is locked in place
float startingAngleRadians = std::atan2(lightPos.z, lightPos.x);
float angleRadians = startingAngleRadians;
float cubeToLightRadius = (lightPos - cubePos).magnitude();

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

  // Logic to rotate light source around cube where y position remains constant
  MathUtils::Vector3 newLightPos(
    std::cos(angleRadians) * cubeToLightRadius,
    lightPos.y,
    std::sin(angleRadians) * cubeToLightRadius
  );

  lightPos = newLightPos;
  angleRadians += deltaTime;

  // render background solid color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glUseProgram(cubeShaderProgramId);

  // Bind texture to default texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, toonRampTex);
  glUniform1i(glGetUniformLocation(cubeShaderProgramId, "toonRamp"), 0);

  GLuint lightColorLoc = glGetUniformLocation(cubeShaderProgramId, "lightColor");
  glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
  GLuint lightPosLoc = glGetUniformLocation(cubeShaderProgramId, "lightPos");
  glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
  // camera world position
  GLuint viewPosLoc = glGetUniformLocation(cubeShaderProgramId, "viewPos");
  glUniform3f(viewPosLoc, mainCamPosition.x, mainCamPosition.y, mainCamPosition.z);

  // view/projection transformations
  auto projection = mainCamera.getProjectionMatrix();
  // camera view matrix
  auto view = mainCamera.getViewMatrix();
  GLuint projLoc = glGetUniformLocation(cubeShaderProgramId, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.cells);
  GLuint viewLoc = glGetUniformLocation(cubeShaderProgramId, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.cells);
  // world transformations
  MathUtils::Matrix4 model(MathUtils::MAT4_IDENTITY);
  GLuint modelLoc = glGetUniformLocation(cubeShaderProgramId, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.cells);

  /* Render Shapes */
  {
    if (cube != nullptr) {
      cube->Draw();
    }

    if (cylinder != nullptr) {
      cylinder->Draw();
    }

    if (cone != nullptr) {
      cone->Draw();
    }

    if (sphere != nullptr) {
      sphere->Draw();
    }
  }

  /* 2nd Render Pass to add Outline to shape via inverted hull method */

  // draw back of hull only
  glCullFace(GL_FRONT);
  // use invertedHull shader program
  glUseProgram(outlineShaderProgramId);
  // set projection, view, and model matrices to this shader program
  projLoc = glGetUniformLocation(outlineShaderProgramId, "projection");
  viewLoc = glGetUniformLocation(outlineShaderProgramId, "view");
  modelLoc = glGetUniformLocation(outlineShaderProgramId, "model");

  glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.cells);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.cells);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.cells);
  
  // Draw same mesh again using the outline shader
  {
    if (cube != nullptr) {
      cube->Draw();
    }

    if (cylinder != nullptr) {
      cylinder->Draw();
    }

    if (cone != nullptr) {
      cone->Draw();
    }

    if (sphere != nullptr) {
      sphere->Draw();
    }
  }

  // restore to culling the back faces
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);


  // draw lamp object
  glUseProgram(lightShaderProgramId);
  // set projection view matrix
  projLoc = glGetUniformLocation(lightShaderProgramId, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.cells);
  viewLoc = glGetUniformLocation(lightShaderProgramId, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.cells);
  // set translation matrix
  // set scale matrix
  // send translation scale matrix to model uniform
  model = MathUtils::makeTranslationMatrix(lightPos);
  model = model * MathUtils::makeScaleMatrix(0.2f);
  modelLoc = glGetUniformLocation(lightShaderProgramId, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.cells);

  glBindVertexArray(lightCubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  SDL_GL_SwapWindow(win);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  delete cube;
  delete cylinder;
  delete cone;
  delete sphere;
  
  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}

GLuint loadTexture(const char* path) {
  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  int width, height, colorChannelsCount;
  // keep image right side up rather than being upside down
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path, &width, &height, &colorChannelsCount, 0);

  if (data) {
    GLenum format = (colorChannelsCount == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Clamp to edge to prevent texture wrapping artifacts at extreme angles
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // set to gl nearest so that the color sampling remains crisp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  stbi_image_free(data);
  data = nullptr;

  return textureId;
}