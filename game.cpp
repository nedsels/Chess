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
    controlledSquares['W'] = std::unordered_map<Position, bool>();
    controlledSquares['B'] = std::unordered_map<Position, bool>();
    kingInCheck = std::unordered_map<char, bool>();
    kingAttackSquares = std::unordered_map<char, std::unordered_map<Position, int>>();
    enPassantPawn = nullptr;

    board[(Position) "a1"] = new Piece('W', "rook", (Position) "a1");
    board[(Position) "b1"] = new Piece('W', "knight", (Position) "b1");
    board[(Position) "c1"] = new Piece('W', "bishop", (Position) "c1");
    board[(Position) "d1"] = new Piece('W', "queen", (Position) "d1");
    board[(Position) "e1"] = new Piece('W', "king", (Position) "e1");
    kingPositions['W'] = &(board[(Position) "e1"]->position);
    board[(Position) "f1"] = new Piece('W', "bishop", (Position) "f1");
    board[(Position) "g1"] = new Piece('W', "knight", (Position) "g1");
    board[(Position) "h1"] = new Piece('W', "rook", (Position) "h1");
    board[(Position) "a8"] = new Piece('B', "rook", (Position) "a8");
    board[(Position) "b8"] = new Piece('B', "knight", (Position) "b8");
    board[(Position) "c8"] = new Piece('B', "bishop", (Position) "c8");
    board[(Position) "d8"] = new Piece('B', "queen", (Position) "d8");
    board[(Position) "e8"] = new Piece('B', "king", (Position) "e8");
    kingPositions['B'] = &(board[(Position) "e8"]->position);
    board[(Position) "f8"] = new Piece('B', "bishop", (Position) "f8");
    board[(Position) "g8"] = new Piece('B', "knight", (Position) "g8");
    board[(Position) "h8"] = new Piece('B', "rook", (Position) "h8");

    for (char i = 'a'; i <= 'h'; i++) {
        board[Position(i, 2)] = new Piece('W', "pawn", Position(i, 2));
        board[Position(i, 7)] = new Piece('B', "pawn", Position(i, 7));
    }

    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.find(position) != board.end()) {
                board[position]->board = &board;
                board[position]->controlledSquares = &controlledSquares;
                board[position]->kingInCheck = &kingInCheck;
                board[position]->kingAttackSquares = &kingAttackSquares;
                board[position]->kingPositions = &kingPositions;
                board[position]->maxAttack = &maxAttack;

                if (board[position]->type == "pawn") {
                    board[position]->enPassantPawn = &enPassantPawn;
                }
            }
        }
    }

    updatePieces();
}

void Game::draw()
{
    if (gameOver) {
        grid.changeColor(brown_grayscale, lightBrown_grayscale);
    }

    background.draw();
    grid.draw();
    drawPieces();

    if (!gameOver) {
        if (randHighlightSquare) {
            randHighlightSquare->draw();
        }

        if (selectedSquare) {
            selectedSquare->draw();

            for (int i = 0; i < canMoveSquares.size(); i++) {
                canMoveSquares[i].draw();
            }
        }
    }
}

void Game::drawPieces()
{
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.find(position) != board.end()) {
                board[position]->draw();
            }
        }
    }
}

void Game::update()
{
    if (!gameOver) {
        updateMousePos();
        updateSelectedSquare();
        updateRandHighlightSquare();

        leftClick = false;
        rightClick = false;
    }
}

void Game::nextTurn()
{
    if (turn == 'W') {
        turn = 'B';
    } else {
        turn = 'W';
    }
}

void Game::updateMousePos()
{
    float m = (7 / (SCR_WIDTH - squareSize - 2 * border));
    char col = (char)('a' + floor(m * (mouseX - border)));
    int row = (int)floor(-m * mouseY + (1 + m * (SCR_HEIGHT - border)));

    mousePos = Position(col, row);
}

