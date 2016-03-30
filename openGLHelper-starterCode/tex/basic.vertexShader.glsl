#version 150

in vec3 position;
out vec2 tex;
out float bril;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
  // compute the transformed and projected vertex position (into gl_Position) 
  // compute the vertex color (into col)
  // students need to implement this
  tex=vec2(position.x/64.0,position.y/64.0);
  bril=position.z/64.0;
  gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.0);
}

