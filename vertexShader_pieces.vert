#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexture;

out vec2 textureCoords;

uniform vec2 offset;

void main() {
  textureCoords = aTexture;

  gl_Position = vec4(aPos + offset, 1.0f, 1.0f);
}