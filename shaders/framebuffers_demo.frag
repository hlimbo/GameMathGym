out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

// A Quad in the demos/framebuffers_demo.cpp file is in front of camera and will copy the view the camera is rendering onto its framebuffer instead of the default framebuffer.
void CopyColors() {
  FragColor = texture(screenTexture, TexCoords);
}

/* Examples of how to set post processing effects using an FBO */

void Inversion() {
  FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}

void Grayscale() {
  FragColor = texture(screenTexture, TexCoords);
  float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
  FragColor = vec4(average, average, average, 1.0);
}


// Can sample color values from other parts of the texture not specific to that fragment
// A kernel or convolution matrix is a small matrix array of values centered on current pixel that multiplies surrounding pixel values by its kernel values and adds them all together to form single value
// A kernel of sum 1 means the resulting texture color is the same color as the original texture value
// In this example it will use a 3x3 kernel for post-processing
const float offset = 1.0 / 300.0;
void SharpKernel()
{
  vec2 offsets[9] = vec2[](
      vec2(-offset,  offset), // top-left
      vec2( 0.0f,    offset), // top-center
      vec2( offset,  offset), // top-right
      vec2(-offset,  0.0f),   // center-left
      vec2( 0.0f,    0.0f),   // center-center
      vec2( offset,  0.0f),   // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f,   -offset), // bottom-center
      vec2( offset, -offset)  // bottom-right    
  );

  float kernel[9] = float[](
      -1, -1, -1,
      -1,  9, -1,
      -1, -1, -1
  );
  
  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
      sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
  }
  vec3 col = vec3(0.0);
  for(int i = 0; i < 9; i++)
      col += sampleTex[i] * kernel[i];
  
  FragColor = vec4(col, 1.0);
}

void BlurKernel()
{
  vec2 offsets[9] = vec2[](
      vec2(-offset,  offset), // top-left
      vec2( 0.0f,    offset), // top-center
      vec2( offset,  offset), // top-right
      vec2(-offset,  0.0f),   // center-left
      vec2( 0.0f,    0.0f),   // center-center
      vec2( offset,  0.0f),   // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f,   -offset), // bottom-center
      vec2( offset, -offset)  // bottom-right    
  );

  float kernel[9] = float[](
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
    2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
  );
  
  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
      sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
  }
  vec3 col = vec3(0.0);
  for(int i = 0; i < 9; i++)
    col += sampleTex[i] * kernel[i];
  
  FragColor = vec4(col, 1.0);
}

// This outlines tne edges of an image
void EdgeDetectionKernel() 
{
  vec2 offsets[9] = vec2[](
      vec2(-offset,  offset), // top-left
      vec2( 0.0f,    offset), // top-center
      vec2( offset,  offset), // top-right
      vec2(-offset,  0.0f),   // center-left
      vec2( 0.0f,    0.0f),   // center-center
      vec2( offset,  0.0f),   // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f,   -offset), // bottom-center
      vec2( offset, -offset)  // bottom-right    
  );

  float kernel[9] = float[](
    1.0, 1.0, 1.0,
    1.0, -8.0, 1.0,
    1.0, 1.0, 1.0
  );
  
  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
      sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
  }

  float hasOutline = 0.0;
  vec3 col = vec3(0.0);
  for(int i = 0; i < 9; i++) {
    hasOutline += (sampleTex[i].r + sampleTex[i].g + sampleTex[i].b) * kernel[i];
    col += sampleTex[i] * kernel[i];
  }

  vec3 outlineColor = vec3(hasOutline, hasOutline, hasOutline);
  int centerIndex = 4;
  vec3 texSample = sampleTex[centerIndex];
  FragColor = vec4(hasOutline + texSample, 1.0);
}

void main()
{ 
  EdgeDetectionKernel();
  // FragColor = texture(screenTexture, TexCoords);
}