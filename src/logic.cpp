#include "logic.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <utility>

template<typename T1, typename T2>
constexpr int IX(T1 i, T2 j) { return (static_cast<int>(i) + (g::N + 2) * static_cast<int>(j)); }

static inline void swapBuffers(float*& a, float*& b) noexcept {
    std::swap(a, b);
}

static inline void clearDensityBoundaries(float* p) {
    for (int i = 0; i < g::N + 2; i++) {
        p[IX(i, 0)] = 0.0f;
        p[IX(i, g::N + 1)] = 0.0f;
        p[IX(0, i)] = 0.0f;
        p[IX(g::N + 1, i)] = 0.0f;
    }
}

Logic::Logic() = default;
Logic::~Logic() = default;

void Logic::update(Uint64 dt) {
    const float dtSeconds = static_cast<float>(dt) / 1000.0f;

    applyDensityDecay(dtSeconds);

    const float visc = _settings.viscosity;

    // vel step
    diffuse(1, _u, _u0, visc, static_cast<float>(dt));
    diffuse(2, _v, _v0, visc, static_cast<float>(dt));
    project(_u, _v, _u0, _v0);

    swapBuffers(_u0, _u);
    swapBuffers(_v0, _v);

    advect(1, _u, _u0, _u0, _v0, static_cast<float>(dt));
    advect(2, _v, _v0, _u0, _v0, static_cast<float>(dt));
    project(_u, _v, _u0, _v0);

    // dens step
    diffuse(0, _p, _p0, visc, static_cast<float>(dt));
    swapBuffers(_p0, _p);
    advect(0, _p, _p0, _u, _v, static_cast<float>(dt));
}

void Logic::applyDensityDecay(float dtSeconds) {
    const float decay = _settings.densityDecayPerSecond;
    if (decay <= 0.0f) {
        return;
    }

    const float factor = std::max(0.0f, 1.0f - decay * dtSeconds);

    // Decay BOTH buffers to avoid “swap bringing old density back”
    for (int j = 0; j < g::N + 2; j++) {
        for (int i = 0; i < g::N + 2; i++) {
            _p[IX(i, j)] *= factor;
            _p0[IX(i, j)] *= factor;
        }
    }
}

void Logic::reset() {
    std::fill(_pStore.begin(), _pStore.end(), 0.0f);
    std::fill(_p0Store.begin(), _p0Store.end(), 0.0f);
    std::fill(_uStore.begin(), _uStore.end(), 0.0f);
    std::fill(_u0Store.begin(), _u0Store.end(), 0.0f);
    std::fill(_vStore.begin(), _vStore.end(), 0.0f);
    std::fill(_v0Store.begin(), _v0Store.end(), 0.0f);

    clearDensityBoundaries(_p);
    clearDensityBoundaries(_p0);
}

void Logic::addDensity(int dt) {
    const float cellSize = static_cast<float>(g::SCREEN_SIZE) / static_cast<float>(g::N);

    int i = static_cast<int>(_mouseX / cellSize) + 1;
    int j = static_cast<int>((_mouseY - g::OFFSET) / cellSize) + 1;

    i = std::clamp(i, 1, g::N);
    j = std::clamp(j, 1, g::N);

    const float amount = static_cast<float>(dt) * _settings.densityAddScale;

    float& dst = _p0[IX(i, j)];
    dst = std::min(255.0f, dst + amount);

    clearDensityBoundaries(_p0);
}

void Logic::addVelocity(int dt) {
    const float cellSize = static_cast<float>(g::SCREEN_SIZE) / static_cast<float>(g::N);

    int i = static_cast<int>(_mouseX / cellSize) + 1;
    int j = static_cast<int>((_mouseY - g::OFFSET) / cellSize) + 1;

    i = std::clamp(i, 1, g::N);
    j = std::clamp(j, 1, g::N);

    const float amount = static_cast<float>(dt) * _settings.velocityAddScale;

    _u0[IX(i, j)] += amount;
    _v0[IX(i, j)] += amount;
}

