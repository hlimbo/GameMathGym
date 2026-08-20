layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sceneColor;
uniform sampler2D sceneNormal;
uniform sampler2D sceneDepth;

// Tomorrow: figure out how to write the Sobel Filter Post Process Algorithm for line edge detection
void main()
{
  FragColor = texture(sceneColor, TexCoord);
}