#pragma once

#include "graphics.h"


class Logic {
public:

	Logic();
	~Logic();

	float getMouseX() { return this->_mouseX;  }
	float getMouseY() { return this->_mouseY;  }

	void draw(Graphics &p_graphics);
	void parseMousePos();
	void drawGrid(Graphics &p_graphics);
	void update(Uint64 p_dt);
	void addDensity(Uint64 p_dt);

private:
	float _mouseX = NULL;
	float _mouseY = NULL;
	float _densityGrid[globals::N * globals::N] = { 0.0f };
};