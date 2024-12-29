#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "graphics.h"
#include "logo.h"
#include "imguiBackend.h"

Graphics::Graphics() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer("Fluid Simulation", globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &this->_window, &this->_renderer);
    SDL_SetRenderLogicalPresentation(this->_renderer, globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_IOStream* io_image = SDL_IOFromMem(logo_png, logo_png_len);
    SDL_Surface* surface = IMG_Load_IO(io_image, 1);
    SDL_SetWindowIcon(this->_window, surface);

    this->_imguiBackend = new ImguiBackend(_window);
}

Graphics::~Graphics() {
    delete this->_imguiBackend;
    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    SDL_Quit();
}

void Graphics::draw() {
    this->_imguiBackend->drawImgui();
}

void Graphics::blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst) {
    SDL_RenderTexture(this->_renderer, p_texture, p_src, p_dst);
}

void Graphics::flip() {
    if (!_imguiBackend) {
        SDL_RenderPresent(this->_renderer);
    }
    else {
        SDL_GL_SwapWindow(this->_window);
    }
}

void Graphics::clear() {
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->_renderer);
}

void Graphics::handleEvent(SDL_Event& p_event) {
    this->_imguiBackend->handleEvent(p_event);
}

SDL_Renderer* Graphics::getRenderer() const {
    return this->_renderer;
}