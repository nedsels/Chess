#ifndef GAME_H
#define GAME_H

#include "grid.h"

struct PromotionPrompt
{
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

class Game
{
public:
	Game();

	void draw();
	void update();

private:
	// Setup / update
	Position mousePos;
	Rectangle background;
	Grid grid;
	std::unordered_map<Position, std::shared_ptr<Piece>> board;
	char turn;
	bool gameOver;
	int numPieces;
	int fiftyMoveCounter;

	void createStartingBoard();
	void newPiece(char color, std::string type, Position pos);
	void nextTurn();
	void movePiece(std::shared_ptr<Piece> piece, Position posTo);
	void prepToUpdatePieces();
	void updatePieces();
	void drawPieces();
	void deletePiece(std::shared_ptr<Piece> piece);
	void detectGameOver();

	// Highlight squares
	std::shared_ptr<Rectangle> randHighlightSquare;
	std::shared_ptr<Rectangle> selectedSquare;
	std::shared_ptr<Piece> selectedPiece;
	std::vector<Rectangle> canMoveSquares;

	void updateSelectedSquare();
	void updateRandHighlightSquare();
	void updateCanMoveSquares();

	// Handling king in check
	std::unordered_map<char, Position*> kingPositions;
	std::unordered_map<char, std::set<Position>> visibleSquares;
	std::unordered_map<char, bool> kingInCheck;
	std::unordered_map<char, std::set<Position>> checkActionSquares;
	int piecesCheckingKing;

	// Promotion
	std::shared_ptr<PromotionPrompt> promoPrompt;
	Position promoPos;
	int promoSelection;

	void initiatePromo(Position pos);
	void promote();

	// En passant
	std::shared_ptr<Piece> enPassantPawn;

	// Utility
	bool noValidMove(char color);
	bool isValidMove(Position pos);
	bool isValidPromo(glm::vec2 mouseCoords);
	int getPromoSelection(glm::vec2 mouseCoords);
};

#endif