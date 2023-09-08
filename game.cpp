#include "game.h"

Game::Game()
{
	gameOver = false;

	leftClick = false;
	rightClick = false;

	grid = Grid();
	background = Rectangle(0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, darkGray);

	turn = 'W';
	randHighlightSquare = nullptr;
	selectedSquare = nullptr;
	selectedPiece = nullptr;
	enPassantPawn = nullptr;
	piecesCheckingKing = 0;
	promoSelection = -1;
	kingInCheck = std::unordered_map<char, bool>();
	canMoveSquares = std::vector<Rectangle>();
	visibleSquares = std::unordered_map<char, std::set<Position>>();
	kingPositions = std::unordered_map<char, Position*>();

	createStartingBoard();

	updatePieces();
}

void Game::createStartingBoard()
{
	// White
	newPiece('W', "rook", (Position)"a1");
	newPiece('W', "knight", (Position)"b1");
	newPiece('W', "bishop", (Position)"c1");
	newPiece('W', "queen", (Position)"d1");
	newPiece('W', "king", (Position)"e1");
	kingPositions['W'] = &(board[(Position)"e1"]->position);
	newPiece('W', "bishop", (Position)"f1");
	newPiece('W', "knight", (Position)"g1");
	newPiece('W', "rook", (Position)"h1");

	// Black
	newPiece('B', "rook", (Position)"a8");
	newPiece('B', "knight", (Position)"b8");
	newPiece('B', "bishop", (Position)"c8");
	newPiece('B', "queen", (Position)"d8");
	newPiece('B', "king", (Position)"e8");
	kingPositions['B'] = &(board[(Position)"e8"]->position);
	newPiece('B', "bishop", (Position)"f8");
	newPiece('B', "knight", (Position)"g8");
	newPiece('B', "rook", (Position)"h8");

	for (char i = 'a'; i <= 'h'; i++)
	{
		newPiece('W', "pawn", Position(i, 2));
		newPiece('B', "pawn", Position(i, 7));
	}
}

void Game::newPiece(char color, std::string type, Position pos)
{
	board[pos] = std::shared_ptr<Piece>(new Piece(color, type, pos));

	board[pos]->board = &board;

	board[pos]->visibleSquares = &visibleSquares;

	board[pos]->kingInCheck = &(kingInCheck[board[pos]->color]);

	board[pos]->checkActionSquares = &checkActionSquares;

	board[pos]->kingPositions = &kingPositions;

	board[pos]->piecesCheckingKing = &piecesCheckingKing;

	board[pos]->enPassantPawn = &enPassantPawn;
}

void Game::updatePieces()
{
	prepToUpdatePieces();

	char otherColor;
	if (turn == 'W')
	{
		otherColor = 'B';
	} else
	{
		otherColor = 'W';
	}

	// Check opposite side first to see if any "turn" pieces are pinned
	for (auto it = board.begin(); it != board.end(); it++)
	{
		if (it->second->color == otherColor)
		{
			it->second->process();
		}
	}

	if (piecesCheckingKing)
	{
		kingInCheck[turn] = true;
	}

	// If more than 1 piece is checking king, king must move
	if (piecesCheckingKing >= 2)
	{
		board[*kingPositions[turn]]->process();
	} else
	{
		for (auto it = board.begin(); it != board.end(); it++)
		{
			if (it->second->color == turn)
			{
				it->second->process();
			}
		}
	}

	// updateValidMoves() does not account for moves into check
	board[*kingPositions[turn]]->invalidateMoveIntoCheck();
	board[*kingPositions[otherColor]]->invalidateMoveIntoCheck();
}

void Game::prepToUpdatePieces()
{
	// Clears/resets certain attributes in the pieces so they can be updated properly
	visibleSquares['W'].clear();
	visibleSquares['B'].clear();

	checkActionSquares['W'].clear();
	checkActionSquares['B'].clear();

	kingInCheck['W'] = false;
	kingInCheck['B'] = false;

	for (auto it = board.begin(); it != board.end(); it++)
	{
		it->second->pinned = false;
		it->second->validMoves.clear();
	}

	piecesCheckingKing = 0;
}

void Game::update()
{
	if (!gameOver)
	{
		mousePos = coordToPos(mouseX, mouseY);
		updateSelectedSquare();
		updateRandHighlightSquare();

		leftClick = false;
		rightClick = false;
	}
}

