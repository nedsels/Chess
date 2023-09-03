#include "shapes.h"

Rectangle::Rectangle() {
  x = 0.0f;
  y = 0.0f;
  width = 0.0f;
  height = 0.0f;
  color = glm::vec3(0.0f);
  transparency = 0.0f;

  shader = Shader("vertexShader_shapes.vert", "fragmentShader_shapes.frag");
}

Rectangle::Rectangle(float x, float y, float width, float height,
                     glm::vec3 color) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  this->color = color;
  transparency = 0.0f;

  shader = Shader("vertexShader_shapes.vert", "fragmentShader_shapes.frag");

  setup();
}

Rectangle::Rectangle(float x, float y, float width, float height,
                     glm::vec3 color, float transparency) {
  this->x = x;
  this->y = y;
  this->width = squareSize;
  this->height = squareSize;
  this->color = color;
  this->transparency = transparency;

  shader = Shader("vertexShader_shapes.vert", "fragmentShader_shapes.frag");

  setup();
}

Rectangle::Rectangle(float x, float y, glm::vec3 color) {
  this->x = x;
  this->y = y;
  this->width = squareSize;
  this->height = squareSize;
  this->color = color;
  transparency = 0.0f;

  shader = Shader("vertexShader_shapes.vert", "fragmentShader_shapes.frag");

  setup();
}

Rectangle::Rectangle(float x, float y, glm::vec3 color, float transparency) {
  this->x = x;
  this->y = y;
  this->width = squareSize;
  this->height = squareSize;
  this->color = color;
  this->transparency = transparency;

  shader = Shader("vertexShader_shapes.vert", "fragmentShader_shapes.frag");

  setup();
}

void Rectangle::setup() {
  float vertices[12] = {
      // bottom left
      (2.0f / SCR_WIDTH) * x - 1.0f, (2.0f / 768.0f) * y - 1.0f, 0.0f,
      // top left
      (2.0f / SCR_HEIGHT) * x - 1.0f, (2.0f / 768.0f) * (y + height) - 1.0f,
      0.0f,
      // bottom right
      (2.0f / SCR_WIDTH) * (x + width) - 1.0f, (2.0f / 768.0f) * y - 1.0f, 0.0f,
      // top right
      (2.0f / SCR_WIDTH) * (x + width) - 1.0f,
      (2.0f / SCR_HEIGHT) * (y + height) - 1.0f, 0.0f};

  unsigned int indices[6] = {
      0, 1, 2,  // first triangle
      1, 2, 3   // second triangle
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Rectangle::draw() {
  shader.use();
  shader.set3f("color", color);
  shader.set1f("transparency", transparency);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}