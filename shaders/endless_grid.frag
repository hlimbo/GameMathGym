layout (location = 0) out vec4 FragColor;

in vec3 QuadWorldPos;

const float gridSize = 100.0;

// in world coordinate space
uniform float gridCellSize = 0.025;
uniform vec3 camWorldPos;
uniform float gridMinPixelsBetweenCells = 2.0;
uniform vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 gridColorThick = vec4(0.75, 0.75, 0.75, 1.0);

const float lineDerivativeThickness = 4.0;

float log10(float x)
{
  // By default, log function in glsl has the base of natural log e ~ 2.71
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/log.xhtml
  // To convert it to base 10 we use the change of base log formula:
  //  https://www.purplemath.com/modules/logrules5.htm
  float f = log(x) / log(10.0);
  return f;
}

void main()
{
  // To understand derivatives in glsl, see: https://www.aclockworkberry.com/shader-derivative-functions/
  vec2 dvx = vec2(dFdx(QuadWorldPos.x), dFdy(QuadWorldPos.x));
  vec2 dvy = vec2(dFdx(QuadWorldPos.z), dFdy(QuadWorldPos.z));
  
  // represents all of the derivatives for a given world position fragment for the xz plane where dvx contains partial derivatives x and y  for worldPos.x and dvy contains partial derivatives x and y for worldPos.z
  vec2 dudv = vec2(length(dvx), length(dvy));
  // magnitude of the derivatives
  float l = length(dudv);

  // Log Base 10 has a nice property where the grid cells grow by a factor of 10 the further the grid cell is from the perspective camera
  // Taking the log and multiplying each GridCellSizeLod influences how many cells you see inside each subgrid. In this case you will see 10x10 inside 1 subgrid.
  float LOD = max(0.0, log10(l * gridMinPixelsBetweenCells / gridCellSize) + 1.0);
  
  // setup multiple Level of detail variables to create a cleaner looking grid
  float GridCellSizeLod0 = gridCellSize * pow(10.0, floor(LOD));
  float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
  float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

  dudv *= lineDerivativeThickness;

  vec2 mod_div_dudv = mod(QuadWorldPos.xz, GridCellSizeLod0) / dudv;
  float Lod0a = max(
    1.0 - abs(clamp(mod_div_dudv.x, 0.0, 1.0) * 2.0 - 1.0),
    1.0 - abs(clamp(mod_div_dudv.y, 0.0, 1.0) * 2.0 - 1.0)
  );

  mod_div_dudv = mod(QuadWorldPos.xz, GridCellSizeLod1) / dudv;
  float Lod1a = max(
    1.0 - abs(clamp(mod_div_dudv.x, 0.0, 1.0) * 2.0 - 1.0),
    1.0 - abs(clamp(mod_div_dudv.y, 0.0, 1.0) * 2.0 - 1.0)
  );

  mod_div_dudv = mod(QuadWorldPos.xz, GridCellSizeLod2) / dudv;
  float Lod2a = max(
    1.0 - abs(clamp(mod_div_dudv.x, 0.0, 1.0) * 2.0 - 1.0),
    1.0 - abs(clamp(mod_div_dudv.y, 0.0, 1.0) * 2.0 - 1.0)
  );

  float LOD_fade = fract(LOD);

  // Equivalent to doing if Lod2a > 0, then else if Lod2a > 0, then else default statement
  // Why avoid if else branching? because on GPUs it processes math equations faster than branching
  float isLod2a = (1.0 - step(Lod2a, 0.0));
  float isLod1a = (1.0 - step(Lod1a, 0.0));
  float isLod0a = (1.0 - step(Lod0a, 0.0));
  
  float isLod1aExclusive = (1.0 - isLod2a) * isLod1a;
  float isLod0aExclusive = (1.0 - isLod2a) * (1.0 - isLod1a) * isLod0a;

  float redChannel = 
    isLod2a * gridColorThick.r +
    isLod1aExclusive * mix(gridColorThick.r, gridColorThin.r, LOD_fade) +
    isLod0aExclusive * gridColorThin.r;

  float greenChannel = 
    isLod2a * gridColorThick.g +
    isLod1aExclusive * mix(gridColorThick.g, gridColorThin.g, LOD_fade) +
    isLod0aExclusive * gridColorThin.g;

  float blueChannel =
    isLod2a * gridColorThick.b +
    isLod1aExclusive * mix(gridColorThick.b, gridColorThin.b, LOD_fade) +
    isLod0aExclusive * gridColorThin.b;

  // LOD_fade is inverted to prevent circular pattern which reveals where the next LOD starts and ends
  float alphaChannel =
    isLod2a * Lod2a +
    isLod1aExclusive * Lod1a +
    isLod0aExclusive * (Lod0a * (1.0 - LOD_fade));

  vec4 Color = vec4(
    redChannel,
    greenChannel,
    blueChannel,
    alphaChannel
  );

  // The further away the grid lines are in a perspective camera, the lower the alpha value gets. This gives us a fading out effect of the grid the further away it is from the camera
  float OpacityFalloff = (1.0 - clamp(length(QuadWorldPos.xz - camWorldPos.xz) / gridSize, 0.0, 1.0));
  Color.a *= OpacityFalloff;

  FragColor = Color;
}