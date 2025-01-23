#include "logic.h"
#include <SDL3/SDL.h>
#if TARGET_OS_MAC
#include <math.h>
#endif
#include <algorithm>
#include <iostream>

template<typename T1, typename T2>
constexpr int IX(T1 i, T2  j) { return (static_cast<int>(i) + (g::N + 2) * static_cast<int>(j)); }

#define swap(x0, x) { std::copy(x, x + (g::N + 2) * (g::N + 2), x0); }

Logic::Logic()
{}

Logic::~Logic()
{}

void Logic::draw(Graphics &graphics) {
    SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 255, 255, 255);

    //SDL_RenderLine(graphics.getRenderer(), g::GRID_SIZE, 0, g::GRID_SIZE, g::SCREEN_SIZE + g::OFFSET); // vertical left
    //SDL_RenderLine(graphics.getRenderer(), g::SCREEN_SIZE - g::GRID_SIZE, 0, g::SCREEN_SIZE - g::GRID_SIZE, g::SCREEN_SIZE + g::OFFSET); // vertical right
    //SDL_RenderLine(graphics.getRenderer(), 0, g::GRID_SIZE + g::OFFSET, g::SCREEN_SIZE, g::GRID_SIZE + g::OFFSET); // horitzontal top
    //SDL_RenderLine(graphics.getRenderer(), 0, g::SCREEN_SIZE + g::OFFSET - g::GRID_SIZE, g::SCREEN_SIZE, g::SCREEN_SIZE + g::OFFSET - g::GRID_SIZE); // horitzontal bottom

    if (this->_mouseX >= 0.0f && this->_mouseY >= 0.0f) {
        SDL_SetRenderDrawBlendMode(graphics.getRenderer(), SDL_BLENDMODE_BLEND);
        for (int a = 0; a < (g::N + 2) * (g::N + 2); a++) {
            if (this->_p0[a]) {
                SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 0, 0, this->_p0[a]);
                int newJ = floor(a / (g::N + 2)); // vertical pos
                int newI = a - newJ * (g::N + 2); // horizontal pos

                SDL_FRect rect{};
                rect.x = newI * g::GRID_SIZE + 1;
                rect.y = newJ * g::GRID_SIZE + g::OFFSET + 1;
                rect.w = g::GRID_SIZE - 1;
                rect.h = g::GRID_SIZE - 1;

                SDL_RenderRect(graphics.getRenderer(), &rect);
                SDL_RenderFillRect(graphics.getRenderer(), &rect);
            }
        }
    }
    this->drawVelocity(graphics);
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
    const float visc = 0.001f;
    //vel step
    this->diffuse(1, this->_u, this->_u0, visc, dt);
    this->diffuse(2, this->_v, this->_v0, visc, dt);
    this->project(this->_u, this->_v, this->_u0, this->_v0);

    swap(this->_u0, this->_u);
    swap(this->_v0, this->_v);

    this->advect(1, this->_u, this->_u0, this->_u0, this->_v0, dt);
    this->advect(2, this->_v, this->_v0, this->_u0, this->_v0, dt);
    this->project(this->_u, this->_v, this->_u0, this->_v0);

    //dens step
    this->diffuse(0, this->_p, this->_p0, visc, dt); swap(this->_p0, this->_p);
    this->advect(0, this->_p, this->_p0, this->_u, this->_v, dt);
}

void Logic::addDensity(int dt) { // TIME NOT NEEDED??? because right now, it is related to the frame_time aka the fps target im setting.
    float p = dt / 1.0f;

    // add magnitude here add i + (1 * magnitude)
    float i = floor(this->_mouseX / g::GRID_SIZE);
    float j = floor((this->_mouseY - g::OFFSET) / g::GRID_SIZE);

    if (this->_p0[IX(i, j)] + p > 255) {
        this->_p0[IX(i, j)] = 255;
    }
    else {
        this->_p0[IX(i, j)] += p;
    }
}

