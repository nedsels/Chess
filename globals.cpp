#include "globals.h"

// consts
const unsigned int SCR_WIDTH = 768;
const unsigned int SCR_HEIGHT = 768;

const glm::vec3 darkGray = glm::vec3(33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);
const glm::vec3 brown = glm::vec3(209.0f / 255.0f, 139.0f / 255.0f, 71.0f / 255.0f);
const glm::vec3 brown_grayscale = glm::vec3(152.0f / 255.0f);
const glm::vec3 lightBrown = glm::vec3(255.0f / 255.0f, 206.0f / 255.0f, 158.0f / 255.0f);
const glm::vec3 lightBrown_grayscale = glm::vec3(215.0f / 255.0f);
const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);

// modifiables
float border;
float squareSize;

float mouseX;
float mouseY;
bool leftClick;
bool rightClick;
bool clicking;

// functions
std::string combineChars(char a, char b) {
	std::string output;

	output.push_back(a);
	output.push_back(b);

	return output;
}

int charToInt(char letter) { return letter - '0'; }

Position add(Position pos1, int colChange, int rowChange) {
	return Position((char)(pos1.col + colChange), pos1.row + rowChange);
}