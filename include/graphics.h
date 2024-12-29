#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL_opengl.h>

#include "globals.h"

#include "imguiBackend.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class SDL_FRect;

class Graphics {
public:
    Graphics();

    ~Graphics();

    void draw();

    void blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst);

    void flip();

    void clear();

    void handleEvent(SDL_Event& p_event);

    SDL_Renderer* getRenderer() const;

private:
    ImguiBackend* _imguiBackend;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};