#include "graphics.h"
#include "logo.h"
#include <cmath>    

ImGuiIO* io = nullptr;

static void initImGuiIO() {
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

    // called once after renderer creation:
    _densityTexture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        g::N,
        g::N);

    SDL_SetTextureScaleMode(_densityTexture, SDL_SCALEMODE_LINEAR);
    _densityPixels.resize(static_cast<size_t>(g::N) * static_cast<size_t>(g::N));
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
        ImGui::Text(" | FPS: %.1f (%.3f ms/frame)", io->Framerate, 1000.0f / io->Framerate);
        ImGui::EndMainMenuBar();
    }

    {
        Logic::Settings& s = _logic.settings();

        ImGui::Begin("Sim");

        ImGui::SliderFloat("Viscosity", &s.viscosity, 0.0f, 0.01f, "%.6f");
        ImGui::SliderInt("Diffuse iter", &s.diffuseIterations, 1, 60);
        ImGui::SliderInt("Project iter", &s.projectIterations, 1, 60);

        ImGui::Separator();

        ImGui::SliderFloat("Density add scale", &s.densityAddScale, 0.0f, 10.0f, "%.2f");
        ImGui::SliderFloat("Velocity add scale", &s.velocityAddScale, 0.0f, 10.0f, "%.2f");

        ImGui::Separator();

        ImGui::SliderFloat("Density decay / sec", &s.densityDecayPerSecond, 0.0f, 5.0f, "%.2f");

        ImGui::Separator();

        ImGui::Checkbox("Draw velocity", &s.drawVelocity);
        ImGui::SliderInt("Velocity stride", &s.velocityStride, 1, 32);
        ImGui::SliderFloat("Velocity scale", &s.velocityScale, 0.0f, 50.0f, "%.1f");

        ImGui::Separator();
        ImGui::SliderFloat("Brightness", &s.brightness, 0.1f, 20.0f, "%.2f");
        ImGui::SliderFloat("Gamma", &s.gamma, 0.1f, 2.2f, "%.2f");

        ImGui::Separator();

        if (ImGui::Button("Reset")) {
            _logic.reset();
        }

        ImGui::End();
    }


    ImGui::Render();
    SDL_SetRenderDrawColorFloat(_renderer, black_bg.x, black_bg.y, black_bg.z, black_bg.w);
    SDL_RenderClear(_renderer);
	drawDensityTexture();
    //this->_logic.draw(*this);
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

void Graphics::drawDensityTexture() {
    const float* d = this->_logic.getDensity();
    const Logic::Settings& s = _logic.settings();

    const float brightness = s.brightness;
    const float gamma = s.gamma;

    for (int j = 1; j <= g::N; j++) {
        for (int i = 1; i <= g::N; i++) {
            const int srcIdx = i + (g::N + 2) * j;
            const int dstIdx = (i - 1) + g::N * (j - 1);

            float v = d[srcIdx];
            if (v < 0.0f) v = 0.0f;
            if (v > 255.0f) v = 255.0f;

            float t = (v / 255.0f) * brightness;
            if (t > 1.0f) t = 1.0f;
            t = std::pow(t, gamma);

            const Uint8 intensity = static_cast<Uint8>(t * 255.0f);

            _densityPixels[dstIdx] =
                (255u << 24) |
                (static_cast<Uint32>(intensity) << 16) |
                (0u << 8) |
                (0u << 0);
        }
    }

    SDL_UpdateTexture(_densityTexture, nullptr, _densityPixels.data(), g::N * static_cast<int>(sizeof(Uint32)));

    SDL_FRect dst{};
    dst.x = 0.0f;
    dst.y = static_cast<float>(g::OFFSET);
    dst.w = static_cast<float>(g::SCREEN_SIZE);
    dst.h = static_cast<float>(g::SCREEN_SIZE);

    SDL_RenderTexture(_renderer, _densityTexture, nullptr, &dst);
}