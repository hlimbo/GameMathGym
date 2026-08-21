layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sceneColor;
uniform sampler2D sceneNormal;
uniform sampler2D sceneDepth;

// perspective matrix camera parameters
uniform float far;
uniform float near;

uniform vec2 screenDimensions;

// https://www.vertexfragment.com/ramblings/unity-postprocessing-sobel-outline/
vec4 SobelSample(sampler2D tex, vec2 uv, vec3 offset)
{
  vec4 pixelCenter = texture(tex, uv);
  vec4 pixelLeft = texture(tex, uv - offset.xz);
  vec4 pixelRight = texture(tex, uv + offset.xz);
  vec4 pixelUp = texture(tex, uv + offset.zy);
  vec4 pixelDown = texture(tex, uv - offset.zy);

  // Similar to Manhattan Distance Formula: https://www.geeksforgeeks.org/data-science/manhattan-distance/
  vec4 res = abs(pixelLeft - pixelCenter) + abs(pixelRight - pixelCenter) + abs(pixelUp - pixelCenter) + abs(pixelDown - pixelCenter);
  return res;
}

// Assumption -> the camera projection matrix uses a perspective 4x4 matrix
// takes in a depth value that is read from the depth texture buffer which ranges between 0 to 1
float LinearizeDepth(float depth)
{
  // convert to NDC range [-1, to 1]
  float z = depth * 2.0 - 1.0;
  // convert from NDC space to View space
  // can be derived by using the z-depth formulas from the symmteric perspective matrix
  float zView = (2.0 * far * near) / (z * (far - near) - (far + near));
  return zView;
}

// https://www.vertexfragment.com/ramblings/unity-postprocessing-sobel-outline/
float SobelSampleDepth(sampler2D depthTex, vec2 uv, vec3 offset)
{
  // obtain neighboring fragments of center fragment to get the depth samples
  float depth = texture(depthTex, uv).r;
  float pixelCenter = LinearizeDepth(depth);
  float pixelLeft = LinearizeDepth(texture(depthTex, uv - offset.xz).r);
  float pixelRight = LinearizeDepth(texture(depthTex, uv + offset.xz).r);
  float pixelUp = LinearizeDepth(texture(depthTex, uv + offset.zy).r);
  float pixelDown = LinearizeDepth(texture(depthTex, uv - offset.zy).r);

  mat3 depthSamples = mat3(
    0.0,        pixelUp,        0.0,
    pixelLeft,  pixelCenter,    pixelRight,
    0.0,        pixelDown,      0.0
  );

  // https://en.wikipedia.org/wiki/Sobel_operator
  // calculates the approximations of the derivatives...
  // here we only need to sample 4 neighbors + the center (could alternatively try to sample the diagonals to make it 8 directions instead of 4)
  mat3 Gx = mat3(
    -1.0, 0.0, 1.0,
    -2.0, 0.0, 2.0,
    -1.0, 0.0, 1.0
  );
  
  mat3 Gy = mat3(
    -1.0, -2.0, -1.0,
    0.0, 0.0, 0.0,
    1.0, 2.0, 1.0
  );

  mat3 depthGxMat = Gx * depthSamples;
  mat3 depthGyMat = Gy * depthSamples;
  float depthGx = 0.0;
  float depthGy = 0.0;
  for (int i = 0;i < 3; ++i) {
    for (int j = 0;j < 3; ++j) {
      depthGx += depthGxMat[i][j];
      depthGy += depthGyMat[i][j];
    }
  }

  float depthMagnitude = sqrt(depthGx * depthGx + depthGy * depthGy);
  return depthMagnitude;
}