void Logic::diffuse(int b, float* x, float* x0, float diff, float dt) {
    float a = dt / 1000.0f * diff * g::N * g::N;

    const int iters = std::max(1, _settings.diffuseIterations);
    for (int k = 0; k < iters; k++) {
        for (int j = 1; j <= g::N; j++) {
            for (int i = 1; i <= g::N; i++) {
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

void Logic::project(float* u, float* v, float* p, float* div) {
    const float h = 1.0f / g::N;

    for (int j = 1; j <= g::N; j++) {
        for (int i = 1; i <= g::N; i++) {
            div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
                                         v[IX(i, j + 1)] - v[IX(i, j - 1)]);
            p[IX(i, j)] = 0.0f;
        }
    }

    set_bnd(0, div);
    set_bnd(0, p);

    const int iters = std::max(1, _settings.projectIterations);
    for (int k = 0; k < iters; k++) {
        for (int j = 1; j <= g::N; j++) {
            for (int i = 1; i <= g::N; i++) {
                p[IX(i, j)] = (div[IX(i, j)] +
                               p[IX(i - 1, j)] + p[IX(i + 1, j)] +
                               p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4.0f;
            }
        }
        set_bnd(0, p);
    }

    for (int j = 1; j <= g::N; j++) {
        for (int i = 1; i <= g::N; i++) {
            u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
            v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
        }
    }

    set_bnd(1, u);
    set_bnd(2, v);
}

void Logic::set_bnd(int b, float* x) {
    for (int i = 1; i <= g::N; i++) {
        x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
        x[IX(g::N + 1, i)] = b == 1 ? -x[IX(g::N, i)] : x[IX(g::N, i)];
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, g::N + 1)] = b == 2 ? -x[IX(i, g::N)] : x[IX(i, g::N)];
    }
    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, g::N + 1)] = 0.5f * (x[IX(1, g::N + 1)] + x[IX(0, g::N)]);
    x[IX(g::N + 1, 0)] = 0.5f * (x[IX(g::N, 0)] + x[IX(g::N + 1, 1)]);
    x[IX(g::N + 1, g::N + 1)] = 0.5f * (x[IX(g::N, g::N + 1)] + x[IX(g::N + 1, g::N)]);
}

void Logic::advect(int b, float* d, float* d0, float* u, float* v, float dt) {
    float x, y, s0, t0, s1, t1;

    const float dt0 = (dt / 1000.0f) * g::N;

    for (int j = 1; j <= g::N; j++) {
        for (int i = 1; i <= g::N; i++) {
            x = i - dt0 * u[IX(i, j)];
            y = j - dt0 * v[IX(i, j)];

            if (x < 0.5f) x = 0.5f;
            if (x > g::N + 0.5f) x = g::N + 0.5f;

            const int i0 = static_cast<int>(x);
            const int i1 = i0 + 1;

            if (y < 0.5f) y = 0.5f;
            if (y > g::N + 0.5f) y = g::N + 0.5f;

            const int j0 = static_cast<int>(y);
            const int j1 = j0 + 1;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            d[IX(i, j)] =
                s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }

    set_bnd(b, d);
}

void Logic::parseMousePos() {
    float x = 0.0f;
    float y = 0.0f;
    SDL_GetMouseState(&x, &y);

    _mouseX = x;
    _mouseY = y;

    // Clamp to simulation draw area
    if (_mouseX < 0.0f) _mouseX = 0.0f;
    if (_mouseX > static_cast<float>(g::SCREEN_SIZE - 1)) _mouseX = static_cast<float>(g::SCREEN_SIZE - 1);

    if (_mouseY < static_cast<float>(g::OFFSET)) _mouseY = static_cast<float>(g::OFFSET);
    if (_mouseY > static_cast<float>(g::SCREEN_SIZE + g::OFFSET - 1)) _mouseY = static_cast<float>(g::SCREEN_SIZE + g::OFFSET - 1);
}