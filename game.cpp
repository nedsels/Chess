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
    promotion = false;
    promoSelection = -1;

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
                board[position]->visibleSquares = &visibleSquares;
                board[position]->kingInCheck = &(kingInCheck[board[position]->color]);
                board[position]->checkActionSquares = &checkActionSquares;
                board[position]->kingPositions = &kingPositions;
                board[position]->piecesCheckingKing = &piecesCheckingKing;

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
        if (!promotion && selectedSquare) {
            selectedSquare->draw();

            for (int i = 0; i < canMoveSquares.size(); i++) {
                canMoveSquares[i].draw();
            }
        } else if (promotion) {
            promoPrompt->draw();
        }

        if (randHighlightSquare) {
            randHighlightSquare->draw();
        }
    }
}

void Game::drawPieces()
{
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.count(position)) {
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
    if (!promotion) {
        if (selectedSquare && isValidMove(mousePos) && leftClick) {
            movePiece(selectedPiece, mousePos);

            delete selectedSquare;
            selectedSquare = nullptr;

            selectedPiece = nullptr;
        } else if (selectedSquare && rightClick) {
            delete selectedSquare;
            selectedSquare = nullptr;

            selectedPiece = nullptr;
        } else if (board.count(mousePos) && board.at(mousePos)->color == turn && leftClick) {
            selectedPiece = board[mousePos];

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
}

void Game::updateRandHighlightSquare()
{
    if (!promotion) {
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
    } else {
        glm::vec2 promoMousePos = glm::vec2(mouseX, SCR_HEIGHT - mouseY);

        if (isValidPromo(promoMousePos) && leftClick) {
            delete board[promoPos];

            if (promoSelection == 0) {
                board[promoPos] = new Piece(turn, "knight", promoPos);
            } else if (promoSelection == 1) {
                board[promoPos] = new Piece(turn, "bishop", promoPos);
            } else if (promoSelection == 2) {
                board[promoPos] = new Piece(turn, "rook", promoPos);
            } else if (promoSelection == 3) {
                board[promoPos] = new Piece(turn, "queen", promoPos);
            }

            board[promoPos]->board = &board;
            board[promoPos]->visibleSquares = &visibleSquares;
            board[promoPos]->kingInCheck = &(kingInCheck[board[promoPos]->color]);
            board[promoPos]->checkActionSquares = &checkActionSquares;
            board[promoPos]->kingPositions = &kingPositions;
            board[promoPos]->piecesCheckingKing = &piecesCheckingKing;

            promoPrompt = nullptr;
            promotion = false;
            promoSelection = -1;

            grid.changeColor(brown, lightBrown);

            nextTurn();

            updatePieces();
        } else if (isValidPromo(promoMousePos)) {
            promoSelection = getPromoSelection(promoMousePos);

            if (!randHighlightSquare) {
                randHighlightSquare = new Rectangle;
            }

            *randHighlightSquare = promoPrompt->squares[promoSelection];
            randHighlightSquare->color = yellow;
            randHighlightSquare->transparency = 0.7f;
        } else {
            delete randHighlightSquare;
            randHighlightSquare = nullptr;
        }
    }
}

void Game::updateCanMoveSquares()
{
    canMoveSquares.clear();

    if (selectedPiece) {
        for (char i = 'a'; i <= 'h'; i++) {
            for (int j = 1; j <= 8; j++) {
                Position pos = Position(i, j);

                if (selectedPiece->validMoves.count(pos)) {
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
    /* std::cout << ":: :: " << turn << " TURN :: ::" << std::endl
              << std::endl; */

    visibleSquares['W'].clear();
    visibleSquares['B'].clear();

    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            checkActionSquares['W'].erase(Position(i, j));
            checkActionSquares['B'].erase(Position(i, j));

            if (board.count(Position(i, j))) {
                board[Position(i, j)]->pinned = false;
                board[Position(i, j)]->validMoves.clear();
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

    piecesCheckingKing = 0;

    /* std::cout << "Determining valid moves for " << otherColor << "..."
              << std::endl
              << std::endl; */
    for (char i = 'a'; i <= 'h'; i++) {
        for (int j = 1; j <= 8; j++) {
            Position position = Position(i, j);
            if (board.count(position) && board[position]->color == otherColor) {
                board[position]->updateValidMoves();
            }
        }
    }

    if (piecesCheckingKing) {
        // std::cout << piecesCheckingKing << " pieces checking king" << std::endl;
        kingInCheck[turn] = true;
    }

    /* std::cout << "Determining valid moves for " << turn << "..." << std::endl
              << std::endl; */
    if (piecesCheckingKing < 2) {
        for (char i = 'a'; i <= 'h'; i++) {
            for (int j = 1; j <= 8; j++) {
                Position position = Position(i, j);
                if (board.count(position) && board[position]->color == turn) {
                    board[position]->updateValidMoves();
                }
            }
        }
    } else {
        board[*kingPositions[turn]]->updateValidMoves();
    }

    board[*kingPositions[turn]]->invalidateMoveIntoCheck();
    board[*kingPositions[otherColor]]->invalidateMoveIntoCheck();
}

void Game::movePiece(Piece* piece, Position posTo)
{
    Position posFrom = piece->position;

    /* std::cout << "Moving " << piece->color << " " << piece->type << " from "
              << posFrom.col << posFrom.row << " to " << posTo.col << posTo.row
              << std::endl
              << std::endl; */

    int dir;
    if (piece->color == 'W') {
        dir = 1;
    } else {
        dir = -1;
    }

    if (board.count(posTo)) {
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

    if (piece->type == "pawn" && (posTo.row == 8 || posTo.row == 1)) {
        promotion = true;
        promoPrompt = new PromotionPrompt(posTo);
        promoPos = posTo;

        grid.changeColor(brown_grayscale, lightBrown_grayscale);

        int gridIndex = (posTo.row - 1) * 8 + (posTo.col - 'a');
        if ((gridIndex % 2 == 0 && gridIndex / 8 % 2 == 0) || (gridIndex % 2 == 1 && gridIndex / 8 % 2 == 1)) {
            grid.squares[gridIndex].color = brown;
        } else {
            grid.squares[gridIndex].color = lightBrown;
        }
    }

    if (!promotion) {
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

bool Game::isValidMove(Position pos)
{
    if (selectedPiece->validMoves.find(pos) != selectedPiece->validMoves.end()) {
        return true;
    } else {
        return false;
    }
}

bool Game::isValidPromo(glm::vec2 mouseCoords)
{
    if (!promoPrompt) {
        return false;
    }

    if (mouseCoords.x >= promoPrompt->background.offset.x + promoPrompt->backgroundBorder && mouseCoords.x <= promoPrompt->background.offset.x + promoPrompt->background.width - promoPrompt->backgroundBorder && mouseCoords.y >= promoPrompt->background.offset.y + promoPrompt->backgroundBorder && mouseCoords.y <= promoPrompt->background.offset.y + promoPrompt->background.height - promoPrompt->backgroundBorder) {
        return true;
    }

    return false;
}

int Game::getPromoSelection(glm::vec2 mouseCoords)
{
    for (int i = 3; i >= 0; i--) {
        if (mouseCoords.x >= promoPrompt->background.offset.x + promoPrompt->backgroundBorder + (squareSize * i)) {
            return i;
        }
    }

    std::cout << "Game::getPromoSelection ERROR" << std::endl;
    return -1;
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

PromotionPrompt::PromotionPrompt() { }

PromotionPrompt::PromotionPrompt(Position promotionSquare)
{
    /* std::cout << "Creating new promoPromt for " << promotionSquare.col << promotionSquare.row << std::endl
              << std::endl; */

    distance = 8.0f;
    backgroundBorder = 8.0f;

    float width = backgroundBorder * 2 + squareSize * 4;
    float height = backgroundBorder * 2 + squareSize;

    /* std::cout << "width = " << width << std::endl;
    std::cout << "height = " << height << std::endl; */

    char color = '\0';
    if (promotionSquare.row == 1) {
        color = 'B';

        offset = posToCoord(promotionSquare) + glm::vec2(-width / 2.0f, squareSize + distance);
    } else if (promotionSquare.row == 8) {
        color = 'W';

        offset = posToCoord(promotionSquare) + glm::vec2(-width / 2.0f, (squareSize + distance + (2.0f * backgroundBorder)) * -1.0f);
    }

    /* std::cout << std::endl
              << "offset = (" << offset.x << ", " << offset.y << ")" << std::endl; */

    while (offset.x < distance) {
        offset.x += 1.0f;
    }

    while (offset.x + width > (squareSize * 8) - distance) {
        offset.x -= 1.0f;
    }

    // std::cout << "After adjustment, offset = (" << offset.x << ", " << offset.y << ")" << std::endl;

    background = Rectangle(border + offset.x, border + offset.y, width, height, black);

    /* std::cout << std::endl
              << "Background at (" << background.offset.x << ", " << background.offset.y << ")" << std::endl
              << std::endl; */

    for (int i = 0; i < 4; i++) {
        squares[i] = Rectangle(background.offset.x + backgroundBorder + (squareSize * i), background.offset.y + backgroundBorder, squareSize, squareSize, blue);
    }

    pieces[0] = Piece(color, "knight", true, squares[0].offset);
    // std::cout << color << " knight with offset (" << squares[0].offset.x << ", " << squares[0].offset.y << ")" << std::endl;
    pieces[1] = Piece(color, "bishop", true, squares[1].offset);
    // std::cout << color << " bishop with offset (" << squares[1].offset.x << ", " << squares[1].offset.y << ")" << std::endl;
    pieces[2] = Piece(color, "rook", true, squares[2].offset);
    // std::cout << color << " rook with offset (" << squares[2].offset.x << ", " << squares[2].offset.y << ")" << std::endl;
    pieces[3] = Piece(color, "queen", true, squares[3].offset);
    // std::cout << color << " queen with offset (" << squares[3].offset.x << ", " << squares[3].offset.y << ")" << std::endl;
}

void PromotionPrompt::draw()
{
    background.draw();

    for (int i = 0; i < 4; i++) {
        squares[i].draw();
        pieces[i].draw();
    }
}