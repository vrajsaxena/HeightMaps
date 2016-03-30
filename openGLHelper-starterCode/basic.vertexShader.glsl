#version 150

in vec3 position;
out vec4 col;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
  // compute the transformed and projected vertex position (into gl_Position) 
  // compute the vertex color (into col)
  // students need to implement this
  col=vec4(position.z/32.0,position.z/32.0,1.0,1.0);
  gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.0);
}