void Game::updateSelectedSquare()
{
    if (selectedSquare && isValidMove(mousePos) && leftClick) {
        movePiece(selectedPiece, mousePos);

        delete selectedSquare;
        selectedSquare = nullptr;

        selectedPiece = nullptr;
    } else if (selectedSquare && rightClick) {
        delete selectedSquare;
        selectedSquare = nullptr;

        selectedPiece = nullptr;
    } else if (board.find(mousePos) != board.end() && board.at(mousePos)->color == turn && leftClick) {
        selectedPiece = getPieceAt(mousePos);

        if (selectedSquare) {
            *selectedSquare = Rectangle(border + squareSize * (mousePos.col - 'a'),
                border + squareSize * (mousePos.row - 1), blue, 0.7f);
        } else {
            selectedSquare = new Rectangle(border + squareSize * (mousePos.col - 'a'),
                border + squareSize * (mousePos.row - 1), blue, 0.7f);
        }

        updateCanMoveSquares();
    }
}

void Game::updateRandHighlightSquare()
{
    if (mousePos.isValid() && !randHighlightSquare) {
        randHighlightSquare = new Rectangle(border + squareSize * (mousePos.col - 'a'),
            border + squareSize * (mousePos.row - 1), yellow, 0.7f);
    } else if (mousePos.isValid() && randHighlightSquare) {
        *randHighlightSquare = Rectangle(border + squareSize * (mousePos.col - 'a'),
            border + squareSize * (mousePos.row - 1), yellow, 0.7f);
    } else if (!mousePos.isValid()) {
        delete randHighlightSquare;
        randHighlightSquare = nullptr;
    }
}

void Game::updateCanMoveSquares()
{
    canMoveSquares.clear();

    if (selectedPiece) {
        for (char i = 'a'; i <= 'h'; i++) {
            for (int j = 1; j <= 8; j++) {
                Position pos = Position(i, j);

                if (selectedPiece->validMoves.find(pos) != selectedPiece->validMoves.end()) {
                    canMoveSquares.push_back(
                        Rectangle(border + squareSize * (pos.col - 'a'),
                            border + squareSize * (pos.row - 1), blue, 0.7f));
                }
            }
        }
    }
}

void Game::updatePieces()
{
    std::cout << ":: :: " << turn << " TURN :: ::" << std::endl
              << std::endl;
    controlledSquares['W'].clear();
    controlledSquares['B'].clear();

    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            kingAttackSquares['W'][Position(i, j)] = 0;
            kingAttackSquares['B'][Position(i, j)] = 0;

            if (board.find(Position(i, j)) != board.end()) {
                board[Position(i, j)]->pinned = false;
            }
        }
    }

    kingInCheck['W'] = false;
    kingInCheck['B'] = false;

    char otherColor;
    if (turn == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    std::unordered_map<char, Position> kingPositions;

    std::cout << "Determining valid moves for " << otherColor << "..."
              << std::endl
              << std::endl;
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.find(position) != board.end() && getPieceAt(position)->color == otherColor) {
                if (board[position]->type == "king") {
                    kingPositions[board[position]->color] = position;
                }

                board[position]->updateValidMoves();
            }
        }
    }

    maxAttack = 0;

    if (kingInCheck[turn]) {
        for (auto it = kingAttackSquares[otherColor].begin();
             it != kingAttackSquares[otherColor].end(); it++) {
            if (it->second > maxAttack) {
                maxAttack = it->second;
            }
        }

        std::cout << std::endl
                  << "Only valid non-king moves for " << turn << ":" << std::endl;
        for (auto it = kingAttackSquares[otherColor].begin();
             it != kingAttackSquares[otherColor].end(); it++) {
            if (it->second == maxAttack) {
                std::cout << "    " << it->first.col << it->first.row << std::endl;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Determining valid moves for " << turn << "..." << std::endl
              << std::endl;
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.find(position) != board.end() && getPieceAt(position)->color == turn) {
                if (board[position]->type == "king") {
                    kingPositions[board[position]->color] = position;
                }

                board[position]->updateValidMoves();
            }
        }
    }

    board[kingPositions[turn]]->invalidateMoveIntoCheck();
    board[kingPositions[otherColor]]->invalidateMoveIntoCheck();
}

