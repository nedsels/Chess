#include "pieces.h"

Piece::Piece() { }

Piece::Piece(char color, std::string type, Position position)
{
    shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
    hasMoved = false;
    this->position = position;
    this->type = type;
    this->color = color;
    promotionOption = false;
    manualOffset = glm::vec2(0.0f);

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

    std::string texturePath = "textures/pieces/" + combineChars(color, '_') + type + "_2x_ns.png";

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

    shader.use();
    shader.set1i("pieceTexture", 0);
}

Piece::Piece(char color, std::string type, bool promotionOption, glm::vec2 offset)
{
    shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
    hasMoved = false;
    this->position = Position('\0', -1);
    this->type = type;
    this->color = color;
    this->promotionOption = promotionOption;
    manualOffset = offset;

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

    std::string texturePath = "textures/pieces/" + combineChars(color, '_') + type + "_2x_ns.png";

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

    shader.use();
    shader.set1i("pieceTexture", 0);
}

void Piece::setup()
{
    float vertices[20];
    if (!promotionOption) {
        // bottom left
        vertices[0] = (2.0f / SCR_WIDTH) * border - 1.0f;
        vertices[1] = (2.0f / SCR_HEIGHT) * border - 1.0f;

        // top left
        vertices[5] = (2.0f / SCR_HEIGHT) * border - 1.0f;
        vertices[6] = (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f;

        // bottom right
        vertices[10] = (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f;
        vertices[11] = (2.0f / SCR_HEIGHT) * border - 1.0f;

        // top right
        vertices[15] = (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f;
        vertices[16] = (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f;
    } else {
        // bottom left
        vertices[0] = -1.0f;
        vertices[1] = -1.0f;

        // top left
        vertices[5] = -1.0f;
        vertices[6] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;

        // bottom right
        vertices[10] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
        vertices[11] = -1.0f;

        // top right
        vertices[15] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
        vertices[16] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
    }

    // z coords
    vertices[2] = 0.0f;
    vertices[7] = 0.0f;
    vertices[12] = 0.0f;
    vertices[17] = 0.0f;

    // texture coords
    vertices[3] = 0.0f;
    vertices[4] = 0.0f;

    vertices[8] = 0.0f;
    vertices[9] = 1.0f;

    vertices[13] = 1.0f;
    vertices[14] = 0.0f;

    vertices[18] = 1.0f;
    vertices[19] = 1.0f;

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
    visibleSquares->at(color).insert(diagonal1);

    if (*(kingPositions->at(otherColor)) == diagonal1) {
        (*piecesCheckingKing)++;
        (*checkActionSquares)[color].insert(position);

        /* std::cout << otherColor << " king in check from " << type << " at "
                  << position.col << position.row
                  << ". Attack squares: " << std::endl;
        std::cout << "    " << position.col << position.row << std::endl;
        std::cout << "    " << kingPositions->at(otherColor)->col
                  << kingPositions->at(otherColor)->row << " ("
                  << (*checkActionSquares)[color].at(*(kingPositions->at(otherColor)))
                  << ")" << std::endl; */
    }

    if (board->count(diagonal1) && board->at(diagonal1)->color == otherColor) {
        validMoves[diagonal1] = true;
    }

    Position diagonal2 = add(position, 1, dir);
    visibleSquares->at(color).insert(diagonal2);

    if (*(kingPositions->at(otherColor)) == diagonal2) {
        (*piecesCheckingKing)++;
        (*checkActionSquares)[color].insert(position);

        /* std::cout << otherColor << " king in check from " << type << " at "
                  << position.col << position.row
                  << ". Attack squares: " << std::endl;
        std::cout << "    " << position.col << position.row << std::endl;
        std::cout << "    " << kingPositions->at(otherColor)->col
                  << kingPositions->at(otherColor)->row << " ("
                  << (*checkActionSquares)[color][*(kingPositions->at(otherColor))]
                  << ")" << std::endl; */
    }

    if (board->count(diagonal2) && board->at(diagonal2)->color == otherColor) {
        validMoves[diagonal2] = true;
    }

    // in front
    Position inFront1 = add(position, 0, dir);
    if (!board->count(inFront1)) {
        validMoves[inFront1] = true;

        if ((color == 'W' && position.row == 2) || (color == 'B' && position.row == 7)) {
            Position inFront2 = add(position, 0, 2 * dir);
            if (!board->count(inFront2)) {
                validMoves[inFront2] = true;
            }
        }
    }

    // en passant
    if ((color == 'W' && position.row == 5) || (color == 'B' && position.row == 4)) {
        Position enPassant1 = add(position, -1, dir);
        Position checkPos1 = add(position, -1, 0);

        if (board->count(checkPos1)) {
            Piece* checkPiece = board->at(checkPos1);

            if (checkPiece->type == "pawn" && checkPiece->color == otherColor && checkPiece == *enPassantPawn) {
                validMoves[enPassant1] = true;
            }
        }

        Position enPassant2 = add(position, 1, dir);
        Position checkPos2 = add(position, 1, 0);

        if (board->count(checkPos2)) {
            Piece* checkPiece = board->at(checkPos2);

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
                if (board->count(checkPosition)) {
                    if (!checkForPin) {
                        if (board->at(checkPosition)->color == otherColor) {
                            validMoves[checkPosition] = true;
                            visibleSquares->at(color).insert(checkPosition);
                            pinPosition = checkPosition;
                            checkForPin = true;

                            if (checkPosition == *(kingPositions->at(otherColor))) {
                                (*piecesCheckingKing)++;
                                /* std::cout << otherColor << " king in check from " << type
                                          << " at " << position.col << position.row
                                          << ". Attack squares: " << std::endl; */

                                for (Position attackSquare = add(checkPosition, -colDir, -rowDir);
                                     attackSquare != add(position, -colDir, -rowDir);
                                     attackSquare = add(attackSquare, -colDir, -rowDir)) {
                                    (*checkActionSquares)[color].insert(attackSquare);

                                    /* std::cout << "    " << attackSquare.col << attackSquare.row
                                              << " (" << (*checkActionSquares)[color][attackSquare]
                                              << ")" << std::endl; */
                                }
                            }

                            continue;
                        } else {
                            visibleSquares->at(color).insert(checkPosition);
                            break;
                        }
                    } else if (checkForPin && board->at(checkPosition)->type == "king" && board->at(checkPosition)->color == otherColor) {
                        board->at(pinPosition)->pinned = true;
                        break;
                    }
                }

                if (!checkForPin) {
                    validMoves[checkPosition] = !checkForPin;
                    visibleSquares->at(color).insert(checkPosition);
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
                if (board->count(checkPosition)) {
                    if (board->at(checkPosition)->color == otherColor) {
                        validMoves[checkPosition] = true;
                    }
                } else {
                    validMoves[checkPosition] = true;
                }

                visibleSquares->at(color).insert(checkPosition);

                if (checkPosition == *(kingPositions->at(otherColor))) {
                    (*piecesCheckingKing)++;
                    (*checkActionSquares)[color].insert(position);

                    /* std::cout << otherColor << " king in check from " << type << " at "
                              << position.col << position.row
                              << ". Attack squares: " << std::endl;
                    std::cout << "    " << position.col << position.row << std::endl;
                    std::cout
                        << "    " << kingPositions->at(otherColor)->col
                        << kingPositions->at(otherColor)->row << " ("
                        << (*checkActionSquares)[color][*(kingPositions->at(otherColor))]
                        << ")" << std::endl; */
                }
            }
        }
    }

    for (int j = -2; j <= 2; j += 4) {
        for (int i = -1; i <= 1; i += 2) {
            Position checkPosition = add(position, i, j);

            if (checkPosition.isValid()) {
                if (board->count(checkPosition)) {
                    if (board->at(checkPosition)->color == otherColor) {
                        validMoves[checkPosition] = true;
                    }
                } else {
                    validMoves[checkPosition] = true;
                }

                visibleSquares->at(color).insert(checkPosition);

                if (checkPosition == *(kingPositions->at(otherColor))) {
                    (*piecesCheckingKing)++;
                    (*checkActionSquares)[color].insert(position);

                    /* std::cout << otherColor << " king in check from " << type << " at "
                              << position.col << position.row
                              << ". Attack squares: " << std::endl;
                    std::cout << "    " << position.col << position.row << std::endl;
                    std::cout
                        << "    " << kingPositions->at(otherColor)->col
                        << kingPositions->at(otherColor)->row << " ("
                        << (*checkActionSquares)[color][*(kingPositions->at(otherColor))]
                        << ")" << std::endl; */
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
                if (board->count(checkPosition)) {
                    if (!checkForPin) {
                        if (board->at(checkPosition)->color == otherColor) {
                            validMoves[checkPosition] = true;
                            visibleSquares->at(color).insert(checkPosition);
                            pinPosition = checkPosition;
                            checkForPin = true;

                            if (checkPosition == *(kingPositions->at(otherColor))) {
                                (*piecesCheckingKing)++;
                                /* std::cout << otherColor << " king in check from " << type
                                          << " at " << position.col << position.row
                                          << ". Attack squares: " << std::endl; */

                                for (Position attackSquare = add(checkPosition, -i, -j);
                                     attackSquare != add(position, -i, -j);
                                     attackSquare = add(attackSquare, -i, -j)) {
                                    (*checkActionSquares)[color].insert(attackSquare);

                                    /* std::cout << "    " << attackSquare.col << attackSquare.row
                                              << " (" << (*checkActionSquares)[color][attackSquare]
                                              << ")" << std::endl; */
                                }
                            }

                            continue;
                        } else {
                            visibleSquares->at(color).insert(checkPosition);
                            break;
                        }
                    } else if (checkForPin && board->at(checkPosition)->type == "king" && board->at(checkPosition)->color == otherColor) {
                        board->at(pinPosition)->pinned = true;
                        break;
                    }
                }

                if (!checkForPin) {
                    validMoves[checkPosition] = !checkForPin;
                    visibleSquares->at(color).insert(checkPosition);
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
                if (board->count(checkPosition) && board->at(checkPosition)->color == otherColor) {
                    validMoves[checkPosition] = true;
                } else if (!board->count(checkPosition)) {
                    validMoves[checkPosition] = true;
                }

                visibleSquares->at(color).insert(checkPosition);
            }
        }
    }

    // castling
    Position potentialRookFileA = add(position, -4, 0);
    if (!hasMoved && board->count(potentialRookFileA) && !board->at(potentialRookFileA)->hasMoved && !board->count(add(position, -1, 0)) && !visibleSquares->at(otherColor).count(add(position, -1, 0)) && !board->count(add(position, -2, 0)) && !visibleSquares->at(otherColor).count(add(position, -2, 0)) && !board->count(add(position, -3, 0))) {
        validMoves[add(position, -2, 0)] = true;
    }

    Position potentialRookFileH = add(position, 3, 0);
    if (!hasMoved && board->count(potentialRookFileH) && !board->at(potentialRookFileH)->hasMoved && !board->count(add(position, 1, 0)) && !visibleSquares->at(otherColor).count(add(position, 1, 0)) && !board->count(add(position, 2, 0)) && !visibleSquares->at(otherColor).count(add(position, 2, 0))) {
        validMoves[add(position, 2, 0)] = true;
    }
}

void Piece::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();

    if (!promotionOption) {
        float offsetX = (2.0f / SCR_WIDTH) * (squareSize * (position.col - 'a'));
        float offsetY = (2.0f / SCR_HEIGHT) * (squareSize * (position.row - 1));
        shader.set2f("offset", offsetX, offsetY);
    } else {
        float offsetX = (2.0f / SCR_WIDTH) * manualOffset.x;
        float offsetY = (2.0f / SCR_HEIGHT) * manualOffset.y;
        shader.set2f("offset", offsetX, offsetY);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Piece::updateValidMoves()
{
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

        if (type != "king" && *kingInCheck) {
            char otherColor;
            if (color == 'W') {
                otherColor = 'B';
            } else {
                otherColor = 'W';
            }

            /* std::cout << color << " king in check, so removing " << position.col
                      << position.row << " " << type << " positions:" << std::endl; */
            std::vector<Position> positionsToRemove;

            for (auto it = validMoves.begin(); it != validMoves.end(); it++) {
                if (!checkActionSquares->at(otherColor).count(it->first)) {
                    positionsToRemove.push_back(it->first);
                }
            }

            for (int i = 0; i < positionsToRemove.size(); i++) {
                validMoves.erase(positionsToRemove[i]);

                /* std::cout << "    " << positionsToRemove[i].col
                          << positionsToRemove[i].row << std::endl; */
            }

            // std::cout << std::endl;
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
        if (visibleSquares->at(otherColor).count(it->first)) {
            positionsToRemove.push_back(it->first);
        }
    }

    for (int i = 0; i < positionsToRemove.size(); i++) {
        validMoves.erase(positionsToRemove[i]);
    }
}
