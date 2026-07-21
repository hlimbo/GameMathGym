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

#define STB_IMAGE_IMPLEMENTATION // this ensures the code can compile and successfully link -- without this, it will generate LNK2019 errors for stbi_load and stbi_image_free functions
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <assert.h>

#include "utils/shader_utils.h"

#ifdef __EMSCRIPTEN__
  const char* versionHeader = "#version 300 es\n";
  const char* glslPrecision = "precision mediump float;\n"; // 16-bit floats supported on web
#else
  const char* versionHeader = "#version 330 core\n";
  const char* glslPrecision = "";
#endif

const char* WINDOW_NAME = "3D Model Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

// 3d_models/Raiden/Raiden.obj
// 3d_models/UltimatePlatformerPack-Dec2021/Character/glTF/Character.gltf
const std::string filePath("3d_models/Raiden/Raiden.obj");

// 3D Model Data
std::vector<unsigned int> meshIndices; // VBO
std::vector<unsigned int> vertexIndices; // EBO
/*
  Packed Vertex Data Containing:
  1. Vertex Positions
  2. Normals
  3. Colors RGBA
  4. Texture UV Coords
  5. Texture Layer IDs (for 3D Models that have more than 1 diffuse texture)
*/
std::vector<float> vertexData;
// how many data points a single vertex data source contains
const GLsizei vertexStride = 13;
// contains filepath names of the image textures if the 3d model has any
std::vector<aiString> textureStrs;
std::unordered_map<std::string, int> textureToLayerMap;
GLuint textureArrayId;

GLuint VAO; // Vertex Array Object (required to draw the triangles on screen)
GLuint VBO; // Vertex Buffer Object (contains the vertex data that lives in the GPU)
GLuint EBO; // Element Buffer Object (this is to reuse vertex data that could belong to multiple triangles)

// Shaders
GLuint vertexShader;
GLuint fragShader;
GLuint shaderProgram;
const char* vertShaderSrc = "shaders/base_material.vert";
const char* fragShaderSrc = "shaders/base_material.frag";

// this is used to prevent criss-crossing of the lines rendered for the model
// when merging multiple meshes into 1 VBO
unsigned int vertexOffset = 0;

void CollectMeshIndices(const aiScene* scene);
void CollectVertexData(const aiScene* scene, const std::vector<unsigned int>& meshIndices);
void LoadTextures();
void SetupMesh();
void DestroyMesh();
void DrawModel();
void PrepareMaterial(aiMaterial* material, GLuint shaderProgram);

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

  // Load Shader Program
  std::string vertCodeStr(ShaderUtils::LoadShaderSource(vertShaderSrc));
  const char* vertCodeArr[2] = {
    versionHeader,
    vertCodeStr.c_str()
  };

  std::string fragCodeStr(ShaderUtils::LoadShaderSource(fragShaderSrc));
  const char* fragCodeArr[3] = {
    versionHeader,
    glslPrecision,
    fragCodeStr.c_str()
  };

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 2, vertCodeArr, NULL);
  glCompileShader(vertexShader);
  ShaderUtils::VerifyShaderCompilationStatus(vertexShader, vertShaderSrc);

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 3, fragCodeArr, NULL);
  glCompileShader(fragShader);
  ShaderUtils::VerifyShaderCompilationStatus(fragShader, fragShaderSrc);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragShader);
  glLinkProgram(shaderProgram);
  ShaderUtils::VerifyShaderProgramLinkStatus(shaderProgram);

  // no longer need shader objects
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

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

  // used to debug and render faces as lines instead of filling it in the face with color
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  CollectMeshIndices(scene);
  CollectVertexData(scene, meshIndices);
  LoadTextures();
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

