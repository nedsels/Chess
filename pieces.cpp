#include "pieces.h"

Piece::Piece(std::string position) {
  shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
  canEnPassant = false;
  this->position = strToPos(position);
  board = nullptr;
  validMoves = std::vector<std::string>();

  setup();

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Piece::setup() {
  float vertices[20] = {
      // bottom left
      (2.0f / SCR_WIDTH) * border - 1.0f, (2.0f / SCR_HEIGHT) * border - 1.0f,
      0.0f,        // vertices
      0.0f, 0.0f,  // texture

      // top left
      (2.0f / SCR_HEIGHT) * border - 1.0f,
      (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f,
      0.0f,        // vertices
      0.0f, 1.0f,  // texture

      // bottom right
      (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f,
      (2.0f / SCR_HEIGHT) * border - 1.0f,
      0.0f,        // vertices
      1.0f, 0.0f,  // texture

      // top right
      (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f,
      (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f,
      0.0f,       // vertices
      1.0f, 1.0f  // texture
  };

  unsigned int indices[6] = {
      0, 1, 2,  // first triangle
      1, 2, 3   // second triangle
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

std::vector<std::string> Piece::getPawnMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves;

  if (piece == nullptr) {
    return validMoves;
  }

  int dir;
  char otherColor;
  if (piece->color == 'W') {
    dir = 1;
    otherColor = 'B';
  } else {
    dir = -1;
    otherColor = 'W';
  }

  std::string pos = posToStr(piece->position);

  // diagonals
  std::string diagonal1 = combineChars(pos[0] - 1, pos[1] + dir);
  if (isPieceAt(diagonal1)) {
    validMoves.push_back(diagonal1);
  }

  std::string diagonal2 = combineChars(pos[0] + 1, pos[1] + dir);
  if (isPieceAt(diagonal2)) {
    validMoves.push_back(diagonal2);
  }

  // in front
  std::string inFront1 = combineChars(pos[0], pos[1] + dir);
  if (!isPieceAt(inFront1)) {
    validMoves.push_back(inFront1);
  }

  if ((piece->color == 'W' && pos[1] == '2') ||
      (piece->color == 'B' && pos[1] == '7')) {
    std::string inFront2 = combineChars(pos[0], pos[1] + 2 * dir);
    if (!isPieceAt(inFront2)) {
      validMoves.push_back(inFront2);
    }
  }

  // en passant
  if ((piece->color == 'W' && pos[1] == '5') ||
      (piece->color == 'B' && pos[1] == '4')) {
    std::string enPassant1 = combineChars(pos[0] - 1, pos[1] + dir);
    std::string enPassant2 = combineChars(pos[0] + 1, pos[1] + dir);

    if (isPieceAt(pos[0] - 1, pos[1])) {
      Piece* checkPiece = getPieceAt(pos[0] - 1, pos[1]);

      if (checkPiece->type == "pawn" && checkPiece->color == otherColor &&
          checkPiece->canEnPassant) {
        validMoves.push_back(enPassant1);
      }
    }

    if (isPieceAt(pos[0] + 1, pos[1])) {
      Piece* checkPiece = getPieceAt(pos[0] + 1, pos[1]);

      if (checkPiece->type == "pawn" && checkPiece->color == otherColor &&
          checkPiece->canEnPassant) {
        validMoves.push_back(enPassant2);
      }
    }
  }

  return validMoves;
}

std::vector<std::string> Piece::getBishopMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves;

  if (piece == nullptr) {
    return validMoves;
  }

  char otherColor;
  if (piece->color == 'W') {
    otherColor = 'B';
  } else {
    otherColor = 'W';
  }

  std::string pos = posToStr(piece->position);

  char i;
  char j;

  // up-right diagonal
  i = (char)(pos[0] + 1);
  j = (char)(pos[1] + 1);
  while (Position(i, j).isValid()) {
    if (isPieceAt(i, j)) {
      if (getPieceAt(i, j)->color == otherColor) {
        validMoves.push_back(combineChars(i, j));
      }

      break;
    }

    validMoves.push_back(combineChars(i, j));

    i++;
    j++;
  }

  // up-left diagonal
  i = (char)(pos[0] - 1);
  j = (char)(pos[1] + 1);
  while (Position(i, j).isValid()) {
    if (isPieceAt(i, j)) {
      if (getPieceAt(i, j)->color == otherColor) {
        validMoves.push_back(combineChars(i, j));
      }

      break;
    }

    validMoves.push_back(combineChars(i, j));

    i--;
    j++;
  }

  // down-right diagonal
  i = (char)(pos[0] + 1);
  j = (char)(pos[1] - 1);
  while (Position(i, j).isValid()) {
    if (isPieceAt(i, j)) {
      if (getPieceAt(i, j)->color == otherColor) {
        validMoves.push_back(combineChars(i, j));
      }

      break;
    }

    validMoves.push_back(combineChars(i, j));

    i++;
    j--;
  }

  // down-left diagonal
  i = (char)(pos[0] - 1);
  j = (char)(pos[1] - 1);
  while (Position(i, j).isValid()) {
    if (isPieceAt(i, j)) {
      if (getPieceAt(i, j)->color == otherColor) {
        validMoves.push_back(combineChars(i, j));
      }

      break;
    }

    validMoves.push_back(combineChars(i, j));

    i--;
    j--;
  }

  return validMoves;
}

std::vector<std::string> Piece::getKnightMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves;

  if (piece == nullptr) {
    return validMoves;
  }

  char otherColor;
  if (piece->color == 'W') {
    otherColor = 'B';
  } else {
    otherColor = 'W';
  }

  std::string pos = posToStr(piece->position);

  for (int i = -2; i <= 2; i += 4) {
    for (int j = -1; j <= 1; j += 2) {
      Position checkPosition = Position((char)(pos[0] + i), (char)(pos[1] + j));
      if (checkPosition.isValid()) {
        if (isPieceAt(checkPosition) &&
            getPieceAt(checkPosition)->color == otherColor) {
          validMoves.push_back(posToStr(checkPosition));
        } else if (!isPieceAt(checkPosition)) {
          validMoves.push_back(posToStr(checkPosition));
        }
      }
    }
  }

  for (int j = -2; j <= 2; j += 4) {
    for (int i = -1; i <= 1; i += 2) {
      Position checkPosition = Position((char)(pos[0] + i), (char)(pos[1] + j));

      if (checkPosition.isValid()) {
        if (isPieceAt(checkPosition) &&
            getPieceAt(checkPosition)->color == otherColor) {
          validMoves.push_back(posToStr(checkPosition));
        } else if (!isPieceAt(checkPosition)) {
          validMoves.push_back(posToStr(checkPosition));
        }
      }
    }
  }

  return validMoves;
}

std::vector<std::string> Piece::getRookMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves;

  if (piece == nullptr) {
    return validMoves;
  }

  char otherColor;
  if (piece->color == 'W') {
    otherColor = 'B';
  } else {
    otherColor = 'W';
  }

  std::string pos = posToStr(piece->position);

  // up
  for (char i = pos[0] + 1; Position(i, pos[1]).isValid(); i++) {
    if (isPieceAt(i, pos[1])) {
      if (getPieceAt(i, pos[1])->color == otherColor) {
        validMoves.push_back(combineChars(i, pos[1]));
      }

      break;
    }

    validMoves.push_back(combineChars(i, pos[1]));
  }

  // down
  for (char i = pos[0] - 1; Position(i, pos[1]).isValid(); i--) {
    if (isPieceAt(i, pos[1])) {
      if (getPieceAt(i, pos[1])->color == otherColor) {
        validMoves.push_back(combineChars(i, pos[1]));
      }

      break;
    }

    validMoves.push_back(combineChars(i, pos[1]));
  }

  // right
  for (char i = pos[1] + 1; Position(pos[0], i).isValid(); i++) {
    if (isPieceAt(pos[0], i)) {
      if (getPieceAt(pos[0], i)->color == otherColor) {
        validMoves.push_back(combineChars(pos[0], i));
      }

      break;
    }

    validMoves.push_back(combineChars(pos[0], i));
  }

  // left
  for (char i = pos[1] - 1; Position(pos[0], i).isValid(); i--) {
    if (isPieceAt(pos[0], i)) {
      if (getPieceAt(pos[0], i)->color == otherColor) {
        validMoves.push_back(combineChars(pos[0], i));
      }

      break;
    }

    validMoves.push_back(combineChars(pos[0], i));
  }

  return validMoves;
}

std::vector<std::string> Piece::getQueenMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves = getRookMoves(piece, color);
  std::vector<std::string> diagonalMoves = getBishopMoves(piece, color);
  validMoves.insert(validMoves.end(), diagonalMoves.begin(),
                    diagonalMoves.end());

  return validMoves;
}

std::vector<std::string> Piece::getKingMoves(Piece* piece, char color) {
  std::vector<std::string> validMoves;

  if (piece == nullptr) {
    return validMoves;
  }

  char otherColor;
  if (piece->color == 'W') {
    otherColor = 'B';
  } else {
    otherColor = 'W';
  }

  std::string pos = posToStr(piece->position);

  for (int j = -1; j <= 1; j++) {
    for (int i = -1; i <= 1; i++) {
      if (j == 0 && i == 0) {
        continue;
      }

      Position checkPosition = Position((char)(pos[0] + i), (char)(pos[1] + j));

      if (checkPosition.isValid()) {
        if (isPieceAt(checkPosition) &&
            getPieceAt(checkPosition)->color == otherColor) {
          validMoves.push_back(posToStr(checkPosition));
        } else if (!isPieceAt(checkPosition)) {
          validMoves.push_back(posToStr(checkPosition));
        }
      }
    }
  }

  return validMoves;
}

void Piece::draw() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  shader.use();
  float offsetX = (2.0f / SCR_WIDTH) * (squareSize * (position.col - 'a'));
  float offsetY = (2.0f / SCR_WIDTH) * (squareSize * (position.row - 1));

  shader.set2f("offset", offsetX, offsetY);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Piece::updateValidMoves() {
  if (type == "pawn") {
    validMoves = getPawnMoves(this, color);
  } else if (type == "bishop") {
    validMoves = getBishopMoves(this, color);
  } else if (type == "knight") {
    validMoves = getKnightMoves(this, color);
  } else if (type == "rook") {
    validMoves = getRookMoves(this, color);
  } else if (type == "queen") {
    validMoves = getQueenMoves(this, color);
  } else if (type == "king") {
    validMoves = getKingMoves(this, color);
  }

  std::cout << type << std::endl;

  for (int i = 0; i < validMoves.size(); i++) {
    std::cout << validMoves[i] << std::endl;
  }

  std::cout << std::endl;
}

white_pawn::white_pawn(std::string position) : Piece(position) {
  type = "pawn";
  color = 'W';
  value = 1;

  data = stbi_load("pieces/w_pawn_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // std::cout << "white_pawn generated at " << position << std::endl;
  } else {
    std::cout << "Failed to load white_pawn" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

white_bishop::white_bishop(std::string position) : Piece(position) {
  type = "bishop";
  color = 'W';
  value = 3;

  data =
      stbi_load("pieces/w_bishop_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load white_bishop" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

white_knight::white_knight(std::string position) : Piece(position) {
  type = "knight";
  color = 'W';
  value = 3;

  data =
      stbi_load("pieces/w_knight_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load white_knight" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

white_rook::white_rook(std::string position) : Piece(position) {
  type = "rook";
  color = 'W';
  value = 5;

  data = stbi_load("pieces/w_rook_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load white_rook" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

white_queen::white_queen(std::string position) : Piece(position) {
  type = "queen";
  color = 'W';
  value = 9;

  data = stbi_load("pieces/w_queen_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load white_queen" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

white_king::white_king(std::string position) : Piece(position) {
  type = "king";
  color = 'W';
  value = 0;

  data = stbi_load("pieces/w_king_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load white_king" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_pawn::black_pawn(std::string position) : Piece(position) {
  type = "pawn";
  color = 'B';
  value = 1;

  data = stbi_load("pieces/b_pawn_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_pawn" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_bishop::black_bishop(std::string position) : Piece(position) {
  type = "bishop";
  color = 'B';
  value = 3;

  data =
      stbi_load("pieces/b_bishop_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_bishop" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_knight::black_knight(std::string position) : Piece(position) {
  type = "knight";
  color = 'B';
  value = 3;

  data =
      stbi_load("pieces/b_knight_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_knight" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_rook::black_rook(std::string position) : Piece(position) {
  type = "rook";
  color = 'B';
  value = 5;

  data = stbi_load("pieces/b_rook_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_rook" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_queen::black_queen(std::string position) : Piece(position) {
  type = "queen";
  color = 'B';
  value = 9;

  data = stbi_load("pieces/b_queen_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_queen" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

black_king::black_king(std::string position) : Piece(position) {
  type = "king";
  color = 'B';
  value = 0;

  data = stbi_load("pieces/b_king_2x_ns.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load black_king" << std::endl;
  }

  stbi_image_free(data);

  shader.set1i("texture1", 0);
}

bool Piece::isPieceAt(char col, char row) {
  if (board->find(combineChars(col, row)) != board->end()) {
    return true;
  } else {
    return false;
  }
}

bool Piece::isPieceAt(char col, int row) {
  if (board->find(combineChars(col, (char)('a' + (row - 1)))) != board->end()) {
    return true;
  } else {
    return false;
  }
}

bool Piece::isPieceAt(std::string pos) {
  if (board->find(pos) != board->end()) {
    return true;
  } else {
    return false;
  }
}

bool Piece::isPieceAt(Position pos) { return isPieceAt(posToStr(pos)); }

Piece* Piece::getPieceAt(char col, char row) {
  std::string position = combineChars(col, row);

  if (!isPieceAt(position)) {
    return nullptr;
  }

  return (*board)[position];
}

Piece* Piece::getPieceAt(char col, int row) {
  return getPieceAt(col, (char)('a' + row - 1));
}

Piece* Piece::getPieceAt(std::string position) {
  if (!isPieceAt(position)) {
    return nullptr;
  }

  return (*board)[position];
}

Piece* Piece::getPieceAt(Position pos) { return getPieceAt(posToStr(pos)); }
