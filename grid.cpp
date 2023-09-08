#include "grid.h"

Grid::Grid()
{
	border = 30.0f;
	squareSize = (SCR_WIDTH - (border * 2)) / 8.0f;

	for (int i = 0; i < 64; i++)
	{
		squares[i] =
			Rectangle(border + ((i % 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
				border + ((i / 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
				{ 0.0f, 0.0f, 0.0f });

		if (i % 2 == i / 8 % 2)
		{
			squares[i].color = brown;
		} else
		{
			squares[i].color = lightBrown;
		}
	}
}

Grid::Grid(glm::vec3 colorDark, glm::vec3 colorLight)
{
	border = 30.0f;
	squareSize = (SCR_WIDTH - (border * 2)) / 8.0f;

	for (int i = 0; i < 64; i++)
	{
		squares[i] =
			Rectangle(border + ((i % 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
				border + ((i / 8) * ((SCR_WIDTH - (border * 2)) / 8.0f)),
				{ 0.0f, 0.0f, 0.0f });

		if (i % 2 == i / 8 % 2)
		{
			squares[i].color = colorDark;
		} else
		{
			squares[i].color = colorLight;
		}
	}
}

void Grid::changeColor(glm::vec3 colorDark, glm::vec3 colorLight)
{
	for (int i = 0; i < 64; i++)
	{
		if (i % 2 == i / 8 % 2)
		{
			squares[i].color = colorDark;
		} else
		{
			squares[i].color = colorLight;
		}
	}
}

void Grid::draw()
{
	for (int i = 0; i < 64; i++)
	{
		squares[i].draw();
	}
}

int Grid::gridIndex(Position pos)
{
	return (pos.row - 1) * 8 + (pos.col - 'a');
}

bool Grid::lightSquare(Position pos)
{
	int i = gridIndex(pos);

	if (i % 2 == i / 8 % 2)
	{
		return false;
	} else
	{
		return true;
	}
}