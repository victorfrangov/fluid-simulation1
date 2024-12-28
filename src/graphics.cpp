#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "graphics.h"
#include "logo.h"

#include <iostream>

Graphics::Graphics() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer("Fluid Simulation", globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_WINDOW_RESIZABLE, &this->_window, &this->_renderer);
    SDL_SetRenderLogicalPresentation(this->_renderer, globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_IOStream* io = SDL_IOFromMem(logo_png, logo_png_len);
    SDL_Surface* surface = IMG_Load_IO(io, 1);
    SDL_SetWindowIcon(this->_window, surface);
}

Graphics::~Graphics() {
    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
}

void Graphics::blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst) {
    SDL_RenderTexture(this->_renderer, p_texture, p_src, p_dst);
}

void Graphics::flip() {
    SDL_RenderPresent(this->_renderer);
}

void Graphics::clear() {
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->_renderer);
}

SDL_Renderer* Graphics::getRenderer() const {
    return this->_renderer;
}