#include "globals.h"

// consts
const unsigned int SCR_WIDTH = 768;
const unsigned int SCR_HEIGHT = 768;

const glm::vec3 darkGray =
    glm::vec3(33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);
const glm::vec3 brown =
    glm::vec3(209.0f / 255.0f, 139.0f / 255.0f, 71.0f / 255.0f);
const glm::vec3 lightBrown =
    glm::vec3(255.0f / 255.0f, 206.0f / 255.0f, 158.0f / 255.0f);
const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);

// modifiables
float border;
float squareSize;

float mouseX;
float mouseY;
bool leftClick;
bool rightClick;

// functions
Position strToPos(std::string posStr) {
  Position position;

  position.col = posStr[0];
  position.row = posStr[1] - '0';

  return position;
}

std::string posToStr(Position pos) {
  std::string position;

  position.push_back(pos.col);
  position.push_back('0' + pos.row);

  return position;
}

std::string combineChars(char a, char b) {
  std::string output;

  output.push_back(a);
  output.push_back(b);

  return output;
}