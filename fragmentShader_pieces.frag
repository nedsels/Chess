#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D texture1;

void main() {
  vec4 piece = texture(texture1, textureCoords);	
  if (piece.a < 0.1f) {
    discard;
  }

  FragColor = piece;
}