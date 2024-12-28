#pragma once

#include "graphics.h"
#include "input.h"
#include "globals.h"
#include "logic.h"
#include <chrono>

class Fluid {
public:
	Fluid();
	~Fluid();
private:
	void gameLoop();

	void draw(Uint64 p_currentFPS, Uint64 p_elapsedTime);

	void update(Uint64 p_elapsedTime);

	void handleInput(Input &p_input);

	Graphics _graphics;
	bool _running;
	Logic _logic;
	bool _mouseHeld = false;
	Uint64 _mouseHoldStartTime = 0.0f;
};