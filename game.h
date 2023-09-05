#pragma once

#ifndef GAME_H
#define GAME_H

#include "grid.h"

class Game {
 public:
  Game();

  void draw();
  void update();

  ~Game();

 private:
  Rectangle background;
  Grid grid;
  std::unordered_map<Position, Piece*> board;
  Position mousePos;
  Rectangle* randHighlightSquare;
  Rectangle* selectedSquare;
  Piece* selectedPiece;
  std::vector<Rectangle> canMoveSquares;
  std::unordered_map<char, std::unordered_map<Position, bool>>
      controlledSquares;

  char turn;
  void nextTurn();
  void updateMousePos();
  void updateSelectedSquare();
  void updateRandHighlightSquare();
  void updateCanMoveSquares();
  void updatePieces();
  void movePiece(Piece* piece, Position posTo);
  bool isPieceAt(char col, char row);
  bool isPieceAt(char col, int row);
  bool isPieceAt(Position pos);
  Piece* getPieceAt(char col, char row);
  Piece* getPieceAt(char col, int row);
  Piece* getPieceAt(Position pos);
  bool isValidMove(Position pos);
};

#endif