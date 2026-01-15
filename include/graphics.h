#pragma once
#include <SDL3/SDL.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "globals.h"
#include "logic.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class SDL_FRect;
class Logic;

class Graphics {
public:
    Graphics(Logic& logic);
    ~Graphics();

    void draw();
    void flip();
    void clear();
    void handleEvent(SDL_Event& event);

    SDL_Renderer* getRenderer() const;

private:
    void drawDensityTexture();

    Logic& _logic;
    SDL_Window* _window;
    SDL_Renderer* _renderer;

    SDL_Texture* _densityTexture = nullptr;
    std::vector<Uint32> _densityPixels;
};