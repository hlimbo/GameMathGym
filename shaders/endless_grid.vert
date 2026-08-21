const vec3 quadVertices[4] = vec3[4](
  vec3(-1.0, 1.0, 0.0),     // top-left
  vec3(1.0, 1.0, 0.0),      // top-right
  vec3(-1.0, -1.0, 0.0),    // bottom-left
  vec3(1.0, -1.0, 0.0)      // bottom-right
);

const uint indices[6] = uint[6](
  2, 3, 1,
  1, 0, 2
);

void main()
{
  // gl_VertexID - is a vertex language input variable that holds an integer index for the vertex.
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/gl_VertexID.xhtml
  vec3 vertex = quadVertices[indices[gl_VertexID]];
  gl_Position = vec4(vertex, 1.0);
}