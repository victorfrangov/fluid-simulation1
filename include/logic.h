#pragma once

#include "globals.h"

#include <SDL3/SDL.h>
#include <vector>

class Graphics;

class Logic {
public:
    struct Settings {
        float viscosity = 0.001f;
        int diffuseIterations = 20;
        int projectIterations = 10;
        float densityAddScale = 1.0f;
        float velocityAddScale = 1.0f;
        float densityDecayPerSecond = 0.25f;
        bool drawVelocity = false;
        int velocityStride = 8;
        float velocityScale = 10.0f;
        float brightness = 6.0f;
        float gamma = 0.6f;
    };

    Logic();
    ~Logic();

    void parseMousePos();
    void update(Uint64 dt);

    void addDensity(int dt);
    void addVelocity(int dt);

    void diffuse(int b, float* x, float* x0, float diff, float dt);
    void set_bnd(int b, float* x);
    void advect(int b, float* d, float* d0, float* u, float* v, float dt);
    void project(float* u, float* v, float* p, float* div);

    const float* getDensity() const noexcept { return _p; }

    Settings& settings() noexcept { return _settings; }
    const Settings& settings() const noexcept { return _settings; }

    void reset();

private:
    void applyDensityDecay(float dtSeconds);

    Settings _settings{};

    float _mouseX = 0.0f;
    float _mouseY = 0.0f;

    int _side = g::N + 2;
    int _count = (g::N + 2) * (g::N + 2);

    std::vector<float> _pStore;
    std::vector<float> _p0Store;
    std::vector<float> _uStore;
    std::vector<float> _u0Store;
    std::vector<float> _vStore;
    std::vector<float> _v0Store;

    float* _p = nullptr;
    float* _p0 = nullptr;
    float* _u = nullptr;
    float* _u0 = nullptr;
    float* _v = nullptr;
    float* _v0 = nullptr;
};