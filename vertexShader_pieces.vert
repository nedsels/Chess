#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexture;

out vec2 textureCoords;

uniform vec2 offset;

void main() {
  textureCoords = aTexture;

  gl_Position = vec4(aPos.xy + offset, aPos.z, 1.0f);
}