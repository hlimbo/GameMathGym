out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
  // invert the y-axis
  vec2 invertedCoord = vec2(TexCoord.x, 1.0f - TexCoord.y);
  FragColor = texture(ourTexture, invertedCoord);
}