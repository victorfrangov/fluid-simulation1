#include "input.h"

void Input::beginNewFrame() {
    this->_pressedKeys.clear();
    this->_releasedKeys.clear();
}

void Input::keyDownEvent(const SDL_Event& e) {
    this->_pressedKeys[e.key.scancode] = true;
    this->_heldKeys[e.key.scancode] = true;
}

void Input::keyUpEvent(const SDL_Event& e) {
    this->_releasedKeys[e.key.scancode] = true;
    this->_heldKeys[e.key.scancode] = false;
}

bool Input::wasKeyPressed(SDL_Scancode p_key) {
    return this->_pressedKeys[p_key];
}

bool Input::wasKeyReleased(SDL_Scancode p_key) {
    return this->_releasedKeys[p_key];
}

bool Input::isKeyHeld(SDL_Scancode p_key) {
    return this->_heldKeys[p_key];
}