out vec4 FragColor;

// gets sent from the game engine via OpenGL uniform function helpers
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos; // world position of a vertex received from the vertex shader
in vec3 Normal;
// color coming in from vertex shader
in vec4 VertexLighting;
in vec4 VertexColor;

void ambientLighting() 
{
    // all it does it reduces its light's strength by multiplying it by some percentage
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * VertexColor.rgb;
    FragColor = vec4(result, 1.0);
}

// Diffuse Lighting
void lambertLighting() 
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // calculate direction vector between light source and fragment's position
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // take the max(dot) product here because each color channel ranges between 0 to 1
    // whereas dot product of 2 unit vectors range between -1 to 1
    // 0 represents absence of light and 1 represents full presence of light
    // if max(dot) results in 0 that means no light is provided
    // if max(dot) > 0 then there is some light being applied to the mesh
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * VertexColor.rgb;
    FragColor = vec4(result, 1.0);
}

// Phong Lighting Model
void specularLighting() 
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // calculate direction vector between light source and fragment's position
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    // 1st arg exppects the vector to point from the light source towards the fragment position which is why it's negated
    vec3 reflectDir = reflect(-lightDir, norm);

    // specular lighting calculation
    int shininess = 32; // shininess of the highlight - the higher the number the more it reflects the light instead of scattering it all around and makes the highlight smaller
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // take the max(dot) product here because each color channel ranges between 0 to 1
    // whereas dot product of 2 unit vectors range between -1 to 1
    // 0 represents absence of light and 1 represents full presence of light
    // if max(dot) results in 0 that means no light is provided
    // if max(dot) > 0 then there is some light being applied to the mesh
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse + specular) * VertexColor.rgb;
    FragColor = vec4(result, 1.0);
}

void main()
{
    specularLighting();
    // Goraud Shading
    // FragColor = vec4(VertexLighting.rgb * VertexColor.rgb, 1.0);
}