vec3 calcSobelNormal(vec3 color, vec3 offset)
{
  // Parameters to tune
  vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
  // used to increase or decrease outline thickness strength
  float outlineNormalMultiplier = 10.0;
  // used to remove noise artifacts from the outline
  float outlineNormalBias = 4.0;

  /* Uses the normals obtained by rendering a 3d scene onto an fbo and projecting the normal texture onto a quad */
  vec4 sobelNormalVec = SobelSample(sceneNormal, TexCoord, offset);
  float sobelNormal = sobelNormalVec.x + sobelNormalVec.y + sobelNormalVec.z;

  // fine tune the sobel based on normal multiplier and normal bias parameters
  sobelNormal = pow(sobelNormal * outlineNormalMultiplier, outlineNormalBias);

  // modulate outline color based on transparency
  vec3 outlineColorBlend = mix(color, outlineColor.rgb, outlineColor.a);

  vec3 finalColor = mix(color, outlineColorBlend, sobelNormal);
  return finalColor;
}

// it's normal to have sides of the geometry to be completely engulfed in the outlineColor when viewing from specific angles
vec3 calcSobelDepth(vec3 color, vec3 offset)
{
  // Parameters to tune
  // Observation: If I set alpha value to something low, the shadowing effect this shader has becomes more subtle
  vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
  // used to increase or decrease outline thickness strength
  float outlineDepthMultiplier = 4.0;
  // used to remove noise artifacts from the outline
  float outlineDepthBias = 2.0;

  /* Uses the depth values obtained by rendering a 3d scene onto an fbo and projecting the depth values onto a quad */
  float sobelDepth = SobelSampleDepth(sceneDepth, TexCoord, offset);
  // fine tune sobel based on multiplier and bias parameters
  sobelDepth = pow(sobelDepth * outlineDepthMultiplier, outlineDepthBias);

  // modulate outline color based on transparency
  vec3 outlineBlendColor = mix(color, outlineColor.rgb, outlineColor.a);
  vec3 finalColor = mix(color, outlineBlendColor, sobelDepth);

  return finalColor;
}

vec3 calcSobelCombined(vec3 color, vec3 offset)
{
// Parameters to tune
  vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
  // used to increase or decrease outline thickness strength
  float outlineNormalMultiplier = 10.0;
  // used to remove noise artifacts from the outline
  float outlineNormalBias = 4.0;
  // used to increase or decrease outline thickness strength
  float outlineDepthMultiplier = 10.0;
  // used to remove noise artifacts from the outline
  float outlineDepthBias = 4.0;

  /* Uses the normals obtained by rendering a 3d scene onto an fbo and projecting the normal texture onto a quad */
  vec4 sobelNormalVec = SobelSample(sceneNormal, TexCoord, offset);
  float sobelNormal = sobelNormalVec.x + sobelNormalVec.y + sobelNormalVec.z;
  sobelNormal = clamp(sobelNormal, 0.0, 1.0);

  // fine tune the sobel based on normal multiplier and normal bias parameters
  sobelNormal = pow(sobelNormal * outlineNormalMultiplier, outlineNormalBias);

  /* Uses the depth values obtained by rendering a 3d scene onto an fbo and projecting the depth values onto a quad */
  float sobelDepth = SobelSampleDepth(sceneDepth, TexCoord, offset);
  // fine tune sobel based on multiplier and bias parameters
  sobelDepth = pow(sobelDepth * outlineDepthMultiplier, outlineDepthBias);


  outlineColor.a = smoothstep(0.0, 1.0, max(sobelDepth, sobelNormal));
  float sobelCombined = clamp(max(sobelDepth, sobelNormal), 0.0, 1.0);
  
  // modulate outline color based on transparency
  vec3 outlineBlendColor = mix(color, outlineColor.rgb, outlineColor.a);
  vec3 finalColor = mix(color, outlineBlendColor, sobelCombined);
  return finalColor;
}

void main()
{
  float outLineThickness = 0.5;
  vec3 offset = vec3(1.0 / screenDimensions.x, 1.0 / screenDimensions.y, 0.0) * outLineThickness;
  vec3 color = texture(sceneColor, TexCoord).rgb;


  vec3 finalColor = calcSobelNormal(color, offset);
  // vec3 finalColor = calcSobelDepth(color, offset);
  // vec3 finalColor = calcSobelCombined(color, offset);
  FragColor = vec4(finalColor, 1.0);
}