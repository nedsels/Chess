#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
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
extern const glm::vec3 lightBrown;
extern const glm::vec3 blue;
extern const glm::vec3 yellow;

// modifiables
extern float border;
extern float squareSize;

extern float mouseX;
extern float mouseY;
extern bool leftClick;
extern bool rightClick;
extern bool clicking;

// functions
Position strToPos(std::string posStr);
std::string posToStr(Position pos);
std::string combineChars(char a, char b);

#endif