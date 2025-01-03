#include "graphics.h"
#include "logo.h"

ImGuiIO* io = nullptr;

void initImGuiIO() {
    io = &ImGui::GetIO();
};

Graphics::Graphics(Logic& logic) : _logic(logic) {
    SDL_Init(SDL_INIT_VIDEO);

    //Make it borderless??
    SDL_CreateWindowAndRenderer("Fluid Simulation", g::SCREEN_SIZE + 1, g::SCREEN_SIZE + g::OFFSET + 1, SDL_WINDOW_OPENGL, &this->_window, &this->_renderer);
    SDL_SetRenderLogicalPresentation(this->_renderer, g::SCREEN_SIZE + 1, g::SCREEN_SIZE + g::OFFSET + 1, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    initImGuiIO();
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
        ImGui::Text(" | FPS: %.1f (%.3f ms/frame)", io->Framerate, 1000.0f / io->Framerate);
        ImGui::EndMainMenuBar();
    }


    ImGui::Render();
    SDL_SetRenderDrawColorFloat(_renderer, black_bg.x, black_bg.y, black_bg.z, black_bg.w);
    SDL_RenderClear(_renderer);
    this->_logic.draw(*this);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);

}

void Graphics::flip() {
    SDL_RenderPresent(this->_renderer);
}

void Graphics::clear() {
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->_renderer);
}

void Graphics::handleEvent(SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

SDL_Renderer* Graphics::getRenderer() const {
    return this->_renderer;
}