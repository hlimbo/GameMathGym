out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 Color;
in vec2 TextureCoord;
in float TextureLayerId;

// uniform sampler2D ourTexture;
// array of diffuse 2D textures
uniform sampler2DArray uDiffuseArray;

void main() {
    //FragColor = Color;
    // FragColor = texture(ourTexture, TextureCoord) * Color;
    vec3 textureSampleCoords = vec3(TextureCoord.x, TextureCoord.y, TextureLayerId);
    FragColor = texture(uDiffuseArray, textureSampleCoords);
}