#include "game.h"

Game::Game() {
  leftClick = false;
  rightClick = false;

  grid = Grid();
  background =
      Rectangle(0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, darkGray);

  turn = 'W';
  randHighlightSquare = nullptr;
  selectedSquare = nullptr;
  selectedPiece = nullptr;
  controlledSquares['W'] = std::unordered_map<Position, bool>();
  controlledSquares['B'] = std::unordered_map<Position, bool>();

  board[(Position) "a1"] = new white_rook((Position) "a1");
  board[(Position) "b1"] = new white_knight((Position) "b1");
  board[(Position) "c1"] = new white_bishop((Position) "c1");
  board[(Position) "d1"] = new white_queen((Position) "d1");
  board[(Position) "e1"] = new white_king((Position) "e1");
  board[(Position) "f1"] = new white_bishop((Position) "f1");
  board[(Position) "g1"] = new white_knight((Position) "g1");
  board[(Position) "h1"] = new white_rook((Position) "h1");
  board[(Position) "a8"] = new black_rook((Position) "a8");
  board[(Position) "b8"] = new black_knight((Position) "b8");
  board[(Position) "c8"] = new black_bishop((Position) "c8");
  board[(Position) "d8"] = new black_queen((Position) "d8");
  board[(Position) "e8"] = new black_king((Position) "e8");
  board[(Position) "f8"] = new black_bishop((Position) "f8");
  board[(Position) "g8"] = new black_knight((Position) "g8");
  board[(Position) "h8"] = new black_rook((Position) "h8");

  for (char i = 'a'; i <= 'h'; i++) {
    board[Position(i, 2)] = new white_pawn(Position(i, 2));
    board[Position(i, 7)] = new black_pawn(Position(i, 7));
  }

  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      Position position = Position(i, j);
      if (board.find(position) != board.end()) {
        board[position]->board = &board;
        board[position]->controlledSquares = &controlledSquares;
      }
    }
  }

  updatePieces();
}

void Game::draw() {
  background.draw();
  grid.draw(board);

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

void Game::update() {
  updateMousePos();
  updateSelectedSquare();
  updateRandHighlightSquare();

  leftClick = false;
  rightClick = false;
}

void Game::nextTurn() {
  if (turn == 'W') {
    turn = 'B';
  } else {
    turn = 'W';
  }
}

void Game::updateMousePos() {
  float m = (7 / (SCR_WIDTH - squareSize - 2 * border));
  char col = (char)('a' + floor(m * (mouseX - border)));
  int row = (int)floor(-m * mouseY + (1 + m * (SCR_HEIGHT - border)));

  mousePos = Position(col, row);
}

void Game::updateSelectedSquare() {
  if (selectedSquare && isValidMove(mousePos) && leftClick) {
    movePiece(selectedPiece, mousePos);

    delete selectedSquare;
    selectedSquare = nullptr;

    selectedPiece = nullptr;
  } else if (selectedSquare && rightClick) {
    delete selectedSquare;
    selectedSquare = nullptr;

    selectedPiece = nullptr;
  } else if (board.find(mousePos) != board.end() &&
             board.at(mousePos)->color == turn && leftClick) {
    selectedPiece = getPieceAt(mousePos);

    if (selectedSquare) {
      *selectedSquare =
          Rectangle(border + squareSize * (mousePos.col - 'a'),
                    border + squareSize * (mousePos.row - 1), blue, 0.7f);
    } else {
      selectedSquare =
          new Rectangle(border + squareSize * (mousePos.col - 'a'),
                        border + squareSize * (mousePos.row - 1), blue, 0.7f);
    }

    updateCanMoveSquares();
  }
}

void Game::updateRandHighlightSquare() {
  if (mousePos.isValid() && !randHighlightSquare) {
    randHighlightSquare =
        new Rectangle(border + squareSize * (mousePos.col - 'a'),
                      border + squareSize * (mousePos.row - 1), yellow, 0.7f);
  } else if (mousePos.isValid() && randHighlightSquare) {
    *randHighlightSquare =
        Rectangle(border + squareSize * (mousePos.col - 'a'),
                  border + squareSize * (mousePos.row - 1), yellow, 0.7f);
  } else if (!mousePos.isValid()) {
    delete randHighlightSquare;
    randHighlightSquare = nullptr;
  }
}

void Game::updateCanMoveSquares() {
  canMoveSquares.clear();

  if (selectedPiece) {
    for (char i = 'a'; i <= 'h'; i++) {
      for (int j = 1; j <= 8; j++) {
        Position pos = Position(i, j);

        if (selectedPiece->validMoves.find(pos) !=
            selectedPiece->validMoves.end()) {
          canMoveSquares.push_back(
              Rectangle(border + squareSize * (pos.col - 'a'),
                        border + squareSize * (pos.row - 1), blue, 0.7f));
        }
      }
    }
  }
}

void Game::updatePieces() {
  controlledSquares['W'].clear();
  controlledSquares['B'].clear();

  std::vector<Position> kingPositions;

  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      Position position = Position(i, j);
      if (board.find(position) != board.end()) {
        if (board[position]->type == "king") {
          kingPositions.push_back(position);
        }

        board[position]->updateValidMoves();
      }
    }
  }

  for (int i = 0; i < kingPositions.size(); i++) {
    board[kingPositions[i]]->invalidateMoveIntoCheck();
  }
}

void Game::movePiece(Piece* piece, Position posTo) {
  Position posFrom = piece->position;

  if (isPieceAt(posTo)) {
    delete board[posTo];
    board[posTo] = nullptr;
  }

  board[posTo] = piece;
  piece->position = posTo;
  board.erase(posFrom);

  updatePieces();

  nextTurn();
}

bool Game::isPieceAt(char col, char row) {
  return isPieceAt(Position(col, row));
}

bool Game::isPieceAt(char col, int row) {
  return isPieceAt(Position(col, row));
}

bool Game::isPieceAt(Position pos) {
  if (board.find(pos) != board.end()) {
    return true;
  } else {
    return false;
  }
}

Piece* Game::getPieceAt(char col, char row) {
  return getPieceAt(Position(col, row));
}

Piece* Game::getPieceAt(char col, int row) {
  return getPieceAt(Position(col, row));
}

Piece* Game::getPieceAt(Position pos) {
  if (!isPieceAt(pos)) {
    return nullptr;
  }

  return board[pos];
}

bool Game::isValidMove(Position pos) {
  if (selectedPiece->validMoves.find(pos) != selectedPiece->validMoves.end()) {
    return true;
  } else {
    return false;
  }
}

Game::~Game() {
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
