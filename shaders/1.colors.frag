out vec4 FragColor;

// gets sent from the game engine via OpenGL uniform function helpers
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

// PNG texture for toonRamp
uniform sampler2D toonRamp;

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

// https://developer.valvesoftware.com/wiki/Half_Lambert
// My Preferred Lighting Model looks nice!
void halfLambertLighting()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = pow(dot(norm, lightDir) * 0.5 + 0.5, 2);
    vec3 diffuse = diff * lightColor;
    vec3 result = diffuse * VertexColor.rgb;
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

// https://www.jordanstevenstechart.com/lighting-models
void bandedLighting()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    // float light = max(0.0, dot(norm, lightDir));
    float light = pow(dot(norm, lightDir) * 0.5 + 0.5, 2);

    // the closer lightSteps is to 256, the less light the object gets
    // the further away lightSteps is from 256, the more light the object receives
    float lightSteps = 64.0;
    float lightBandsMultiplier = lightSteps / 256;
    float lightBandsAdditive = lightSteps / 2;

    float bandedLight = (floor((light*256+lightBandsAdditive)/lightSteps)) * lightBandsMultiplier;

    vec3 litModel = bandedLight * VertexColor.rgb;
    vec3 finalColor = litModel * lightColor;
    FragColor = vec4(finalColor, 1.0);
}

void bandedShading()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    // float NdotL = max(dot(norm, lightDir), 0.0);
    float NdotL = pow(dot(norm, lightDir) * 0.5 + 0.5, 2);

    // manual stepping of light intensity
    // float intensity;

    // if (NdotL > 0.85) {
    //     intensity = 1.0;
    // } else if (NdotL > 0.4) {
    //     intensity = 0.6;
    // } else if (NdotL > 0.1) {
    //     intensity = 0.3;
    // } else {
    //     intensity = 0.1;
    // }

    // code equivalent to the above except I set the lowest default intensity to 0.25
    float numSteps = 3.0;
    float intensity = max(floor(NdotL * numSteps) / numSteps, 0.25);

    vec3 bandLighting = VertexColor.rgb * lightColor * intensity;
    FragColor = vec4(bandLighting, 1.0);
}

void ToonRampStyle()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float NdotL = 0.5 * dot(norm, lightDir) + 0.5;

    vec2 rampUV = vec2(NdotL, 0.5);
    vec3 rampColor = texture(toonRamp, rampUV).rgb;

    // specular lighting calculation
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    // 1st arg expects the vector to point from the light source towards the fragment position which is why it's negated
    vec3 reflectDir = reflect(-lightDir, norm);

    int shininess = 32; // shininess of the highlight - the higher the number the more it reflects the light instead of scattering it all around and makes the highlight smaller
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // color blending
    vec3 finalColor = VertexColor.rgb * rampColor + specular;
    FragColor = vec4(finalColor, 1.0);
}

void main()
{
    // specularLighting();
    // Goraud Shading
    // FragColor = vec4(VertexLighting.rgb * VertexColor.rgb, 1.0);
    // halfLambertLighting();
    // lambertLighting();
    //bandedLighting();
    // bandedShading();
    ToonRampStyle();
}