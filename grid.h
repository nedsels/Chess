#pragma once

#ifndef GRID_H
#define GRID_H

#include "pieces.h"
#include "shapes.h"

class Grid {
 public:
  Grid();

  Rectangle squares[64];

  void draw(std::unordered_map<Position, Piece*> board);
};

#endif