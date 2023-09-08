#include "pieces.h"

Piece::Piece() : Piece('W', "pawn", (Position)"a1") {}

Piece::Piece(char color, std::string type, Position position)
{
	shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
	hasMoved = false;
	this->position = position;
	this->type = type;
	this->color = color;

	if (color == 'W')
	{
		otherColor = 'B';
		dir = 1;
	} else
	{
		otherColor = 'W';
		dir = -1;
	}

	forPromo = false;
	manualOffset = glm::vec2(0.0f);
	validMoves = std::set<Position>();

	if (type == "pawn")
	{
		value = 1;
	} else if (type == "bishop" || type == "knight")
	{
		value = 3;
	} else if (type == "rook")
	{
		value = 5;
	} else if (type == "queen")
	{
		value = 9;
	} else
	{
		value = 0;
	}

	setup();
}

Piece::Piece(char color, std::string type, bool forPromo, glm::vec2 offset)
{
	shader = Shader("vertexShader_pieces.vert", "fragmentShader_pieces.frag");
	hasMoved = false;
	this->position = Position('\0', -1);
	this->type = type;
	this->color = color;
	this->forPromo = forPromo;
	manualOffset = offset;

	if (type == "pawn")
	{
		value = 1;
	} else if (type == "bishop" || type == "knight")
	{
		value = 3;
	} else if (type == "rook")
	{
		value = 5;
	} else if (type == "queen")
	{
		value = 9;
	} else
	{
		value = 0;
	}
	setup();
}

