#ifndef SHAPES_H
#define SHAPES_H

#include "globals.h"

class Rectangle
{
public:
	Rectangle();
	Rectangle(float x, float y, float width, float height, glm::vec3 color);
	Rectangle(float x, float y, float width, float height, glm::vec3 color,
		float transparency);
	Rectangle(float x, float y, glm::vec3 color);
	Rectangle(float x, float y, glm::vec3 color, float transparency);
	Rectangle(Position pos, glm::vec3 color, float transparency);

	glm::vec2 offset;
	float width;
	float height;
	glm::vec3 color;
	float transparency;

	void draw();

private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	Shader shader;

	void setup();
};

#endif