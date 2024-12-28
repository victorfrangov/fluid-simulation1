#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "globals.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
struct SDL_Texture;

class SDL_FRect;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst);

    void flip();

    void clear();

    SDL_Renderer* getRenderer() const;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};
#endif