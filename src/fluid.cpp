#include <SDL3/SDL.h>
#include <fluid.h>
#include <iostream>

namespace {
    const int FPS_TARGET = 60;
    constexpr int MAX_FRAME_TIME = 1000 / FPS_TARGET;

    unsigned int frameCount = 0;
    float currentFPS = 0.0f;
}

Fluid::Fluid() :
    _running(true),
    _graphics(this->_logic)
{
    this->gameLoop();
}

Fluid::~Fluid()
{}

void Fluid::gameLoop() {
    Input input;
    Uint64 lastUpdateTime = SDL_GetTicks();
    Uint64 lastFpsUpdateTime = lastUpdateTime;

    while (this->_running) {
        input.beginNewFrame();
        this->handleInput(input);

        const Uint64 currentTimeMs = SDL_GetTicks();
        Uint64 elapsedTimeMs = currentTimeMs - lastUpdateTime;

        frameCount++;
        if (currentTimeMs - lastFpsUpdateTime >= 1000) {
            currentFPS = frameCount / ((currentTimeMs - lastFpsUpdateTime) / 1000.0f);
            frameCount = 0;
            lastFpsUpdateTime = currentTimeMs;
        }

        this->update(elapsedTimeMs < MAX_FRAME_TIME ? elapsedTimeMs : MAX_FRAME_TIME);
        lastUpdateTime = currentTimeMs;

        this->draw(currentFPS, elapsedTimeMs);

        // Frame rate limiting
        Uint64 frameEndTime = SDL_GetTicks();
        int frameDuration = static_cast<int>(frameEndTime - currentTimeMs);
        if (frameDuration < MAX_FRAME_TIME) {
            SDL_Delay(MAX_FRAME_TIME - frameDuration);
        }
    }
}

void Fluid::draw(Uint64 currentFPS, Uint64 elapsedTime) {
    this->_graphics.clear();

    this->_graphics.draw();

    this->_graphics.flip();
}

void Fluid::update(Uint64 elapsedTime) {
    this->_logic.parseMousePos();
    if (_mouseHeld) {
        this->_logic.addDensity(static_cast<int>(elapsedTime));
        this->_logic.addVelocity(static_cast<int>(elapsedTime));
    }
    
    this->_logic.update(elapsedTime);
}

void Fluid::handleInput(Input &input) {
    SDL_Event e{};
    const SDL_MouseButtonEvent &mouse = e.button;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            input.keyDownEvent(e);
        }
        else if (e.type == SDL_EVENT_KEY_UP) {
            input.keyUpEvent(e);
        }
        else if (e.type == SDL_EVENT_QUIT) {
            this->_running = false;
            return;
        }

        if (!(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))) {
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && mouse.button == SDL_BUTTON_LEFT) {
                this->_mouseHeld = true;
            }
            else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && mouse.button == SDL_BUTTON_LEFT) {
                this->_mouseHeld = false;
            }

            //if (this->_mouseHeld) {
            //    this->_logic.parseMousePos();
            //    this->_logic.addDensity(MAX_FRAME_TIME);
            //    this->_logic.addVelocity(MAX_FRAME_TIME);
            //}
        }
        this->_graphics.handleEvent(e);
    }
}
