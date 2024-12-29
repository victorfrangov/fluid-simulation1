#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "graphics.h"
#include "logo.h"
static ImGuiIO dummyIO;

Graphics::Graphics() : _io(dummyIO) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer("Fluid Simulation", globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &this->_window, &this->_renderer);
    SDL_SetRenderLogicalPresentation(this->_renderer, globals::SCREEN_SIZE, globals::SCREEN_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_IOStream* io_image = SDL_IOFromMem(logo_png, logo_png_len);
    SDL_Surface* surface = IMG_Load_IO(io_image, 1);
    SDL_SetWindowIcon(this->_window, surface);

#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    this->_glslVersion = "#version 150";
#else
    // GL 3.0 + GLSL 130
    this->_glslVersion = "#version 130";
#endif

    this->_glContext = SDL_GL_CreateContext(this->_window);
    SDL_GL_MakeCurrent(this->_window, this->_glContext);
    SDL_ShowWindow(this->_window);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    this->_io = ImGui::GetIO();

    ImGui_ImplSDL3_InitForOpenGL(this->_window, this->_glContext);
    ImGui_ImplOpenGL3_Init(this->_glslVersion);
}

Graphics::~Graphics() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(this->_glContext);
    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    SDL_Quit();
}

//Called in the main game loop in fluid.cpp
void Graphics::drawImGui() {
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / this->_io.Framerate, this->_io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)this->_io.DisplaySize.x, (int)this->_io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(this->_window);
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

void Graphics::handleEvent(SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

SDL_Renderer* Graphics::getRenderer() const {
    return this->_renderer;
}