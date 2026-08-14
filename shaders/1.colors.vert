layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // world position used to compute lighting in fragment shader later on
out vec3 Normal;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));

	// You take the transpose of the inverse of the upper-left part of the 3x3 model matrix so that when you scale an object non-uniformly (e.g. you scale a cube's x by 10 and y by 2.5 and z by 3 for example) the lighting model isn't distorted. This is called the normal matrix. This ensures the normal we pass to the fragment shader remains perpendicular to the surface.
	// To learn more go through: http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
	// Supposedly, doing inverse matrix operations in shader code is inefficient as it has to do it for every single vertex in the scene
	// A better way could be to pre-compute the transpose of the inverse and send it over to the GPU to use later on. Compute NormalMatrix once then reuse the same normal matrix
	// for all vertices on the mesh in the vertex shader
	mat3 NormalMatrix = mat3(transpose(inverse(model)));
	Normal = NormalMatrix * aNormal;
}