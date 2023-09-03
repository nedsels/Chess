#pragma once

#ifndef POSITION_H
#define POSITION_H

#include <string>

class Position {
 public:
  Position();
  Position(char col, int row);
  Position(char col, char row);
  Position(std::string pos);

  char col;
  int row;

  bool isValid();

 private:
  bool validPos(char col, int row);
  bool validPos(std::string pos);
};

#endif
