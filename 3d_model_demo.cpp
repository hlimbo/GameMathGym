#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <iostream>
#include <vector>
#include <limits>

const char* WINDOW_NAME = "3D Model Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

// 3d_models/Raiden/Raiden.obj
// 3d_models/UltimatePlatformerPack-Dec2021/Character/glTF/Character.gltf
const std::string filePath("3d_models/UltimatePlatformerPack-Dec2021/Character/glTF/Character.gltf");

// 3D Model Data
std::vector<unsigned int> meshIndices; // VBO
std::vector<unsigned int> vertexIndices; // EBO
std::vector<aiVector3D> vertices;

GLuint VAO; // Vertex Array Object (required to draw the triangles on screen)
GLuint VBO; // Vertex Buffer Object (contains the vertex data that lives in the GPU)
GLuint EBO; // Element Buffer Object (this is to reuse vertex data that could belong to multiple triangles)

// this is used to prevent criss-crossing of the lines rendered for the model
// when merging multiple meshes into 1 VBO
unsigned int vertexOffset = 0;

void SetupMesh();
void DestroyMesh();
void DrawModel();

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

  // Create Assimp Logger so it can forward error and warning messages to stdout
  Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
  bool isLogStreamAttached = Assimp::DefaultLogger::get()->attachStream(
    Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDOUT),
    Assimp::Logger::ErrorSeverity::Warn | Assimp::Logger::ErrorSeverity::Err
  );

  if (!isLogStreamAttached) {
    std::cout << "Assimp log stream failed to attach STDOUT stream" << std::endl;
  }

  Assimp::Importer importer;
  // aiProcess_Triangulate option tells assimp to have the 3d model use all triangles for rendering if the artist is using quads or n-gons. assimp will do calculations on its end
  // to triangulate the entire model
  // aiProcess_FlipUVs option flips the texture coordinates on y-axis where necessary during processing
  const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes);

  // verify model can be loaded successfully
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return SDL_APP_FAILURE;
  }

  if (scene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING) {
    std::cout << "WARNING::ASSIMP::" << "validation warnings outputted" << std::endl;
  }

  unsigned int rootMeshCount = scene->mRootNode->mNumMeshes;
  unsigned int childrenCount = scene->mRootNode->mNumChildren;
  std::cout << "root mesh count: " << rootMeshCount << std::endl;
  std::cout << "root child count: " << childrenCount << std:: endl;

  for (unsigned int i = 0; i < rootMeshCount; ++i) {
    unsigned int meshIndex = scene->mRootNode->mMeshes[i];
    aiMesh* mesh = scene->mMeshes[meshIndex];
    std::cout << "mesh vertex count: " << mesh->mNumVertices << std::endl;
    std::cout << "mesh face count: " << mesh->mNumFaces << std::endl;
  }

  for (unsigned int i = 0; i < childrenCount; ++i) {
    aiNode* childNode = scene->mRootNode->mChildren[i];
    std::cout << "child node " << i << " mesh count: " << childNode->mNumMeshes << std::endl;

    for (unsigned int j = 0; j < childNode->mNumMeshes; ++j) {
      meshIndices.push_back(childNode->mMeshes[j]);
    }
  }

  // find min and max bounds for AABB so that I can scale the model down
  float highest = std::numeric_limits<float>::max();
  float lowest = std::numeric_limits<float>::lowest();
  aiVector3D minBounds(highest, highest, highest);
  aiVector3D maxBounds(lowest, lowest, lowest);

  for (unsigned int meshIndex : meshIndices) {
    aiMesh* mesh = scene->mMeshes[meshIndex];
    std::cout << "mesh name: " << mesh->mName.C_Str() << std::endl;
    std::cout << "mesh vertex count: " << mesh->mNumVertices << std::endl;
    std::cout << "mesh face count: " << mesh->mNumFaces << std::endl;

    // find min and max bounds so that the model can be scaled 
    // IMPORTANT: need to have the ai_Process_GenBoundingBoxes turned on when loading the model via importer.ReadFile() function
    minBounds.x = std::min(minBounds.x, mesh->mAABB.mMin.x);
    minBounds.y = std::min(minBounds.y, mesh->mAABB.mMin.y);
    minBounds.z = std::min(minBounds.z, mesh->mAABB.mMin.z);
    maxBounds.x = std::max(maxBounds.x, mesh->mAABB.mMax.x);
    maxBounds.y = std::max(maxBounds.y, mesh->mAABB.mMax.y);
    maxBounds.z = std::max(maxBounds.z, mesh->mAABB.mMax.z);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      vertices.push_back(mesh->mVertices[i]);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j) {
        // IMPORTANT: need to add vertexOffset here as each mesh face index starts at 0
        // this ensures the mapping to OpenGL contains all unique indices
        vertexIndices.push_back(face.mIndices[j] + vertexOffset);
      }
    }
    // Apply offset after process the current mesh's vertices
    vertexOffset += mesh->mNumVertices;
  }

  // scale the vertices down
  aiVector3D center = (minBounds + maxBounds) * 0.5f;
  aiVector3D size = maxBounds - minBounds;
  float maxDimension = std::max({size.x, size.y, size.z});
  float scaleFactor = 1.0f / maxDimension; // scale to fit range from -0.5 to 0.5
  for (unsigned int i = 0; i < vertices.size(); ++i) {
    // 1. center vertex around origin
    vertices[i] -= center;
    // 2. scale each vertex down proportionally
    vertices[i] *= scaleFactor;
  }


  // used to debug and render faces as lines instead of filling it in the face with color
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  SetupMesh();

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

  glClearColor(0.1f, 0.25f, 0.25f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  DrawModel();

  SDL_GL_SwapWindow(win);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  DestroyMesh();
  
  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}

void SetupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  // this binds a buffer array to the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // this allocates data and stores data in the initialized memory (I assume this is on the GPU)
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(aiVector3D), &vertices[0], GL_STATIC_DRAW);

  // this binds buffer array to the EBO (EBO used to determine which vertices to reuse when rendering triangles or faces)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

  // set vertex attribute pointers -- lets OpenGL know how many variables an aiVector3D have at a time
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)0);
  //unbind Vertex Array Object -- why? in OpenGL you can only bind one VAO to the GPU at a time
  glBindVertexArray(0);
}

void DestroyMesh() {
  // unbind vertex array
  glBindVertexArray(0);

  // unbind VBO and EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // delete buffers from GPU
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

}

void DrawModel() {
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(vertexIndices.size()), GL_UNSIGNED_INT, 0);
  //unbind vertex array object
  glBindVertexArray(0);
}