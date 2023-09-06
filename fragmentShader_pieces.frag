#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D pieceTexture;

void main() {
  vec4 piece = texture(pieceTexture, textureCoords);	
  if (piece.a < 0.1f) {
    discard;
  }

  FragColor = piece;
}