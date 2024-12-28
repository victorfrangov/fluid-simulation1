#include <SDL3/SDL.h>
#include <fluid.h>

namespace {
    const int FPS_TARGET = 600;
    constexpr int MAX_FRAME_TIME = 1000 / FPS_TARGET;

    unsigned int frameCount = 0;
    float currentFPS = 0.0f;
}

Fluid::Fluid() :
    _running(true)
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

        this->update((((static_cast<float>(elapsedTimeMs)) < (MAX_FRAME_TIME)) ? (static_cast<float>(elapsedTimeMs)) : (MAX_FRAME_TIME)));
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

void Fluid::draw(Uint64 p_currentFPS, Uint64 p_elapsedTime) {
    this->_graphics.clear();

    this->_logic.draw(this->_graphics);

    this->_graphics.flip();
}

void Fluid::update(Uint64 p_elapsedTime) {
    if (_mouseHeld) {
        this->_logic.addDensity(p_elapsedTime);
    }
}

void Fluid::handleInput(Input &p_input) {
    SDL_Event e;
    const SDL_MouseButtonEvent &mouse = e.button;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            p_input.keyDownEvent(e);
        }
        else if (e.type == SDL_EVENT_KEY_UP) {
            p_input.keyUpEvent(e);
        }
        else if (e.type == SDL_EVENT_QUIT) {
            this->_running = false;
            return;
        }
        /*else if (e.type == SDL_EVENT_MOUSE_MOTION && mouse.button == SDL_BUTTON_LEFT) {
            this->_logic.parseMousePos();
            this->_logic.addDensity((mouse.timestamp / 1e+6) - (SDL_GetTicks()));
        }*/
        /*else if (mouse.button == SDL_BUTTON_LEFT) {
            this->_logic.parseMousePos();
        }*/
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && mouse.button == SDL_BUTTON_LEFT) {
            this->_mouseHeld = true;
            this->_mouseHoldStartTime = SDL_GetTicks();
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && mouse.button == SDL_BUTTON_LEFT) {
            this->_mouseHeld = false;
        }

        if (this->_mouseHeld) {
            Uint64 currentTime = SDL_GetTicks();
            Uint64 dt = currentTime - this->_mouseHoldStartTime;
            this->_logic.parseMousePos();
            this->_logic.addDensity(dt);
            this->_mouseHoldStartTime = currentTime;
        }
    }

    if (p_input.wasKeyPressed(SDL_SCANCODE_Q)) {
        this->_running = false;
        return;
    }
}
