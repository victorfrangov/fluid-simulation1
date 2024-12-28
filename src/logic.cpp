#include "logic.h"
#include <SDL3/SDL.h>
#include <iostream>

template<typename T1, typename T2>
constexpr auto IX(T1 i, T2  j) { return ((i) + (globals::N) * (j)); }

Logic::Logic()
{}

Logic::~Logic()
{}

void Logic::draw(Graphics &p_graphics) {
    // Set the draw color to white
    SDL_SetRenderDrawColor(p_graphics.getRenderer(), 255, 255, 255, 255);

    for (int x = 0; x <= globals::SCREEN_SIZE; x += globals::GRID_SIZE) { //vertical lines
        SDL_RenderLine(p_graphics.getRenderer(), x, 0, x, globals::SCREEN_SIZE);
    }

    for (int y = 0; y <= globals::SCREEN_SIZE; y += globals::GRID_SIZE) { //horizontal lines
        SDL_RenderLine(p_graphics.getRenderer(), 0, y, globals::SCREEN_SIZE, y);
    }

    if (this->_mouseX != NULL && this->_mouseY != NULL) {
        this->drawGrid(p_graphics);
    }
}

void Logic::parseMousePos() {
    SDL_GetMouseState(&this->_mouseX, &this->_mouseY);
}

void Logic::drawGrid(Graphics &p_graphics) {
    SDL_SetRenderDrawBlendMode(p_graphics.getRenderer(), SDL_BLENDMODE_BLEND);
        for (int a = 0; a < globals::N * globals::N; a++) {
            if (this->_densityGrid[a]) {
                SDL_SetRenderDrawColor(p_graphics.getRenderer(), 255, 0, 0, this->_densityGrid[a]);
                int newJ = floor(a / globals::N); // vertical pos
                int newI = a - newJ * globals::N; // horizontal pos

                SDL_FRect rect;
                rect.x = newI * globals::GRID_SIZE + 1;
                rect.y = newJ * globals::GRID_SIZE + 1;
                rect.w = globals::GRID_SIZE - 1;
                rect.h = globals::GRID_SIZE - 1;

                SDL_RenderRect(p_graphics.getRenderer(), &rect);
                SDL_RenderFillRect(p_graphics.getRenderer(), &rect);
            }
        }
}

void Logic::update(Uint64 p_dt) {

}

void Logic::addDensity(Uint64 p_dt) {
    float p = p_dt / 1.0f;

    if (p > 255.0f) {
        p = 255.0f;
    }    
    float i = floor(static_cast<int>(this->_mouseX) / globals::GRID_SIZE);
    float j = floor(static_cast<int>(this->_mouseY) / globals::GRID_SIZE);

    if (this->_densityGrid[static_cast<int>(IX(i, j))] + p > 255) {
        this->_densityGrid[static_cast<int>(IX(i, j))] = 255;
    }
    else {
        this->_densityGrid[static_cast<int>(IX(i, j))] += p;
    }
}