#ifndef GLOBALS_H
#define GLOBALS_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <set>
#include <unordered_map>

#include "correctIncludeOrder.cpp"
#include "position.h"
#include "shaders.h"
#include "stb_image.h"

// consts
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern const glm::vec3 darkGray;
extern const glm::vec3 brown;
extern const glm::vec3 brown_grayscale;
extern const glm::vec3 lightBrown;
extern const glm::vec3 lightBrown_grayscale;
extern const glm::vec3 blue;
extern const glm::vec3 yellow;
extern const glm::vec3 black;

// modifiables
extern float border;
extern float squareSize;

extern float mouseX;
extern float mouseY;
extern bool leftClick;
extern bool rightClick;
extern bool clicking;

// functions
std::string combineChars(char a, char b);
int charToInt(char letter);
Position add(Position pos1, int colChange, int rowChange);
glm::vec2 posToCoord(Position pos);

#endif