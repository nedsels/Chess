#ifndef GRID_H
#define GRID_H

#include "pieces.h"
#include "shapes.h"

class Grid
{
public:
	Grid();
	Grid(glm::vec3 colorDark, glm::vec3 colorLight);

	Rectangle squares[64];

	void draw();
	void changeColor(glm::vec3 colorDark, glm::vec3 colorLight);
	int gridIndex(Position pos);
	bool lightSquare(Position pos);
};

#endif