void Game::movePiece(Piece* piece, Position posTo)
{
    Position posFrom = piece->position;

    std::cout << "Moving " << piece->color << " " << piece->type << " from "
              << posFrom.col << posFrom.row << " to " << posTo.col << posTo.row
              << std::endl
              << std::endl;

    int dir;
    if (piece->color == 'W') {
        dir = 1;
    } else {
        dir = -1;
    }

    if (isPieceAt(posTo)) {
        delete board[posTo];
        board[posTo] = nullptr;
        board.erase(posTo);
    }

    if (enPassantPawn && piece->type == "pawn" && add(posTo, 0, -dir) == enPassantPawn->position) {
        Position posToDelete = enPassantPawn->position;
        delete board[posToDelete];
        board[posToDelete] = nullptr;
        board.erase(posToDelete);
        enPassantPawn = nullptr;
    }

    board[posTo] = piece;
    piece->position = posTo;
    board.erase(posFrom);

    piece->hasMoved = true;

    if (piece->type == "pawn" && (posTo == add(posFrom, 0, 2) || posFrom == add(posTo, 0, 2))) {
        enPassantPawn = piece;
    } else {
        enPassantPawn = nullptr;
    }

    if (piece->type == "king") {
        if (posFrom == (Position) "e1") {
            if (posTo == (Position) "c1") {
                movePiece(board[(Position) "a1"], (Position) "d1");
                nextTurn();
            } else if (posTo == (Position) "g1") {
                movePiece(board[(Position) "h1"], (Position) "f1");
                nextTurn();
            }
        } else if (posFrom == (Position) "e8") {
            if (posTo == (Position) "c8") {
                movePiece(board[(Position) "a8"], (Position) "d8");
                nextTurn();
            } else if (posTo == (Position) "g8") {
                movePiece(board[(Position) "h8"], (Position) "f8");
                nextTurn();
            }
        }
    }

    nextTurn();

    updatePieces();

    if (noValidMove(turn)) {
        std::cout << "GAME OVER" << std::endl;
        gameOver = true;
    }
}

bool Game::noValidMove(char color)
{
    for (auto it = board.begin(); it != board.end(); it++) {
        if (it->second->color == color && !it->second->validMoves.empty()) {
            return false;
        }
    }

    return true;
}

bool Game::isPieceAt(char col, char row)
{
    return isPieceAt(Position(col, row));
}

bool Game::isPieceAt(char col, int row)
{
    return isPieceAt(Position(col, row));
}

bool Game::isPieceAt(Position pos)
{
    if (board.find(pos) != board.end()) {
        return true;
    } else {
        return false;
    }
}

Piece* Game::getPieceAt(char col, char row)
{
    return getPieceAt(Position(col, row));
}

Piece* Game::getPieceAt(char col, int row)
{
    return getPieceAt(Position(col, row));
}

Piece* Game::getPieceAt(Position pos)
{
    if (!isPieceAt(pos)) {
        return nullptr;
    }

    return board[pos];
}

bool Game::isValidMove(Position pos)
{
    if (selectedPiece->validMoves.find(pos) != selectedPiece->validMoves.end()) {
        return true;
    } else {
        return false;
    }
}

Game::~Game()
{
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.find(position) != board.end()) {
                delete board[position];
                board[position] = nullptr;
                // std::cout << position << " deleted" << std::endl;
            }
        }
    }

    if (randHighlightSquare) {
        delete randHighlightSquare;
        randHighlightSquare = nullptr;
    }

    if (selectedSquare) {
        delete selectedSquare;
        selectedSquare = nullptr;
    }
}
