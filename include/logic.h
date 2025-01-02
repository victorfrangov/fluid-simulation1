#pragma once

#include "graphics.h"

class Graphics;

class Logic {
public:

	Logic();
	~Logic();

	float getMouseX() { return this->_mouseX;  }
	float getMouseY() { return this->_mouseY;  }

	void draw(Graphics &graphics);
	void parseMousePos();
	void update(Uint64 dt);
	void addDensity(Uint64 dt);
	void diffuse(int b, float diff, float dt);
	void set_bnd(int b);
	void advect(int b, float dt);

private:
	float _mouseX = 0.0f;
	float _mouseY = 0.0f;
	float _p0[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _p[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _u[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _v[(g::N + 2) * (g::N + 2)] = { 0.0f };
};