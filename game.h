#pragma once

#ifndef GAME_H
#define GAME_H

#include "grid.h"

class Game {
 public:
  Game();

  Rectangle background;
  Grid grid;
  std::unordered_map<std::string, Piece*> board;

  void draw();
  void update();

  ~Game();

 private:
  Position mousePos;
  Rectangle* randHighlightSquare;
  Rectangle* selectedSquare;
  std::string selectedSquarePosition;
  std::vector<Rectangle*> canMoveSquares;
  std::vector<std::string> canMovePositions;

  void updateMousePos();
  void updateHighlightSquares();
  void updateCanMoveSquares();
  void updatePieces();
  void sendBoardToPieces();
  bool isPieceAt(char col, char row);
  bool isPieceAt(char col, int row);
  bool isPieceAt(std::string pos);
  bool isPieceAt(Position pos);
  Piece* getPieceAt(char col, char row);
  Piece* getPieceAt(char col, int row);
  Piece* getPieceAt(std::string position);
  Piece* getPieceAt(Position position);
};

#endif