void Piece::setup()
{
	float vertices[16];
	if (!forPromo)
	{
		// bottom left
		vertices[0] = (2.0f / SCR_WIDTH) * border - 1.0f;
		vertices[1] = (2.0f / SCR_HEIGHT) * border - 1.0f;

		// top left
		vertices[4] = (2.0f / SCR_HEIGHT) * border - 1.0f;
		vertices[5] = (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f;

		// bottom right
		vertices[8] = (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f;
		vertices[9] = (2.0f / SCR_HEIGHT) * border - 1.0f;

		// top right
		vertices[12] = (2.0f / SCR_WIDTH) * (border + squareSize) - 1.0f;
		vertices[13] = (2.0f / SCR_HEIGHT) * (border + squareSize) - 1.0f;
	} else
	{
		// bottom left
		vertices[0] = -1.0f;
		vertices[1] = -1.0f;

		// top left
		vertices[4] = -1.0f;
		vertices[5] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;

		// bottom right
		vertices[8] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
		vertices[9] = -1.0f;

		// top right
		vertices[12] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
		vertices[13] = (2.0f / SCR_HEIGHT) * squareSize - 1.0f;
	}

	// texture coords
	vertices[2] = 0.0f;
	vertices[3] = 0.0f;

	vertices[6] = 0.0f;
	vertices[7] = 1.0f;

	vertices[10] = 1.0f;
	vertices[11] = 0.0f;

	vertices[14] = 1.0f;
	vertices[15] = 1.0f;

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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
		(void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::string texturePath = "textures/pieces/"
		+ combineChars(color, '_') + type + "_2x_ns.png";

	data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		std::cout << "Failed to load " << color << " " << type << std::endl;
	}

	stbi_image_free(data);

	shader.use();
	shader.set1i("pieceTexture", 0);
}

void Piece::processPawn()
{
	// diagonals
	Position diagonal1 = add(position, -1, dir);
	visibleSquares->at(color).insert(diagonal1);

	if (*(kingPositions->at(otherColor)) == diagonal1)
	{
		(*piecesCheckingKing)++;
		(*checkActionSquares)[color].insert(position);
	}

	if (board->count(diagonal1) && board->at(diagonal1)->color == otherColor)
	{
		validMoves.insert(diagonal1);
	}

	Position diagonal2 = add(position, 1, dir);
	visibleSquares->at(color).insert(diagonal2);

	if (*(kingPositions->at(otherColor)) == diagonal2)
	{
		(*piecesCheckingKing)++;
		(*checkActionSquares)[color].insert(position);
	}

	if (board->count(diagonal2) && board->at(diagonal2)->color == otherColor)
	{
		validMoves.insert(diagonal2);
	}

	// in front
	Position inFront1 = add(position, 0, dir);
	if (!board->count(inFront1))
	{
		validMoves.insert(inFront1);

		if (!hasMoved)
		{
			Position inFront2 = add(position, 0, 2 * dir);

			if (!board->count(inFront2))
			{
				validMoves.insert(inFront2);
			}
		}
	}

	// en passant
	if (*enPassantPawn && (*enPassantPawn)->color == otherColor)
	{
		if ((*enPassantPawn)->position == add(position, -1, 0))
		{
			validMoves.insert(diagonal1);
		}

		if ((*enPassantPawn)->position == add(position, 1, 0))
		{
			validMoves.insert(diagonal2);
		}
	}
}

void Piece::processBishop()
{
	for (int i = -1; i <= 1; i += 2)
	{
		for (int j = -1; j <= 1; j += 2)
		{
			bool checkForPin = false;
			Position pinPosition;

			// go in direction (i, j) until not on board
			for (Position checkPosition = add(position, i, j);
				checkPosition.isValid();
				checkPosition = add(checkPosition, i, j))
			{
				if (!checkForPin)
				{
					// [not checking for pin] If piece at position:
					//     a. If opposite color, add checkPosition to valid moves and
					//        visible squares, and start checking for pin
					//     b. If same color, add checkPosition to visible squares
					//     c. If empty, add checkPosition to valid moves and visible squares
					if (board->count(checkPosition))
					{
						if (board->at(checkPosition)->color == otherColor)
						{ // a.
							validMoves.insert(checkPosition);
							visibleSquares->at(color).insert(checkPosition);

							// Start checking for pin on next iteration of loop
							pinPosition = checkPosition;
							checkForPin = true;

							// Detect if king is in check
							if (checkPosition == *(kingPositions->at(otherColor)))
							{
								(*piecesCheckingKing)++;

								// Attack squares are all "checkPosition"s so far (excluding
								// king position), as well as this->position
								for (Position attackSquare = add(checkPosition, -i, -j);
									attackSquare != add(position, -i, -j);
									attackSquare = add(attackSquare, -i, -j))
								{
									(*checkActionSquares)[color].insert(attackSquare);
								}
							}

							continue;
						} else
						{ // b.
							visibleSquares->at(color).insert(checkPosition);
							break;
						}
					}

					// c.
					validMoves.insert(checkPosition);
					visibleSquares->at(color).insert(checkPosition);
				} else if (checkForPin && board->count(checkPosition)
					&& board->at(checkPosition)->type == "king"
					&& board->at(checkPosition)->color == otherColor)
				{
					board->at(pinPosition)->pinned = true;
					break;
				}
			}
		}
	}
}

void Piece::processKnight()
{
	// When added to knight position, these give its visible squares
	std::vector<int> colDirs = { -2, -2, 2, 2, -1, 1, -1, 1 };
	std::vector<int> rowDirs = { -1, 1, -1, 1, -2, -2, 2, 2 };

	for (int k = 0; k < colDirs.size(); k++)
	{
		int i = colDirs[k];
		int j = rowDirs[k];

		Position checkPosition = add(position, i, j);

		if (checkPosition.isValid())
		{
			// If no piece in checkPosition, or piece of opposite color, add to valid moves
			if (!board->count(checkPosition)
				|| (board->count(checkPosition)
					&& board->at(checkPosition)->color == otherColor))
			{
				validMoves.insert(checkPosition);
			}

			// Add to visible squares regardless
			visibleSquares->at(color).insert(checkPosition);

			if (checkPosition == *(kingPositions->at(otherColor)))
			{
				(*piecesCheckingKing)++;
				(*checkActionSquares)[color].insert(position);
			}
		}
	}
}

void Piece::processRook()
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((i && j) || (!i && !j))
			{
				continue;
			}

			bool checkForPin = false;
			Position pinPosition;

			// go in direction (i, j) until not on board
			for (Position checkPosition = add(position, i, j);
				checkPosition.isValid();
				checkPosition = add(checkPosition, i, j))
			{
				if (!checkForPin)
				{
					// [not checking for pin] If piece at position:
					//     a. If opposite color, add checkPosition to valid moves and
					//        visible squares, and start checking for pin
					//     b. If same color, add checkPosition to visible squares
					//     c. If empty, add checkPosition to valid moves and visible squares
					if (board->count(checkPosition))
					{
						if (board->at(checkPosition)->color == otherColor)
						{ // a.
							validMoves.insert(checkPosition);
							visibleSquares->at(color).insert(checkPosition);

							// Start checking for pin on next iteration of loop
							pinPosition = checkPosition;
							checkForPin = true;

							// Detect if king is in check
							if (checkPosition == *(kingPositions->at(otherColor)))
							{
								(*piecesCheckingKing)++;

								// Attack squares are all "checkPosition"s so far (excluding
								// king position), as well as this->position
								for (Position attackSquare = add(checkPosition, -i, -j);
									attackSquare != add(position, -i, -j);
									attackSquare = add(attackSquare, -i, -j))
								{
									(*checkActionSquares)[color].insert(attackSquare);
								}
							}

							continue;
						} else
						{ // b.
							visibleSquares->at(color).insert(checkPosition);
							break;
						}
					}

					// c.
					validMoves.insert(checkPosition);
					visibleSquares->at(color).insert(checkPosition);
				} else if (checkForPin && board->count(checkPosition)
					&& board->at(checkPosition)->type == "king"
					&& board->at(checkPosition)->color == otherColor)
				{
					board->at(pinPosition)->pinned = true;
					break;
				}
			}
		}
	}
}

