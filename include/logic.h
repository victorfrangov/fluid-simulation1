#pragma once

#include "graphics.h"

class Graphics;

class Logic {
public:

	Logic();
	~Logic();

	float getMouseX() { return this->_mouseX;  }
	float getMouseY() { return this->_mouseY;  }

	void draw(Graphics &p_graphics);
	void parseMousePos();
	void update(Uint64 p_dt);
	void addDensity(Uint64 p_dt);

private:
	float _mouseX = 0.0f;
	float _mouseY = 0.0f;
	float _densityGrid[globals::N * globals::N] = { 0.0f };
};