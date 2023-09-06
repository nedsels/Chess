#ifndef PIECES_H
#define PIECES_H

#include "globals.h"

class Piece {
public:
    Piece();
    Piece(char color, std::string type, Position position);

    Position position;
    bool hasMoved;
    std::string type;
    char color;
    int value;
    bool pinned;
    std::unordered_map<Position, Piece*>* board;
    std::unordered_map<Position, bool> validMoves;
    std::unordered_map<char, std::unordered_map<Position, bool>>*
        controlledSquares;
    std::unordered_map<char, bool>* kingInCheck;
    std::unordered_map<char, std::unordered_map<Position, int>>*
        kingAttackSquares;
    std::unordered_map<char, Position*>* kingPositions;
    Piece** enPassantPawn;
    int* maxAttack;

    void draw();
    void updateValidMoves();
    void invalidateMoveIntoCheck();

protected:
    Shader shader;
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

#endif