void Logic::diffuse(int b, float* x, float* x0, float diff, float dt) {
    float a = dt / 1000.0f * diff * g::N * g::N;

    for (int k = 0; k < 5; k++) {
        for (int i = 1; i < g::N; i++) {
            for (int j = 1; j < g::N; j++) {
                x[IX(i, j)] = (x0[IX(i, j)] + 
                    a * (x[IX(i - 1, j)] + 
                         x[IX(i + 1, j)] +
                         x[IX(i, j - 1)] + 
                         x[IX(i, j + 1)])
                        ) / (1 + 4 * a);
            }
        }
        set_bnd(b, x);
    }
}

void Logic::set_bnd(int b, float x[]) {
    for (int i = 1; i <= g::N; i++) {
        x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
        x[IX(g::N + 1, i)] = b == 1 ? -x[IX(g::N, i)] : x[IX(g::N, i)];
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, g::N + 1)] = b == 2 ? -x[IX(i, g::N)] : x[IX(i, g::N)];
    }
    x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, g::N + 1)] = 0.5 * (x[IX(1, g::N + 1)] + x[IX(0, g::N)]);
    x[IX(g::N + 1, 0)] = 0.5 * (x[IX(g::N, 0)] + x[IX(g::N + 1, 1)]);
    x[IX(g::N + 1, g::N + 1)] = 0.5 * (x[IX(g::N, g::N + 1)] + x[IX(g::N + 1, g::N)]);
}

void Logic::advect(int b, float* d, float* d0, float* u, float* v, float dt) {
    int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt * g::N;
    for (int i = 1; i <= g::N; i++) {
        for (int j = 1; j <= g::N; j++) {
            x = i - dt0 * u[IX(i, j)]; y = j - dt0 * v[IX(i, j)];
            if (x < 0.5) x = 0.5;
            if (x > g::N + 0.5) x = g::N + 0.5;
            i0 = (int)x; i1 = i0 + 1;
            if (y < 0.5) y = 0.5;
            if (y > g::N + 0.5) y = g::N + 0.5;
            j0 = (int)y; j1 = j0 + 1;
            s1 = x - i0; s0 = 1 - s1;
            t1 = y - j0; t0 = 1 - t1;
            d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }
    set_bnd(b, d);
}

void Logic::addVelocity(int dt) {
    float amountX = dt;
    float amountY = dt;
    int i = static_cast<int>(this->_mouseX / g::GRID_SIZE);
    int j = static_cast<int>((this->_mouseY - g::OFFSET) / g::GRID_SIZE);

    if (i >= 1 && i <= g::N - 1 && j >= 1 && j <= g::N - 1) {
        _u0[IX(i, j)] += amountX;
        _v0[IX(i, j)] += amountY; //this will always add velocity towards the bottom right not towards where the mouse is moving
        //take the previous location of the mouse (0.1s ago) to create a vector
    }
}

void Logic::drawVelocity(Graphics& graphics) const {
    SDL_SetRenderDrawColor(graphics.getRenderer(), 0, 255, 0, 255); // Green color for velocity vectors

    for (int i = 1; i <= g::N; i++) {
        for (int j = 1; j <= g::N; j++) {
            float x = (i * g::GRID_SIZE) + g::GRID_SIZE / 2;
            float y = (j * g::GRID_SIZE + g::OFFSET) + g::GRID_SIZE / 2; // Offset everything to be in the center of the cell
            float u = _u[IX(i, j)];
            float v = _v[IX(i, j)];

            SDL_RenderLine(graphics.getRenderer(),
                x,
                y,
                x + u * 10,
                y + v * 10);
        }
    }
}

void Logic::project(float* u, float* v, float* p, float* div) {
    int i, j, k;
    float h;
    h = 1.0f / g::N;

    for (i = 1; i <= g::N; i++) {
        for (j = 1; j <= g::N; j++) {
            div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
                v[IX(i, j + 1)] - v[IX(i, j - 1)]);
            p[IX(i, j)] = 0;
        }
    }
    set_bnd(0, div); set_bnd(0, p);
    for (k = 0; k < 5; k++) {
        for (i = 1; i <= g::N; i++) {
            for (j = 1; j <= g::N; j++) {
                p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
                    p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
            }
        }
        set_bnd(0, p);
    }
    for (i = 1; i <= g::N; i++) {
        for (j = 1; j <= g::N; j++) {
            u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
            v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
        }
    }
    set_bnd(1, u); set_bnd(2, v);
}