void CollectMeshIndices(const aiScene* scene) {
  unsigned int rootMeshCount = scene->mRootNode->mNumMeshes;
  unsigned int rootChildrenCount = scene->mRootNode->mNumChildren;
  std::cout << "root mesh count: " << rootMeshCount << std::endl;
  std::cout << "root child count: " << rootChildrenCount << std:: endl;

  std::queue<aiNode*> nodes;
  nodes.push(scene->mRootNode);

  /* 
    Collect Mesh Indices using BFS
    Assumption: There is no cyclic dependency for the 3D Model nodes
  */
  while(!nodes.empty()) {
    aiNode* node = nodes.front();
    nodes.pop();

    unsigned int meshCount = node->mNumMeshes;
    unsigned int childrenCount = node->mNumChildren;
    std::cout << "mesh count: " << meshCount << std::endl;
    std::cout << "child count: " << childrenCount << std::endl;
    for (unsigned int i = 0; i < meshCount; ++i) {
      unsigned int meshIndex = node->mMeshes[i];
      meshIndices.push_back(meshIndex);
    }

    for (unsigned int i = 0; i < childrenCount; ++i) {
      aiNode* childNode = node->mChildren[i];
      nodes.push(childNode);
    }
  }
}

void CollectVertexData(const aiScene* scene, const std::vector<unsigned int>& meshIndices) {
  std::vector<aiVector3D> vertices;
  std::vector<aiVector3D> normals;
  std::vector<aiColor4D> colors;
  std::vector<aiVector2D> textureUVCoords;
  // used to determine which vertices get which texture
  std::vector<float> textureLayerIds;

  // Default Color and Material Fallbacks if not found on 3D Model
  aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
  float metallic = 0.0f;
  float roughness = 0.5f;

  // find min and max bounds for AABB so that I can scale the model down
  float highest = std::numeric_limits<float>::max();
  float lowest = std::numeric_limits<float>::lowest();
  aiVector3D minBounds(highest, highest, highest);
  aiVector3D maxBounds(lowest, lowest, lowest);

  for (unsigned int meshIndex : meshIndices) {
    std::cout << "mesh Index: " << meshIndex << std::endl;
    aiMesh* mesh = scene->mMeshes[meshIndex];
    // std::cout << "mesh name: " << mesh->mName.C_Str() << std::endl;
    // std::cout << "mesh vertex count: " << mesh->mNumVertices << std::endl;
    // std::cout << "mesh face count: " << mesh->mNumFaces << std::endl;

    // find min and max bounds so that the model can be scaled 
    // IMPORTANT: need to have the ai_Process_GenBoundingBoxes turned on when loading the model via importer.ReadFile() function
    minBounds.x = std::min(minBounds.x, mesh->mAABB.mMin.x);
    minBounds.y = std::min(minBounds.y, mesh->mAABB.mMin.y);
    minBounds.z = std::min(minBounds.z, mesh->mAABB.mMin.z);
    maxBounds.x = std::max(maxBounds.x, mesh->mAABB.mMax.x);
    maxBounds.y = std::max(maxBounds.y, mesh->mAABB.mMax.y);
    maxBounds.z = std::max(maxBounds.z, mesh->mAABB.mMax.z);

    // Find Texture Data using Assimp and assign
    // textureLayerId to a vertex so that it knows which
    // texture image to use in the 3d model
    unsigned int matIndex = mesh->mMaterialIndex;
    aiMaterial* material = scene->mMaterials[matIndex];
    unsigned int textureDiffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "texture diffuse count: " << textureDiffuseCount << std::endl;

    // Default Layer
    float layerIndex = 0.0f;
    
    // Assumption: that there will only be 1 texture diffuse per 1 mesh
    for (unsigned int i = 0; i < textureDiffuseCount; ++i) {
      aiString textureStr;
      if (material->GetTexture(aiTextureType_DIFFUSE, i, &textureStr) == AI_SUCCESS) {
        std::cout << "texture name: " << textureStr.C_Str() << std:: endl;

        // add unique texture file names here
        std::string textureName(textureStr.C_Str());
        bool isTextureNotFound = textureToLayerMap.find(textureName) == textureToLayerMap.end();
        int layerId = -1;
        if (isTextureNotFound) {
          layerId = textureStrs.size();
          textureToLayerMap[textureName] = layerId;
          textureStrs.push_back(textureStr);
        } else {
          layerId = textureToLayerMap[textureName];
        }

        assert(layerId != -1);
        layerIndex = static_cast<float>(layerId);
      }
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      vertices.push_back(mesh->mVertices[i]);

      if (mesh->HasNormals()) {
        normals.push_back(mesh->mNormals[i]);
      } else {
        // default normals for flat shading
        aiVector3D normal(0.0f, 0.0f, 1.0f);
        normals.push_back(normal);
      }

      // Load In Base Color Material Settings
      unsigned int materialIndex = mesh->mMaterialIndex;
      aiMaterial* material = scene->mMaterials[materialIndex];
      // AI_MATKEY_BASE_COLOR expands to "$clr.base", 0, 0 as its a preprocessor define value
      // AI_MATKEY_COLOR_DIFFUSE is similar which means that aiMaterial::Get accepts 4 arguments rather than 2
      if(material->Get(AI_MATKEY_BASE_COLOR, baseColor) != AI_SUCCESS) {
        material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
      }

      colors.push_back(baseColor);
      textureLayerIds.push_back(layerIndex);

      // Assumption: models will always have their textures set in the 0th index e.g. TEXTURE0 (Only use textures from the 0th index if available)
      if (mesh->mTextureCoords[0]) {
        aiVector2D textureCoords(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        textureUVCoords.push_back(textureCoords);
      } else {
        aiVector2D textureCoords(0.0f, 0.0f);
        textureUVCoords.push_back(textureCoords);
      }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j) {
        // IMPORTANT: need to add vertexOffset here as each mesh face index starts at 0
        // this ensures OpenGL can draw the triangles in the correct order
        vertexIndices.push_back(face.mIndices[j] + vertexOffset);
      }
    }
    
    // Apply offset after process the current mesh's vertices
    // Why? to prevent vertices from being rendered in the incorrect order
    vertexOffset += mesh->mNumVertices;
  }

  // scale the vertices down so that I can view the model within the default screen space coordinates OpenGL uses without implementing a camera
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

  // merge vertex positions, normals, colors, and textureCoords in 1 flat floating point array
  // For now hardcode the color to RED then come back to this to figure out how to extract the color from the materials per mesh
  assert(vertices.size() == colors.size());
  assert(colors.size() == normals.size());
  assert(normals.size() == textureUVCoords.size());
  assert(textureLayerIds.size() == textureUVCoords.size());

  for (unsigned int i = 0; i < vertices.size(); ++i) {
    vertexData.insert(
      vertexData.end(),
      {
        vertices[i].x, vertices[i].y, vertices[i].z,        // position
        normals[i].x, normals[i].y, normals[i].z,           // normal
        colors[i].r, colors[i].g, colors[i].b, colors[i].a, // color
        textureUVCoords[i].x, textureUVCoords[i].y,         // texture coords
        textureLayerIds[i],                                 // texture layer ID
      } 
    );
  }

}

