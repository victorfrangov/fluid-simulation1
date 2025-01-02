#include "logic.h"
#include <SDL3/SDL.h>
#if TARGET_OS_MAC
#include <math.h>
#endif
#include <algorithm>

template<typename T1, typename T2>
constexpr auto IX(T1 i, T2  j) { return ((i) + (g::N + 2) * (j)); }

Logic::Logic()
{}

Logic::~Logic()
{}

void Logic::draw(Graphics &graphics) {
    SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 255, 255, 255);

    /*
    for (int x = 0; x <= g::SCREEN_SIZE; x += g::GRID_SIZE) { // vertical lines
        SDL_RenderLine(graphics.getRenderer(), x, 0, x, g::SCREEN_SIZE + g::OFFSET);
    }

    for (int y = g::OFFSET; y <= g::SCREEN_SIZE + g::OFFSET; y += g::GRID_SIZE) { // horizontal lines
        SDL_RenderLine(graphics.getRenderer(), 0, y, g::SCREEN_SIZE, y);
    }
    */

    SDL_RenderLine(graphics.getRenderer(), g::GRID_SIZE, 0, g::GRID_SIZE, g::SCREEN_SIZE + g::OFFSET); // vertical left
    SDL_RenderLine(graphics.getRenderer(), g::SCREEN_SIZE - g::GRID_SIZE, 0, g::SCREEN_SIZE - g::GRID_SIZE, g::SCREEN_SIZE + g::OFFSET); // vertical right
    SDL_RenderLine(graphics.getRenderer(), 0, g::GRID_SIZE + g::OFFSET, g::SCREEN_SIZE, g::GRID_SIZE + g::OFFSET); // horitzontal top
    SDL_RenderLine(graphics.getRenderer(), 0, g::SCREEN_SIZE + g::OFFSET - g::GRID_SIZE, g::SCREEN_SIZE, g::SCREEN_SIZE + g::OFFSET - g::GRID_SIZE); // horitzontal bottom

    if (this->_mouseX >= 0 && this->_mouseY >= 0) {
        SDL_SetRenderDrawBlendMode(graphics.getRenderer(), SDL_BLENDMODE_BLEND);
        for (int a = 0; a < (g::N + 2) * (g::N + 2); a++) {
            if (this->_p0[a]) {
                SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 0, 0, this->_p0[a]);
                int newJ = floor(a / (g::N + 2)); // vertical pos
                int newI = a - newJ * (g::N + 2); // horizontal pos

                SDL_FRect rect;
                rect.x = newI * g::GRID_SIZE + 1;
                rect.y = newJ * g::GRID_SIZE + g::OFFSET + 1;
                rect.w = g::GRID_SIZE - 1;
                rect.h = g::GRID_SIZE - 1;

                SDL_RenderRect(graphics.getRenderer(), &rect);
                SDL_RenderFillRect(graphics.getRenderer(), &rect);
            }
        }
    }
}

void Logic::parseMousePos() {
    SDL_GetMouseState(&_mouseX, &_mouseY);
    this->_mouseX = static_cast<float>(_mouseX);
    this->_mouseY = static_cast<float>(_mouseY);

    //Boudaries
    if (this->_mouseX <= 0) this->_mouseX = 1;
    if (this->_mouseX >= g::SCREEN_SIZE) this->_mouseX = g::SCREEN_SIZE - 1;
    if (this->_mouseY < g::OFFSET) this->_mouseY = g::OFFSET;
    if (this->_mouseY >= g::SCREEN_SIZE + g::OFFSET) this->_mouseY = g::SCREEN_SIZE + g::OFFSET - 1;
}

void Logic::update(Uint64 dt) {
    this->diffuse(0, 0.001f, dt);
    this->advect(0, dt);
}

void Logic::addDensity(Uint64 dt) {
    float p = dt / 1.0f;

    if (p > 255.0f) { // not necessary
        p = 255.0f;
    }
    // add magnitude here add i + (1 * magnitude)
    float i = floor(static_cast<int>(this->_mouseX) / g::GRID_SIZE);
    float j = floor(static_cast<int>(this->_mouseY - g::OFFSET) / g::GRID_SIZE);

    if (this->_p0[static_cast<int>(IX(i, j))] + p > 255) {
        this->_p0[static_cast<int>(IX(i, j))] = 255;
    }
    else {
        this->_p0[static_cast<int>(IX(i, j))] += p;
    }
}

void Logic::diffuse(int b, float diff, float dt) {
    float a = dt / 1000.0f * diff * g::N * g::N;
    for (int k = 0; k < 5; k++) {
        for (int i = 1; i < g::N; i++) {
            for (int j = 1; j < g::N; j++) {
                _p[IX(i, j)] = (_p0[IX(i, j)] + 
                    a * (_p[IX(i - 1, j)] + 
                         _p[IX(i + 1, j)] +
                         _p[IX(i, j - 1)] + 
                         _p[IX(i, j + 1)])
                        ) / (1 + 4 * a);
            }
        }
        set_bnd(b);
    }
    std::copy(std::begin(this->_p), std::end(this->_p), std::begin(this->_p0));
}

void Logic::set_bnd(int b) {
    for (int i = 1; i <= g::N; i++) {
        _p[IX(0, i)] = b == 1 ? -_p[IX(1, i)] : _p[IX(1, i)];
        _p[IX(g::N + 1, i)] = b == 1 ? -_p[IX(g::N, i)] : _p[IX(g::N, i)];
        _p[IX(i, 0)] = b == 2 ? -_p[IX(i, 1)] : _p[IX(i, 1)];
        _p[IX(i, g::N + 1)] = b == 2 ? -_p[IX(i, g::N)] : _p[IX(i, g::N)];
    }
    _p[IX(0, 0)] = 0.5 * (_p[IX(1, 0)] + _p[IX(0, 1)]);
    _p[IX(0, g::N + 1)] = 0.5 * (_p[IX(1, g::N + 1)] + _p[IX(0, g::N)]);
    _p[IX(g::N + 1, 0)] = 0.5 * (_p[IX(g::N, 0)] + _p[IX(g::N + 1, 1)]);
    _p[IX(g::N + 1, g::N + 1)] = 0.5 * (_p[IX(g::N, g::N + 1)] + _p[IX(g::N + 1, g::N)]);
}

void Logic::advect(int b, float dt) {
    int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt * g::N;
    for (int i = 1; i <= g::N; i++) {
        for (int j = 1; j <= g::N; j++) {
            x = i - dt0 * _u[IX(i, j)];
            y = j - dt0 * _v[IX(i, j)];
            if (x < 0.5) x = 0.5;
            if (x > g::N + 0.5)
                x = g::N + 0.5;
            i0 = (int)x; i1 = i0 + 1;
            if (y < 0.5) y = 0.5;
            if (y > g::N + 0.5) y = g::N + 0.5;
            j0 = (int)y;
            j1 = j0 + 1;
            s1 = x - i0;
            s0 = 1 - s1;
            t1 = y - j0;
            t0 = 1 - t1;
            _p[IX(i, j)] = s0 * (t0 * _p0[IX(i0, j0)] + t1 * _p0[IX(i0, j1)]) +
                s1 * (t0 * _p0[IX(i1, j0)] + t1 * _p0[IX(i1, j1)]);
        }
    }
    set_bnd(b);
}
