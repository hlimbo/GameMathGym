out vec4 FragColor;

in vec4 Color;

void main()
{
  FragColor = Color;
  // FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}