void Game::updateSelectedSquare()
{
	if (!promoPrompt)
	{
		if (selectedSquare && isValidMove(mousePos) && leftClick)
		{ // If user left clicks on valid move, move piece and unselect
			movePiece(selectedPiece, mousePos);

			selectedSquare = nullptr;

			selectedPiece = nullptr;
		} else if (selectedSquare && rightClick)
		{ // If user right clicks, unselect
			selectedSquare = nullptr;

			selectedPiece = nullptr;
		} else if (board.count(mousePos)
			&& board.at(mousePos)->color == turn && leftClick)
		{ // If user left clicks on valid piece, select it and show valid moves
			selectedPiece = board[mousePos];
			selectedSquare =
				std::shared_ptr<Rectangle>(new Rectangle(mousePos, blue, 0.7f));
		}
	} else if (promoPrompt && selectedSquare)
	{
		selectedSquare = nullptr;

		selectedPiece = nullptr;
	}

	updateCanMoveSquares();
}

void Game::movePiece(std::shared_ptr<Piece> piece, Position posTo)
{
	Position posFrom = piece->position;

	// If piece being taken, erase it first
	if (board.count(posTo))
	{
		deletePiece(board[posTo]);
	}

	// If en passant, erase pawn being taken
	if (enPassantPawn && piece->type == "pawn"
		&& add(posTo, 0, -piece->dir) == enPassantPawn->position)
	{
		deletePiece(enPassantPawn);
		enPassantPawn = nullptr;
	}

	// Transfer piece to new position
	board[posTo] = piece;
	piece->position = posTo;
	board.erase(posFrom);
	piece->hasMoved = true;

	// Pawn promotion
	if (piece->type == "pawn" && (posTo.row == 8 || posTo.row == 1))
	{
		initiatePromo(posTo);

		return;
	}

	// If pawn moves two squares, en passant is valid for next move only
	if (piece->type == "pawn"
		&& (posTo == add(posFrom, 0, 2) || posFrom == add(posTo, 0, 2)))
	{
		enPassantPawn = piece;
	} else
	{
		enPassantPawn = nullptr;
	}

	// Castling
	if (piece->type == "king" && !piece->hasMoved)
	{
		int row = piece->position.row;

		Position longCastle = add(piece->position, -2, 0);
		Position shortCastle = add(piece->position, 2, 0);

		if (posTo == longCastle)
		{
			movePiece(board[Position('a', row)], Position('d', row));
			nextTurn();

		} else if (posTo == shortCastle)
		{
			movePiece(board[Position('h', row)], Position('f', row));
			nextTurn();
		}
	}

	nextTurn();

	updatePieces();

	if (noValidMove(turn))
	{
		std::cout << "GAME OVER" << std::endl;
		gameOver = true;
	}
}

void Game::deletePiece(std::shared_ptr<Piece> piece)
{
	board.erase(piece->position);
}

void Game::initiatePromo(Position pos)
{
	// Create promotion prompt
	promoPrompt = std::shared_ptr<PromotionPrompt>(new PromotionPrompt(pos));
	promoPos = pos;

	// Change all squares but promotion square to gray
	grid.changeColor(brown_grayscale, lightBrown_grayscale);

	int gridIndex = grid.index(pos);
	if ((gridIndex % 2 == 0 && gridIndex / 8 % 2 == 0)
		|| (gridIndex % 2 == 1 && gridIndex / 8 % 2 == 1))
	{
		grid.squares[gridIndex].color = brown;
	} else
	{
		grid.squares[gridIndex].color = lightBrown;
	}
}

void Game::updateCanMoveSquares()
{
	canMoveSquares.clear();

	if (selectedPiece)
	{
		for (auto it = selectedPiece->validMoves.begin();
			it != selectedPiece->validMoves.end(); it++)
		{
			canMoveSquares.push_back(Rectangle(*it, blue, 0.7f));
		}
	}
}

void Game::updateRandHighlightSquare()
{
	if (!promoPrompt)
	{
		// If mouse hovers over board, highlight that square
		if (mousePos.isValid())
		{
			randHighlightSquare =
				std::shared_ptr<Rectangle>(new Rectangle(mousePos, yellow, 0.7f));
		} else if (!mousePos.isValid())
		{
			randHighlightSquare = nullptr;
		}
	} else
	{
		glm::vec2 mouseCoords = glm::vec2(mouseX, mouseY);

		// If mouse hovers over new piece selection, highlight it
		if (isValidPromo(mouseCoords))
		{
			promoSelection = getPromoSelection(mouseCoords);

			randHighlightSquare = std::shared_ptr<Rectangle>(new Rectangle);
			*randHighlightSquare = promoPrompt->squares[promoSelection];
			randHighlightSquare->color = yellow;
			randHighlightSquare->transparency = 0.7f;

			if (leftClick)
			{
				promote();
			}
		} else
		{
			randHighlightSquare = nullptr;
		}
	}
}

