#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "globals.h"
#include "logic.h"

//class ImguiBackend;

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class SDL_FRect;
class Logic;

class Graphics {
public:
    Graphics(Logic& p_logic);

    ~Graphics();

    void draw();

    void blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst);

    void flip();

    void clear();

    void handleEvent(SDL_Event& p_event);

    SDL_Renderer* getRenderer() const;

private:
    //ImguiBackend* _imguiBackend;
    Logic& _logic;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};