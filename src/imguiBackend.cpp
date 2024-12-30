#include "imguiBackend.h"
#include <iostream>

ImGuiIO* io = nullptr;

static void initImGuiIO() {
    io = &ImGui::GetIO();
}

ImguiBackend::ImguiBackend(SDL_Window* p_window, Logic& p_logic) :
    _window(p_window), 
    _logic(p_logic)
{
#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    this->_glslVersion = "#version 150";
#else
    // GL 3.0 + GLSL 130
    this->_glslVersion = "#version 130";
#endif

    this->_glContext = SDL_GL_CreateContext(this->_window);
    SDL_GL_MakeCurrent(this->_window, this->_glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_ShowWindow(this->_window);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    initImGuiIO();

    ImGui_ImplSDL3_InitForOpenGL(this->_window, this->_glContext);
    ImGui_ImplOpenGL3_Init(this->_glslVersion);
}

ImguiBackend::~ImguiBackend(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(this->_glContext);
}

void ImguiBackend::drawImgui(Graphics &p_graphics) {
    // Update display size
    int display_w, display_h;
    SDL_GetWindowSizeInPixels(this->_window, &display_w, &display_h);
    io->DisplaySize = ImVec2((float)display_w, (float)display_h);
    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");

    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &show_demo_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color);

    if (ImGui::Button("Button"))
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::End();

    this->_logic.draw(p_graphics);

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(this->_window);
}

void ImguiBackend::handleEvent(SDL_Event& p_event) {
    ImGui_ImplSDL3_ProcessEvent(&p_event);
}