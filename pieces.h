#pragma once

#ifndef PIECES_H
#define PIECES_H

#include "globals.h"

class Piece {
 public:
  Piece();
  Piece(Position position);

  Shader shader;
  std::string type;
  char color;
  int value;
  Position position;
  bool canEnPassant;
  std::unordered_map<Position, Piece*>* board;
  std::unordered_map<Position, bool> validMoves;
  std::unordered_map<char, std::unordered_map<Position, bool>>* controlledSquares;

  void draw();
  void updateValidMoves();
  void invalidateMoveIntoCheck();

 protected:
  int vertices[20];
  int width;
  int height;
  int nrChannels;
  unsigned char* data;
  unsigned int texture;
  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO;

  void setup();
  void updatePawnMoves();
  void updateBishopMoves();
  void updateKnightMoves();
  void updateRookMoves();
  void updateQueenMoves();
  void updateKingMoves();
  bool isPieceAt(char col, char row);
  bool isPieceAt(char col, int row);
  bool isPieceAt(Position pos);
  Piece* getPieceAt(char col, char row);
  Piece* getPieceAt(char col, int row);
  Piece* getPieceAt(Position pos);
};

class white_pawn : public Piece {
 public:
  white_pawn(Position position);
};

class white_bishop : public Piece {
 public:
  white_bishop(Position position);
};

class white_knight : public Piece {
 public:
  white_knight(Position position);
};

class white_rook : public Piece {
 public:
  white_rook(Position position);
};

class white_queen : public Piece {
 public:
  white_queen(Position position);
};

class white_king : public Piece {
 public:
  white_king(Position position);
};

class black_pawn : public Piece {
 public:
  black_pawn(Position position);
};

class black_bishop : public Piece {
 public:
  black_bishop(Position position);
};

class black_knight : public Piece {
 public:
  black_knight(Position position);
};

class black_rook : public Piece {
 public:
  black_rook(Position position);
};

class black_queen : public Piece {
 public:
  black_queen(Position position);
};

class black_king : public Piece {
 public:
  black_king(Position position);
};

#endif