layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos; // world position used to compute lighting in fragment shader later on
out vec3 Normal;
out vec4 VertexLighting;
out vec4 VertexColor;

// Gouraud shading -- as the shading is applied on vertices instead of fragments
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
    float shininess = 128.0; // shininess of the highlight - the higher the number the more it reflects the light instead of scattering it all around and makes the highlight smaller
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // take the max(dot) product here because each color channel ranges between 0 to 1
    // whereas dot product of 2 unit vectors range between -1 to 1
    // 0 represents absence of light and 1 represents full presence of light
    // if max(dot) results in 0 that means no light is provided
    // if max(dot) > 0 then there is some light being applied to the mesh
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse + specular);
    VertexLighting = vec4(result, 1.0);
}

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));

	// You take the transpose of the inverse of the upper-left part of the 3x3 model matrix so that when you scale an object non-uniformly (e.g. you scale a cube's x by 10 and y by 2.5 and z by 3 for example) the lighting model isn't distorted. This is called the normal matrix. This ensures the normal we pass to the fragment shader remains perpendicular to the surface.
	// To learn more go through: http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
	// Supposedly, doing inverse matrix operations in shader code is inefficient as it has to do it for every single vertex in the scene
	// A better way could be to pre-compute the transpose of the inverse and send it over to the GPU to use later on. Compute NormalMatrix once then reuse the same normal matrix
	// for all vertices on the mesh in the vertex shader
	mat3 NormalMatrix = mat3(transpose(inverse(model)));
	Normal = NormalMatrix * aNormal;
    VertexColor = aColor;

    specularLighting();

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}