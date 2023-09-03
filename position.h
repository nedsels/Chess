#pragma once

#ifndef POSITION_H
#define POSITION_H

class Position {
 public:
  Position();
  Position(char col, int row);
  Position(char col, char row);

  char col;
  int row;

  bool isValid();

 private:
  bool validPos(char col, int row);
};

#endif
