#include "grid.h"

Grid::Grid() {
  border = 30.0f;
  squareSize = (SCR_WIDTH - (border * 2)) / 8.0f;

  for (int i = 0; i < 64; i++) {
    squares[i] =
        Rectangle(border + ((i % 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
                  border + ((i / 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
                  {0.0f, 0.0f, 0.0f});

    if ((i % 2 == 0 && i / 8 % 2 == 0) || (i % 2 == 1 && i / 8 % 2 == 1)) {
      squares[i].color = brown;
    } else {
      squares[i].color = lightBrown;
    }
  }
}

void Grid::draw(std::unordered_map<std::string, Piece*> board) {
  for (int i = 0; i < 64; i++) {
    squares[i].draw();
  }

  for (char i = 'a'; i <= 'h'; i++) {
    for (int j = 1; j <= 8; j++) {
      std::string position = i + std::to_string(j);
      if (board.find(position) != board.end()) {
        board[position]->draw();
      }
    }
  }
}