void Game::promote()
{
	newPiece(turn, promoPrompt->pieces[promoSelection].type, promoPos);

	promoPrompt = nullptr;
	promoSelection = -1;

	grid.changeColor(brown, lightBrown);

	// these were skipped earlier:
	nextTurn();
	updatePieces();
}

void Game::draw()
{
	if (gameOver)
	{
		grid.changeColor(brown_grayscale, lightBrown_grayscale);
	}

	background.draw();
	grid.draw();
	drawPieces();

	if (!gameOver)
	{
		if (promoPrompt)
		{
			promoPrompt->draw();
		}

		if (randHighlightSquare)
		{
			randHighlightSquare->draw();
		}

		if (selectedSquare)
		{
			selectedSquare->draw();

			for (int i = 0; i < canMoveSquares.size(); i++)
			{
				canMoveSquares[i].draw();
			}
		}
	}
}

void Game::drawPieces()
{
	for (auto it = board.begin(); it != board.end(); it++)
	{
		it->second->draw();
	}
}

void Game::nextTurn()
{
	if (turn == 'W')
	{
		turn = 'B';
	} else
	{
		turn = 'W';
	}
}

bool Game::noValidMove(char color)
{
	for (auto it = board.begin(); it != board.end(); it++)
	{
		if (it->second->color == color && !it->second->validMoves.empty())
		{
			return false;
		}
	}

	return true;
}

bool Game::isValidMove(Position pos)
{
	if (selectedPiece->validMoves.count(pos))
	{
		return true;
	} else
	{
		return false;
	}
}

bool Game::isValidPromo(glm::vec2 mouseCoords)
{
	if (!promoPrompt)
	{
		return false;
	}

	if (mouseCoords.x >= promoPrompt->background.offset.x
		+ promoPrompt->backgroundBorder
		&& mouseCoords.x <= promoPrompt->background.offset.x
		+ promoPrompt->background.width - promoPrompt->backgroundBorder
		&& mouseCoords.y >= promoPrompt->background.offset.y
		+ promoPrompt->backgroundBorder
		&& mouseCoords.y <= promoPrompt->background.offset.y
		+ promoPrompt->background.height - promoPrompt->backgroundBorder)
	{
		return true;
	}

	return false;
}

int Game::getPromoSelection(glm::vec2 mouseCoords)
{
	for (int i = 3; i >= 0; i--)
	{
		if (mouseCoords.x >= promoPrompt->background.offset.x
			+ promoPrompt->backgroundBorder + (squareSize * i))
		{
			return i;
		}
	}

	std::cout << "Game::getPromoSelection ERROR" << std::endl;
	return -1;
}

PromotionPrompt::PromotionPrompt() : PromotionPrompt((Position)"a1") {}

PromotionPrompt::PromotionPrompt(Position promotionSquare)
{
	distance = 8.0f;
	backgroundBorder = 8.0f;

	float width = backgroundBorder * 2 + squareSize * 4;
	float height = backgroundBorder * 2 + squareSize;

	char color = '\0';
	if (promotionSquare.row == 1)
	{
		color = 'B';

		offset = posToOffset(promotionSquare)
			+ glm::vec2(-width / 2.0f, squareSize + distance);
	} else if (promotionSquare.row == 8)
	{
		color = 'W';

		offset = posToOffset(promotionSquare) + glm::vec2(-width / 2.0f,
			(squareSize + distance + (2.0f * backgroundBorder)) * -1.0f);
	}

	while (offset.x < distance)
	{
		offset.x += 1.0f;
	}

	while (offset.x + width > (squareSize * 8.0f) - distance)
	{
		offset.x -= 1.0f;
	}

	background = Rectangle(border + offset.x, border + offset.y, width, height, black);

	for (int i = 0; i < 4; i++)
	{
		squares[i] = Rectangle(background.offset.x + backgroundBorder + (squareSize * i),
			background.offset.y + backgroundBorder, squareSize, squareSize, blue);
	}

	pieces[0] = Piece(color, "knight", true, squares[0].offset);
	pieces[1] = Piece(color, "bishop", true, squares[1].offset);
	pieces[2] = Piece(color, "rook", true, squares[2].offset);
	pieces[3] = Piece(color, "queen", true, squares[3].offset);
}

void PromotionPrompt::draw()
{
	background.draw();

	for (int i = 0; i < 4; i++)
	{
		squares[i].draw();
		pieces[i].draw();
	}
}