#ifndef PIECES_H
#define PIECES_H

#include "globals.h"
#include "grid.h"

class Piece {
public:
    Piece();
    Piece(char color, std::string type, Position position);
    Piece(char color, std::string type, bool promotionOption, glm::vec2 offset);

    bool promotionOption;
    glm::vec2 manualOffset;
    Position position;
    bool hasMoved;
    std::string type;
    char color;
    int value;
    bool pinned;
    std::unordered_map<Position, Piece*>* board;
    std::unordered_map<Position, bool> validMoves;
    std::unordered_map<char, std::set<Position>>* visibleSquares;
    bool* kingInCheck;
    std::unordered_map<char, std::set<Position>>* checkActionSquares;
    std::unordered_map<char, Position*>* kingPositions;
    Piece** enPassantPawn;
    int* piecesCheckingKing;

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
};

#endif