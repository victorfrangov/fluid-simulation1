#include "graphics.h"
#include "logo.h"

Graphics::Graphics(Logic& p_logic) : _logic(p_logic) {
    SDL_Init(SDL_INIT_VIDEO);

    //Make it borderless??
    SDL_CreateWindowAndRenderer("Fluid Simulation", globals::SCREEN_SIZE + 1, globals::SCREEN_SIZE + globals::OFFSET + 1, SDL_WINDOW_OPENGL, &this->_window, &this->_renderer);
    SDL_SetRenderLogicalPresentation(this->_renderer, globals::SCREEN_SIZE + 1, globals::SCREEN_SIZE + globals::OFFSET + 1, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
    ImGui_ImplSDLRenderer3_Init(_renderer);
}

Graphics::~Graphics() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Graphics::draw() {
    ImVec4 black_bg = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImVec2 p = ImGui::GetWindowPos();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create")) {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
            }
            if (ImGui::MenuItem("Save as..")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }


    ImGui::Render();
    SDL_SetRenderDrawColorFloat(_renderer, black_bg.x, black_bg.y, black_bg.z, black_bg.w);
    SDL_RenderClear(_renderer);
    this->_logic.draw(*this);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);

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

void Graphics::handleEvent(SDL_Event& p_event) {
    ImGui_ImplSDL3_ProcessEvent(&p_event);
}

SDL_Renderer* Graphics::getRenderer() const {
    return this->_renderer;
}