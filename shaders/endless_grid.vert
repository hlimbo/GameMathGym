const float gridSize = 100.0;

uniform vec3 camWorldPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 QuadWorldPos;

/* Quad Geometry Data */
const vec3 quadVertices[4] = vec3[4](
  vec3(-1.0, 0.0, 1.0),     // top-left
  vec3(1.0, 0.0, 1.0),      // top-right
  vec3(-1.0, 0.0, -1.0),    // bottom-left
  vec3(1.0, 0.0, -1.0)      // bottom-right
);

const uint indices[6] = uint[6](
  1, 3, 2,
  2, 0, 1
);
/* End Quad Geometry Data */

void main()
{
  // gl_VertexID - is a vertex language input variable that holds an integer index for the vertex.
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/gl_VertexID.xhtml
  vec3 vertex = quadVertices[indices[gl_VertexID]];

  vertex = vertex * gridSize;
  // this ensures the quad remains relative to the camera's world position at all times
  vertex.x += camWorldPos.x;
  vertex.z += camWorldPos.z;

  // convert from world space to clip space
  vec4 clipSpaceCoord = projection * view * vec4(vertex, 1.0);
  gl_Position = clipSpaceCoord;

  // send world position to fragment shader
  QuadWorldPos = vertex;
}