#include "position.h"

Position::Position() {
	col = 'a';
	row = 1;
}

Position::Position(char col, int row) {
	if (validPos(col, row)) {
		this->col = col;
		this->row = row;
	}
	else {
		this->col = -1;
		this->row = '\0';
	}
}

Position::Position(char col, char row) {
	int setRow = row - '0';

	if (validPos(col, setRow)) {
		this->col = col;
		this->row = setRow;
	}
	else {
		this->col = '\0';
		this->row = -1;
	}
}

Position::Position(std::string pos) {
	if (validPos(pos)) {
		this->col = pos[0];
		this->row = pos[1] - '0';
	}
	else {
		this->col = '\0';
		this->row = -1;
	}
}

bool Position::validPos(char col, int row) {
	if (col >= 'a' && col <= 'h' && row >= 1 && row <= 8) {
		return true;
	}
	else {
		return false;
	}
}

bool Position::validPos(std::string pos) {
	return validPos(pos[0], pos[1] - '0');
}

bool Position::isValid() { return validPos(col, row); }
