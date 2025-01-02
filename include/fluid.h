#pragma once

#include "graphics.h"
#include "input.h"
#include "globals.h"
#include "logic.h"

class Fluid {
public:
	Fluid();
	~Fluid();
private:
	void gameLoop();

	void draw(Uint64 currentFPS, Uint64 elapsedTime);

	void update(Uint64 elapsedTime);

	void handleInput(Input &input);

	Graphics _graphics;
	bool _running;
	Logic _logic;
	bool _mouseHeld = false;
	Uint64 _mouseHoldStartTime = 0.0f;
};