layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Uses Inverted Hull Method to create an outline by rendering the same mesh but bigger
void main()
{
  float outlineThickness = 0.05f;

  // vertex position in view space
  vec4 viewPos = view * model * vec4(aPos, 1.0);

	mat3 NormalMatrix = mat3(transpose(inverse(model)));
  vec3 viewNormal = normalize(mat3(view) * NormalMatrix * aNormal);

  // extrude pposition outwards along the normal
  vec3 extrudedPos = viewPos.xyz + (viewNormal * outlineThickness);
  gl_Position = projection * vec4(extrudedPos, 1.0);
}