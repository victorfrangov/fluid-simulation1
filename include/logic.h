#pragma once

#include "graphics.h"

class Graphics;

class Logic {
public:

	Logic();
	~Logic();

	void draw(Graphics &graphics);
	void parseMousePos();
	void update(Uint64 dt);
	void addDensity(int dt);
	void diffuse(int b, float* x, float* x0, float diff, float dt);
	void set_bnd(int b, float x[]);
	void advect(int b, float* d, float* d0, float* u, float* v, float dt);
	void addVelocity(int dt);
	void drawVelocity(Graphics& graphics) const;
	void project(float* u, float* v, float* p, float* div);
private:
	float _mouseX = 0.0f;
	float _mouseY = 0.0f;

	float _p0[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _p[(g::N + 2) * (g::N + 2)] = { 0.0f };

	float _u0[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _u[(g::N + 2) * (g::N + 2)] = { 0.0f };

	float _v0[(g::N + 2) * (g::N + 2)] = { 0.0f };
	float _v[(g::N + 2) * (g::N + 2)] = { 0.0f };
};