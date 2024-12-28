#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include <map>

class Input {
public:
    void beginNewFrame();
    void keyUpEvent(const SDL_Event& e);
    void keyDownEvent(const SDL_Event& e);
    bool wasKeyPressed(SDL_Scancode p_key);
    bool wasKeyReleased(SDL_Scancode p_key);
    bool isKeyHeld(SDL_Scancode p_key);

private:
    std::map<SDL_Scancode, bool> _heldKeys;
    std::map<SDL_Scancode, bool> _pressedKeys;
    std::map<SDL_Scancode, bool> _releasedKeys;
};

#endif