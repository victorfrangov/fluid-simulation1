#pragma once
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

struct SDL_Window;

class ImguiBackend {
public:
    ImguiBackend(SDL_Window* p_window);

    ~ImguiBackend();

    void drawImgui();
    
    void handleEvent(SDL_Event &p_event);
private:
    SDL_GLContext _glContext;
    const char* _glslVersion;

    SDL_Window* _window;
};