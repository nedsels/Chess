#pragma once

#ifndef PIECES_H
#define PIECES_H

#include "globals.h"

class Piece {
 public:
  Piece(std::string position);

  Shader shader;
  std::string type;
  char color;
  int value;
  Position position;
  bool canEnPassant;
  std::unordered_map<std::string, Piece*>* board;
  std::vector<std::string> validMoves;

  void draw();
  void updateValidMoves();

 protected:
  int width;
  int height;
  int nrChannels;
  unsigned char *data;
  unsigned int texture;
  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO;

  void setup();
  std::vector<std::string> getPawnMoves(Piece* piece, char color);
  std::vector<std::string> getBishopMoves(Piece* piece, char color);
  std::vector<std::string> getKnightMoves(Piece* piece, char color);
  std::vector<std::string> getRookMoves(Piece* piece, char color);
  std::vector<std::string> getQueenMoves(Piece* piece, char color);
  std::vector<std::string> getKingMoves(Piece* piece, char color);
  bool isPieceAt(char col, char row);
  bool isPieceAt(char col, int row);
  bool isPieceAt(std::string pos);
  bool isPieceAt(Position pos);
  Piece* getPieceAt(char col, char row);
  Piece* getPieceAt(char col, int row);
  Piece* getPieceAt(std::string position);
  Piece* getPieceAt(Position position);
};

class white_pawn : public Piece {
 public:
  white_pawn(std::string position);
};

class white_bishop : public Piece {
 public:
  white_bishop(std::string position);
};

class white_knight : public Piece {
 public:
  white_knight(std::string position);
};

class white_rook : public Piece {
 public:
  white_rook(std::string position);
};

class white_queen : public Piece {
 public:
  white_queen(std::string position);
};

class white_king : public Piece {
 public:
  white_king(std::string position);
};

class black_pawn : public Piece {
 public:
  black_pawn(std::string position);
};

class black_bishop : public Piece {
 public:
  black_bishop(std::string position);
};

class black_knight : public Piece {
 public:
  black_knight(std::string position);
};

class black_rook : public Piece {
 public:
  black_rook(std::string position);
};

class black_queen : public Piece {
 public:
  black_queen(std::string position);
};

class black_king : public Piece {
 public:
  black_king(std::string position);
};

#endif