out vec4 FragColor;
  
uniform vec3 objectColor;
// gets sent from the game engine via OpenGL uniform function helpers
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 FragPos; // world position of a vertex received from the vertex shader
in vec3 Normal;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // calculate direction vector between light source and fragment's position
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // lambert lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);

    // example of ambient lighting
    // all it does it reduces its light's strength by multiplying it by some percentage
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * lightColor;

    // vec3 result = ambient * objectColor;
    // FragColor = vec4(result, 1.0);
}