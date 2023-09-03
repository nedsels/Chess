#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform float transparency;

void main() {
  vec4 colorOut = vec4(color, 1.0f - transparency);
  if (colorOut.a < 0.1f) {
    discard;
  }

  FragColor = colorOut;	
}