void LoadTextures() {
  std::cout << "texture strings count: " << textureStrs.size() << std::endl;

  int maxTextureWidth = 0;
  int maxTextureHeight = 0;


  std::vector<unsigned char*> imageData;
  std::vector<int> widths;
  std::vector<int> heights;
  std::vector<GLenum> colorFormats;

  std::string textureFilePath(filePath.substr(0, filePath.rfind("/")) + "/");
  for (unsigned int i = 0; i < textureStrs.size(); ++i) {
    int width, height, nrComponents;
    GLenum colorFormat;
    std::string textureFile(textureFilePath + textureStrs[i].C_Str());
    unsigned char* data = stbi_load(textureFile.c_str(), &width, &height, &nrComponents, 3);
    if (data) {
      imageData.push_back(data);
      widths.push_back(width);
      heights.push_back(height);
      maxTextureWidth = std::max(maxTextureWidth, width);
      maxTextureHeight = std::max(maxTextureHeight, height);
      if (nrComponents == 1) {
        colorFormat = GL_RED;
      } else if (nrComponents == 3) {
        colorFormat = GL_RGB;
      } else if (nrComponents == 4) {
        colorFormat = GL_RGBA;
      }
      colorFormats.push_back(colorFormat);
    }
  }

  // allocate and declare storage for 2D texture array on GPU
  glGenTextures(1, &textureArrayId);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);

  // No mipmaps for now
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Allocate memory on GPU -- have contiguous array of data
  // containing textures
  int layerCount = static_cast<int>(imageData.size());
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, maxTextureWidth, maxTextureHeight, layerCount, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  // Load, Resize and Upload each texture slice
  for (int i = 0;i < layerCount; ++i) {
    unsigned char* srcImg = imageData[i];
    
    // Resize image on CPU to fit the uniform array size
    int numChannels = 3;
    std::vector<unsigned char> resizedImg(maxTextureWidth * maxTextureHeight * numChannels);

    // possible image filtering options
    // STBIR_FILTER_MITCHELL -- the one Unity uses be default
    // STBIR_FILTER_CATMULLROM
    stbir_resize(
      srcImg, widths[i], heights[i], 0,
      resizedImg.data(), maxTextureWidth, maxTextureHeight, 0,
      STBIR_RGB,
      STBIR_TYPE_UINT8,
      STBIR_EDGE_CLAMP,
      STBIR_FILTER_POINT_SAMPLE // keeps sharp pixel boundaries
    );

    // upload slice into layer 'i'
    glTexSubImage3D(
      GL_TEXTURE_2D_ARRAY, 0,
      0, 0, i,
      maxTextureWidth, maxTextureHeight, 1,
      GL_RGB, GL_UNSIGNED_BYTE,
      resizedImg.data()
    );

    stbi_image_free(srcImg);
  }

}

void SetupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  // this binds a buffer array to the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Approach 1: Pack all the data containing the vertex position, normals, colors rgba in 1 float array data structure
  // Using 1 VAO/VBO in this case to draw the entire 3D Model is called buffer batching -- this combines all meshes on 3d model into 1 draw call on GPU
  // by storing vertex position, normals, texture coords, colors rgba in a tightly packed 1D float array data structure
  /*
        vertex position       normal (xyz)    color rgba
      [ 0.0,  1.0, 2.0      0.0, 1.0, 0.0   0.5, 0.2, 0.25, 1.0 ...... ]
  */

  assert(vertexData.size() > 0);
  assert(vertexIndices.size() > 0);

  // this allocates data and stores data in the initialized memory (I assume this is on the GPU)
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

  // set vertex attribute pointers -- lets OpenGL know how many variables an aiVector3D have at a time
  // this maps to vertexPosition (location = 0) in the vertex shader layout program
  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride * sizeof(float), (void*)0);

  // this maps vertexNormal (location = 1) in the vertex shader layout program
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStride * sizeof(float), (void*)(sizeof(float) * 3));

  // this maps color rgba (location = 2) in the vertex shader layout program
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexStride * sizeof(float), (void*)(sizeof(float) * 6));

  // this maps texture coords (UVs) (location = 3) in the vertex shader layout program
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexStride * sizeof(float), (void*)(sizeof(float) * 10));

  // this maps textureLayerId (location = 4) in the vertex shader layout program
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, vertexStride * sizeof(float), (void*)(sizeof(float) * 12));

  // this binds buffer array to the EBO (EBO used to determine which vertices to reuse when rendering triangles or faces)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);
  
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
  /* DRAW MESH */

  glUseProgram(shaderProgram);

  // bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
  // set to texture unit 0
  glUniform1i(glGetUniformLocation(shaderProgram, "uDiffuseArray"), 0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(vertexIndices.size()), GL_UNSIGNED_INT, 0);
  //unbind vertex array object
  glBindVertexArray(0);
}

// This can only called on the render loop as I'd need to set the color properties for all the vertices and fragments via shaders
void PrepareMaterial(aiMaterial* material, GLuint shaderProgram) {
  // Default Fallbacks
  aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
  float metallic = 0.0f;
  float roughness = 0.5f;

  // Apply fallback colors if they cannot be obtained from the material directly
  if (material->Get(AI_MATKEY_BASE_COLOR, baseColor) != AI_SUCCESS) {
    material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
  }

  material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
  
  material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

  // Send the color properties to the fragment shader program for the GPU to process
  GLint baseColorLoc = glGetUniformLocation(shaderProgram, "u_BaseColor");
  if (baseColorLoc != -1) {
    glUniform4f(baseColorLoc, baseColor.r, baseColor.g, baseColor.b, baseColor.a);
  }

  // // Set Metallic (float)
  // GLint metallicLoc = glGetUniformLocation(shaderProgram, "u_Metallic");
  // if (metallicLoc != -1) {
  //   glUniform1f(metallicLoc, metallic);
  // }

  // // Set Roughness (float)
  // GLint roughnessLoc = glGetUniformLocation(shaderProgram, "u_Roughness");
  // if (roughnessLoc != -1) {
  //   glUniform1f(roughnessLoc, roughness);
  // }
}