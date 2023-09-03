#include "game.h"

Game::Game() {
  leftClick = false;
  rightClick = false;

  grid = Grid();
  background =
      Rectangle(0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, darkGray);
  randHighlightSquare = nullptr;
  selectedSquare = nullptr;
  selectedPiece = nullptr;

  board["a1"] = new white_rook("a1");
  board["b1"] = new white_knight("b1");
  board["c1"] = new white_bishop("c1");
  board["d1"] = new white_queen("d1");
  board["e1"] = new white_king("e1");
  board["f1"] = new white_bishop("f1");
  board["g1"] = new white_knight("g1");
  board["h1"] = new white_rook("h1");
  board["a8"] = new black_rook("a8");
  board["b8"] = new black_knight("b8");
  board["c8"] = new black_bishop("c8");
  board["d8"] = new black_queen("d8");
  board["e8"] = new black_king("e8");
  board["f8"] = new black_bishop("f8");
  board["g8"] = new black_knight("g8");
  board["h8"] = new black_rook("h8");

  board["h3"] = new white_pawn("h3");
  board["d3"] = new black_pawn("d3");
  board["c3"] = new black_pawn("c3");

  /* for (char i = 'a'; i <= 'h'; i++) {
    board[std::string(1, i) + "2"] = new white_pawn(std::string(1, i) + "2");
    board[std::string(1, i) + "7"] = new black_pawn(std::string(1, i) + "7");
  } */

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
  updateHighlightSquares();
}

void Game::updateMousePos() {
  float m = (7 / (SCR_WIDTH - squareSize - 2 * border));
  char col = (char)('a' + floor(m * (mouseX - border)));
  int row = (int)floor(-m * mouseY + (1 + m * (SCR_HEIGHT - border)));

  mousePos = Position(col, row);
}

void Game::updateHighlightSquares() {
  if (selectedSquare && isValidMove(mousePos) && leftClick) {
    movePiece(selectedPiece, mousePos);

    delete selectedSquare;
    selectedSquare = nullptr;

    selectedPiece = nullptr;

    selectedSquarePosition = "\0";
  } else if (selectedSquare && rightClick) {
    delete selectedSquare;
    selectedSquare = nullptr;

    selectedPiece = nullptr;

    selectedSquarePosition = "\0";
  } else if (board.find(posToStr(mousePos)) != board.end() && leftClick) {
    selectedSquarePosition = posToStr(mousePos);
    selectedPiece = getPieceAt(selectedSquarePosition);

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

  leftClick = false;
  rightClick = false;
}

void Game::updateCanMoveSquares() {
  canMoveSquares.clear();

  if (selectedPiece) {
    for (int i = 0; i < selectedPiece->validMoves.size(); i++) {
      // std::cout << selectedPiece->validMoves[i] << std::endl;
      canMoveSquares.push_back(Rectangle(
          border + squareSize * (selectedPiece->validMoves[i][0] - 'a'),
          border + squareSize * (selectedPiece->validMoves[i][1] - '1'), blue,
          0.7f));
    }
  }
}

void Game::sendBoardToPieces() {
  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      std::string position = i + std::to_string(j);
      if (board.find(position) != board.end()) {
        board[position]->board = &board;
      }
    }
  }
}

void Game::updatePieces() {
  sendBoardToPieces();

  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      std::string position = i + std::to_string(j);
      if (board.find(position) != board.end()) {
        board[position]->updateValidMoves();
      }
    }
  }
}

void Game::movePiece(Piece* piece, std::string posTo) {
  std::string posFrom = posToStr(piece->position);

  if (isPieceAt(posTo)) {
    delete board[posTo];
    board[posTo] = nullptr;
  }

  board[posTo] = piece;
  piece->position = strToPos(posTo);
  board.erase(posFrom);

  updatePieces();
}

void Game::movePiece(Piece* piece, Position posTo) {
  movePiece(piece, posToStr(posTo));
}

bool Game::isPieceAt(char col, char row) {
  if (board.find(combineChars(col, row)) != board.end()) {
    return true;
  } else {
    return false;
  }
}

bool Game::isPieceAt(char col, int row) {
  if (board.find(combineChars(col, 'a' + row - 1)) != board.end()) {
    return true;
  } else {
    return false;
  }
}

bool Game::isPieceAt(std::string pos) {
  if (board.find(pos) != board.end()) {
    return true;
  } else {
    return false;
  }
}

bool Game::isPieceAt(Position pos) { return isPieceAt(posToStr(pos)); }

Piece* Game::getPieceAt(char col, char row) {
  std::string position = combineChars(col, row);

  if (!isPieceAt(position)) {
    return nullptr;
  }

  return board[position];
}

Piece* Game::getPieceAt(char col, int row) {
  return getPieceAt(col, (char)('a' + (row - 1)));
}

Piece* Game::getPieceAt(std::string pos) {
  if (!isPieceAt(pos)) {
    return nullptr;
  }

  return board[pos];
}

Piece* Game::getPieceAt(Position pos) { return getPieceAt(posToStr(pos)); }

bool Game::isValidMove(std::string pos) {
  for (int i = 0; i < selectedPiece->validMoves.size(); i++) {
    if (selectedPiece->validMoves[i] == pos) {
      return true;
    }
  }

  return false;
}

bool Game::isValidMove(Position pos) { return isValidMove(posToStr(pos)); }

Game::~Game() {
  std::string position;

  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      position = i + std::to_string(j);
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
