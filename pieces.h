#ifndef PIECES_H
#define PIECES_H

#include "globals.h"
#include "grid.h"

class Piece
{
public:
	Piece();
	Piece(char color, std::string type, Position position);
	Piece(char color, std::string type, bool forPromo, glm::vec2 offset);

	Position position;
	glm::vec2 manualOffset;
	bool forPromo;
	std::string type;
	char color;
	char otherColor;
	int value;
	int dir;
	bool hasMoved;
	bool pinned;
	std::set<Position> validMoves;

	// Pointers to Game() attributes
	std::unordered_map<Position, std::shared_ptr<Piece>>* board;
	std::unordered_map<char, std::set<Position>>* visibleSquares;
	bool* kingInCheck;
	int* piecesCheckingKing;
	std::unordered_map<char, std::set<Position>>* checkActionSquares;
	std::unordered_map<char, Position*>* kingPositions;
	std::shared_ptr<Piece>* enPassantPawn;

	void setup();
	void draw();
	void process();
	void invalidateMoveIntoCheck();

protected:
	Shader shader;
	int width;
	int height;
	int nrChannels;
	unsigned char* data;
	unsigned int texture;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	void processPawn();
	void processBishop();
	void processKnight();
	void processRook();
	void processQueen();
	void processKing();
};

#endif