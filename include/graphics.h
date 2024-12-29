#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL_opengl.h>

#include "globals.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class SDL_FRect;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void drawImGui();

    void blitSurface(SDL_Texture* p_texture, SDL_FRect* p_src, SDL_FRect* p_dst);

    void flip();

    void clear();

    void handleEvent(SDL_Event& event);

    SDL_Renderer* getRenderer() const;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    ImGuiIO& _io;
    SDL_GLContext _glContext;
    const char* _glslVersion;
};
#endif