#version 150

in vec2 tex;
in float bril;
out vec4 c;

uniform sampler2D tex0;

void main()
{
  // compute the final pixel color
  // students need to implement this
  c=texture(tex0,tex.xy)*bril;
}

