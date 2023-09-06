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
    char turn;
    bool gameOver;
    Position mousePos;
    Rectangle* randHighlightSquare;
    Rectangle* selectedSquare;
    Piece* selectedPiece;
    std::vector<Rectangle> canMoveSquares;
    std::unordered_map<char, std::unordered_map<Position, bool>>
        controlledSquares;
    std::unordered_map<char, bool> kingInCheck;
    std::unordered_map<char, std::unordered_map<Position, int>> kingAttackSquares;
    int maxAttack;
    std::unordered_map<char, Position*> kingPositions;
    Piece* enPassantPawn;

    void nextTurn();
    void updateMousePos();
    void updateSelectedSquare();
    void updateRandHighlightSquare();
    void updateCanMoveSquares();
    void updatePieces();
    void drawPieces();
    void movePiece(Piece* piece, Position posTo);
    bool noValidMove(char color);
    bool isPieceAt(char col, char row);
    bool isPieceAt(char col, int row);
    bool isPieceAt(Position pos);
    Piece* getPieceAt(char col, char row);
    Piece* getPieceAt(char col, int row);
    Piece* getPieceAt(Position pos);
    bool isValidMove(Position pos);
};

#endif