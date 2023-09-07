#ifndef GAME_H
#define GAME_H

#include "grid.h"

struct PromotionPrompt {
    PromotionPrompt();
    PromotionPrompt(Position promotionSquare);

    float distance;
    float backgroundBorder;
    glm::vec2 offset;
    Rectangle background;
    Rectangle squares[4];
    Piece pieces[4];

    void draw();
};

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
    std::unordered_map<char, std::set<Position>> visibleSquares;
    std::unordered_map<char, bool> kingInCheck;
    std::unordered_map<char, std::set<Position>> checkActionSquares;
    int piecesCheckingKing;
    std::unordered_map<char, Position*> kingPositions;
    Piece* enPassantPawn;
    PromotionPrompt* promoPrompt;
    Position promoPos;
    bool promotion;
    int promoSelection;

    void nextTurn();
    void updateMousePos();
    void updateSelectedSquare();
    void updateRandHighlightSquare();
    void updateCanMoveSquares();
    void updatePieces();
    void drawPieces();
    void movePiece(Piece* piece, Position posTo);
    bool noValidMove(char color);
    bool isValidMove(Position pos);
    bool isValidPromo(glm::vec2 mouseCoords);
    int getPromoSelection(glm::vec2 mouseCoords);
};

#endif