void Piece::processQueen()
{
	processBishop();
	processRook();
}

void Piece::processKing()
{
	// !! Function does not invalidate moves into check.
	for (int j = -1; j <= 1; j++)
	{
		for (int i = -1; i <= 1; i++)
		{
			if (!j && !i)
			{
				continue;
			}

			Position checkPosition = add(position, i, j);

			if (checkPosition.isValid())
			{
				// If no piece in checkPosition, or piece of opposite color, add to valid moves
				if (!board->count(checkPosition)
					|| (board->count(checkPosition)
						&& board->at(checkPosition)->color == otherColor))
				{
					validMoves.insert(checkPosition);
				}

				// Add to visible squares regardless
				visibleSquares->at(color).insert(checkPosition);
			}
		}
	}

	// castling
	Position potentialRookFileA = add(position, -4, 0);
	if (!hasMoved && board->count(potentialRookFileA)
		&& !board->at(potentialRookFileA)->hasMoved
		&& !board->count(add(position, -1, 0))
		&& !visibleSquares->at(otherColor).count(add(position, -1, 0))
		&& !board->count(add(position, -2, 0))
		&& !visibleSquares->at(otherColor).count(add(position, -2, 0))
		&& !board->count(add(position, -3, 0)))
	{
		validMoves.insert(add(position, -2, 0));
	}

	Position potentialRookFileH = add(position, 3, 0);
	if (!hasMoved && board->count(potentialRookFileH)
		&& !board->at(potentialRookFileH)->hasMoved
		&& !board->count(add(position, 1, 0))
		&& !visibleSquares->at(otherColor).count(add(position, 1, 0))
		&& !board->count(add(position, 2, 0))
		&& !visibleSquares->at(otherColor).count(add(position, 2, 0)))
	{
		validMoves.insert(add(position, 2, 0));
	}
}

void Piece::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader.use();

	if (!forPromo)
	{
		float offsetX = (2.0f / SCR_WIDTH) * (squareSize * (position.col - 'a'));
		float offsetY = (2.0f / SCR_HEIGHT) * (squareSize * (position.row - 1));
		shader.set2f("offset", offsetX, offsetY);
	} else
	{
		float offsetX = (2.0f / SCR_WIDTH) * manualOffset.x;
		float offsetY = (2.0f / SCR_HEIGHT) * manualOffset.y;
		shader.set2f("offset", offsetX, offsetY);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Piece::process()
{
	if (!pinned)
	{
		if (type == "pawn")
		{
			processPawn();
		} else if (type == "bishop")
		{
			processBishop();
		} else if (type == "knight")
		{
			processKnight();
		} else if (type == "rook")
		{
			processRook();
		} else if (type == "queen")
		{
			processQueen();
		} else if (type == "king")
		{
			processKing();
		}

		// If king in check, pieces can only block checking piece or take it -
		// these positions are "checkActionSquares"
		// (only applies to non-king "turn" pieces)
		if (type != "king" && *kingInCheck)
		{
			for (auto it = validMoves.begin(); it != validMoves.end();)
			{
				if (!checkActionSquares->at(otherColor).count(*it))
				{
					validMoves.erase(it++);
				} else
				{
					it++;
				}
			}
		}
	}
}

void Piece::invalidateMoveIntoCheck()
{
	if (type != "king")
	{
		return;
	}

	// King cannot move into any "visible squares" of opposite color pieces
	for (auto it = validMoves.begin(); it != validMoves.end();)
	{
		if (visibleSquares->at(otherColor).count(*it))
		{
			validMoves.erase(it++);
		} else
		{
			it++;
		}
	}
}
