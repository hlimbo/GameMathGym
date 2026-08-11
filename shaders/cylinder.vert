layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  // note -- multiplication is read from right to left!
  gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}