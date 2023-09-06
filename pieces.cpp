#include "pieces.h"

Piece::Piece() { }

Piece::Piece(char color, std::string type, Position position)
{
    shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
    hasMoved = false;
    this->position = position;
    this->type = type;
    this->color = color;

    if (type == "pawn") {
        value = 1;
    } else if (type == "bishop" || type == "knight") {
        value = 3;
    } else if (type == "rook") {
        value = 5;
    } else if (type == "queen") {
        value = 9;
    } else {
        value = 0;
    }

    setup();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::string texturePath = "pieces/" + combineChars(color, '_') + type + "_2x_ns.png";

    data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // std::cout << << color << " " << type << " generated at " << position << std::endl;
    } else {
        std::cout << "Failed to load " << color << " " << type << std::endl;
    }

    stbi_image_free(data);

    shader.set1i("pieceTexture", 0);
}

void Piece::setup()
{
    float vertices[20] = {
        // bottom left
        (2.0f / SCR_WIDTH) * border - 1.0f, (2.0f / SCR_HEIGHT) * border - 1.0f,
        0.0f, // vertices
        0.0f, 0.0f, // texture

        // top left
        (2.0f / SCR_HEIGHT) * border - 1.0f,
        (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f, 0.0f, // vertices
        0.0f, 1.0f, // texture

        // bottom right
        (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f,
        (2.0f / SCR_HEIGHT) * border - 1.0f, 0.0f, // vertices
        1.0f, 0.0f, // texture

        // top right
        (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f,
        (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f, 0.0f, // vertices
        1.0f, 1.0f // texture
    };

    unsigned int indices[6] = {
        0, 1, 2, // first triangle
        1, 2, 3 // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Piece::updatePawnMoves()
{
    int dir;
    char otherColor;
    if (color == 'W') {
        dir = 1;
        otherColor = 'B';
    } else {
        dir = -1;
        otherColor = 'W';
    }

    // diagonals
    Position diagonal1 = add(position, -1, dir);
    controlledSquares->at(color)[diagonal1] = true;

    if (*(kingPositions->at(otherColor)) == diagonal1) {
        (*kingInCheck)[otherColor] = true;
        (*kingAttackSquares)[color][position]++;
        (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]++;

        std::cout << otherColor << " king in check from " << type << " at "
                  << position.col << position.row
                  << ". Attack squares: " << std::endl;
        std::cout << "    " << position.col << position.row << std::endl;
        std::cout << "    " << kingPositions->at(otherColor)->col
                  << kingPositions->at(otherColor)->row << " ("
                  << (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]
                  << ")" << std::endl;
    }

    if (isPieceAt(diagonal1) && getPieceAt(diagonal1)->color == otherColor) {
        validMoves[diagonal1] = true;
    }

    Position diagonal2 = add(position, 1, dir);
    controlledSquares->at(color)[diagonal2] = true;

    if (*(kingPositions->at(otherColor)) == diagonal2) {
        (*kingInCheck)[otherColor] = true;
        (*kingAttackSquares)[color][position]++;
        (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]++;

        std::cout << otherColor << " king in check from " << type << " at "
                  << position.col << position.row
                  << ". Attack squares: " << std::endl;
        std::cout << "    " << position.col << position.row << std::endl;
        std::cout << "    " << kingPositions->at(otherColor)->col
                  << kingPositions->at(otherColor)->row << " ("
                  << (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]
                  << ")" << std::endl;
    }

    if (isPieceAt(diagonal2) && getPieceAt(diagonal2)->color == otherColor) {
        validMoves[diagonal2] = true;
    }

    // in front
    Position inFront1 = add(position, 0, dir);
    if (!isPieceAt(inFront1)) {
        validMoves[inFront1] = true;

        if ((color == 'W' && position.row == 2) || (color == 'B' && position.row == 7)) {
            Position inFront2 = add(position, 0, 2 * dir);
            if (!isPieceAt(inFront2)) {
                validMoves[inFront2] = true;
            }
        }
    }

    // en passant
    if ((color == 'W' && position.row == 5) || (color == 'B' && position.row == 4)) {
        Position enPassant1 = add(position, -1, dir);
        Position checkPos1 = add(position, -1, 0);

        if (isPieceAt(checkPos1)) {
            Piece* checkPiece = getPieceAt(checkPos1);

            if (checkPiece->type == "pawn" && checkPiece->color == otherColor && checkPiece == *enPassantPawn) {
                validMoves[enPassant1] = true;
            }
        }

        Position enPassant2 = add(position, 1, dir);
        Position checkPos2 = add(position, 1, 0);

        if (isPieceAt(checkPos2)) {
            Piece* checkPiece = getPieceAt(checkPos2);

            if (checkPiece->type == "pawn" && checkPiece->color == otherColor && checkPiece == *enPassantPawn) {
                validMoves[enPassant2] = true;
            }
        }
    }
}

void Piece::updateBishopMoves()
{
    char otherColor;
    if (color == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    for (int colDir = -1; colDir <= 1; colDir += 2) {
        for (int rowDir = -1; rowDir <= 1; rowDir += 2) {
            bool checkForPin = false;
            Position pinPosition;

            for (Position checkPosition = add(position, colDir, rowDir);
                 checkPosition.isValid();
                 checkPosition = add(checkPosition, colDir, rowDir)) {
                if (isPieceAt(checkPosition)) {
                    if (!checkForPin) {
                        if (getPieceAt(checkPosition)->color == otherColor) {
                            validMoves[checkPosition] = true;
                            controlledSquares->at(color)[checkPosition] = true;
                            pinPosition = checkPosition;
                            checkForPin = true;

                            if (checkPosition == *(kingPositions->at(otherColor))) {
                                (*kingInCheck)[otherColor] = true;
                                std::cout << otherColor << " king in check from " << type
                                          << " at " << position.col << position.row
                                          << ". Attack squares: " << std::endl;

                                for (Position attackSquare = checkPosition;
                                     attackSquare != add(position, -colDir, -rowDir);
                                     attackSquare = add(attackSquare, -colDir, -rowDir)) {
                                    (*kingAttackSquares)[color][attackSquare]++;

                                    std::cout << "    " << attackSquare.col << attackSquare.row
                                              << " (" << (*kingAttackSquares)[color][attackSquare]
                                              << ")" << std::endl;
                                }
                            }

                            continue;
                        } else {
                            controlledSquares->at(color)[checkPosition] = true;
                            break;
                        }
                    } else if (checkForPin && getPieceAt(checkPosition)->type == "king" && getPieceAt(checkPosition)->color == otherColor) {
                        board->at(pinPosition)->pinned = true;
                        break;
                    }
                }

                if (!checkForPin) {
                    validMoves[checkPosition] = !checkForPin;
                    controlledSquares->at(color)[checkPosition] = true;
                }
            }
        }
    }
}

void Piece::updateKnightMoves()
{
    char otherColor;
    if (color == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    for (int i = -2; i <= 2; i += 4) {
        for (int j = -1; j <= 1; j += 2) {
            Position checkPosition = add(position, i, j);

            if (checkPosition.isValid()) {
                if (isPieceAt(checkPosition)) {
                    if (getPieceAt(checkPosition)->color == otherColor) {
                        validMoves[checkPosition] = true;
                    }
                } else {
                    validMoves[checkPosition] = true;
                }

                controlledSquares->at(color)[checkPosition] = true;

                if (checkPosition == *(kingPositions->at(otherColor))) {
                    (*kingInCheck)[otherColor] = true;
                    (*kingAttackSquares)[color][position]++;
                    (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]++;

                    std::cout << otherColor << " king in check from " << type << " at "
                              << position.col << position.row
                              << ". Attack squares: " << std::endl;
                    std::cout << "    " << position.col << position.row << std::endl;
                    std::cout
                        << "    " << kingPositions->at(otherColor)->col
                        << kingPositions->at(otherColor)->row << " ("
                        << (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]
                        << ")" << std::endl;
                }
            }
        }
    }

    for (int j = -2; j <= 2; j += 4) {
        for (int i = -1; i <= 1; i += 2) {
            Position checkPosition = add(position, i, j);

            if (checkPosition.isValid()) {
                if (isPieceAt(checkPosition)) {
                    if (getPieceAt(checkPosition)->color == otherColor) {
                        validMoves[checkPosition] = true;
                    }
                } else {
                    validMoves[checkPosition] = true;
                }

                controlledSquares->at(color)[checkPosition] = true;

                if (checkPosition == *(kingPositions->at(otherColor))) {
                    (*kingInCheck)[otherColor] = true;
                    (*kingAttackSquares)[color][position]++;
                    (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]++;

                    std::cout << otherColor << " king in check from " << type << " at "
                              << position.col << position.row
                              << ". Attack squares: " << std::endl;
                    std::cout << "    " << position.col << position.row << std::endl;
                    std::cout
                        << "    " << kingPositions->at(otherColor)->col
                        << kingPositions->at(otherColor)->row << " ("
                        << (*kingAttackSquares)[color][*(kingPositions->at(otherColor))]
                        << ")" << std::endl;
                }
            }
        }
    }
}

void Piece::updateRookMoves()
{
    char otherColor;
    if (color == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if ((i && j) || (!i && !j)) {
                continue;
            }

            bool checkForPin = false;
            Position pinPosition;

            for (Position checkPosition = add(position, i, j);
                 checkPosition.isValid(); checkPosition = add(checkPosition, i, j)) {
                if (isPieceAt(checkPosition)) {
                    if (!checkForPin) {
                        if (getPieceAt(checkPosition)->color == otherColor) {
                            validMoves[checkPosition] = true;
                            controlledSquares->at(color)[checkPosition] = true;
                            pinPosition = checkPosition;
                            checkForPin = true;

                            if (checkPosition == *(kingPositions->at(otherColor))) {
                                (*kingInCheck)[otherColor] = true;
                                std::cout << otherColor << " king in check from " << type
                                          << " at " << position.col << position.row
                                          << ". Attack squares: " << std::endl;

                                for (Position attackSquare = checkPosition;
                                     attackSquare != add(position, -i, -j);
                                     attackSquare = add(attackSquare, -i, -j)) {
                                    (*kingAttackSquares)[color][attackSquare]++;

                                    std::cout << "    " << attackSquare.col << attackSquare.row
                                              << " (" << (*kingAttackSquares)[color][attackSquare]
                                              << ")" << std::endl;
                                }
                            }

                            continue;
                        } else {
                            controlledSquares->at(color)[checkPosition] = true;
                            break;
                        }
                    } else if (checkForPin && getPieceAt(checkPosition)->type == "king" && getPieceAt(checkPosition)->color == otherColor) {
                        board->at(pinPosition)->pinned = true;
                        break;
                    }
                }

                if (!checkForPin) {
                    validMoves[checkPosition] = !checkForPin;
                    controlledSquares->at(color)[checkPosition] = true;
                }
            }
        }
    }
}

void Piece::updateQueenMoves()
{
    updateBishopMoves();
    updateRookMoves();
}

void Piece::updateKingMoves()
{
    char otherColor;
    if (color == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            if (!j && !i) {
                continue;
            }

            Position checkPosition = add(position, i, j);

            if (checkPosition.isValid()) {
                if (isPieceAt(checkPosition) && getPieceAt(checkPosition)->color == otherColor) {
                    validMoves[checkPosition] = true;
                } else if (!isPieceAt(checkPosition)) {
                    validMoves[checkPosition] = true;
                }

                controlledSquares->at(color)[checkPosition] = true;
            }
        }
    }

    // castling
    Position potentialRookFileA = add(position, -4, 0);
    if (!hasMoved && board->find(potentialRookFileA) != board->end() && !board->at(potentialRookFileA)->hasMoved && board->find(add(position, -1, 0)) == board->end() && controlledSquares->at(otherColor).find(add(position, -1, 0)) == controlledSquares->at(otherColor).end() && board->find(add(position, -2, 0)) == board->end() && controlledSquares->at(otherColor).find(add(position, -2, 0)) == controlledSquares->at(otherColor).end() && board->find(add(position, -3, 0)) == board->end()) {
        validMoves[add(position, -2, 0)] = true;
    }

    Position potentialRookFileH = add(position, 3, 0);
    if (!hasMoved && board->find(potentialRookFileH) != board->end() && !board->at(potentialRookFileH)->hasMoved && board->find(add(position, 1, 0)) == board->end() && controlledSquares->at(otherColor).find(add(position, 1, 0)) == controlledSquares->at(otherColor).end() && board->find(add(position, 2, 0)) == board->end() && controlledSquares->at(otherColor).find(add(position, 2, 0)) == controlledSquares->at(otherColor).end()) {
        validMoves[add(position, 2, 0)] = true;
    }
}

void Piece::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    float offsetX = (2.0f / SCR_WIDTH) * (squareSize * (position.col - 'a'));
    float offsetY = (2.0f / SCR_WIDTH) * (squareSize * (position.row - 1));

    shader.set2f("offset", offsetX, offsetY);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Piece::updateValidMoves()
{
    validMoves.clear();

    if (!pinned) {
        if (type == "pawn") {
            updatePawnMoves();
        } else if (type == "bishop") {
            updateBishopMoves();
        } else if (type == "knight") {
            updateKnightMoves();
        } else if (type == "rook") {
            updateRookMoves();
        } else if (type == "queen") {
            updateQueenMoves();
        } else if (type == "king") {
            updateKingMoves();
        }

        if (type != "king" && kingInCheck->at(color)) {
            char otherColor;
            if (color == 'W') {
                otherColor = 'B';
            } else {
                otherColor = 'W';
            }

            std::cout << color << " king in check, so removing " << position.col
                      << position.row << " " << type << " positions:" << std::endl;
            std::vector<Position> positionsToRemove;

            for (auto it = validMoves.begin(); it != validMoves.end(); it++) {
                if (kingAttackSquares->at(otherColor)[it->first] < *maxAttack) {
                    positionsToRemove.push_back(it->first);
                }
            }

            for (int i = 0; i < positionsToRemove.size(); i++) {
                validMoves.erase(positionsToRemove[i]);

                std::cout << "    " << positionsToRemove[i].col
                          << positionsToRemove[i].row << std::endl;
            }

            std::cout << std::endl;
        }
    }
}

void Piece::invalidateMoveIntoCheck()
{
    char otherColor;
    if (color == 'W') {
        otherColor = 'B';
    } else {
        otherColor = 'W';
    }

    if (type != "king") {
        return;
    }

    std::vector<Position> positionsToRemove;
    for (auto it = validMoves.begin(); it != validMoves.end(); it++) {
        if (controlledSquares->at(otherColor).find(it->first) != controlledSquares->at(otherColor).end()) {
            positionsToRemove.push_back(it->first);
        }
    }

    for (int i = 0; i < positionsToRemove.size(); i++) {
        validMoves.erase(positionsToRemove[i]);
    }
}

bool Piece::isPieceAt(char col, char row)
{
    return isPieceAt(Position(col, row));
}

bool Piece::isPieceAt(char col, int row)
{
    return isPieceAt(Position(col, row));
}

bool Piece::isPieceAt(Position pos)
{
    if (board->find(pos) != board->end()) {
        return true;
    } else {
        return false;
    }
}

Piece* Piece::getPieceAt(char col, char row)
{
    return getPieceAt(Position(col, row));
}

Piece* Piece::getPieceAt(char col, int row)
{
    return getPieceAt(Position(col, row));
}

Piece* Piece::getPieceAt(Position pos)
{
    if (!isPieceAt(pos)) {
        return nullptr;
    }

    return board->at(pos);
}
