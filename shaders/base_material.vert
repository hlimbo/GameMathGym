out vec3 FragPos;
out vec3 Normal;
out vec4 Color;
out vec2 TextureCoord;

// this location is locked in when you do the following in a C++ file
/*
  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)0);

  The following lines turn on location = 0 in the GPU and specify what kind of data this vertex shader will consume
  The GPU now contains the vertex position that was read from a file (e.g. 3d model from assimp)
*/
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTextureCoord;

void main() {
  FragPos = aPos;
  Normal = aNorm;
  Color = aColor;
  TextureCoord = aTextureCoord;
  gl_Position = vec4(aPos, 1.0);
}