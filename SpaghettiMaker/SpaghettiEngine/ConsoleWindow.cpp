#include "ConsoleWindow.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h> // For SDL_OpenURL and hardware information
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>


ConsoleWindow::ConsoleWindow(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

ConsoleWindow::~ConsoleWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ConsoleWindow::render() {
    // Start new frames for OpenGL and SDL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Begin a new ImGui window
    ImGui::Begin("My Custom Window");  // Open a new ImGui window with a custom title

    // Add custom content to the window
    ImGui::Text("Hello, welcome to My Custom Window!");  // Display some text
    ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);  // Example slider
    ImGui::Checkbox("Check Me!", &checkboxValue);  // Example checkbox

    // GitHub Link button
    if (ImGui::Button("GitHub Link")) {
        SDL_OpenURL("https://github.com/CITM-UPC/RollinspaghettiEngine");  // Replace with your actual URL
    }

    // About Us button
    if (ImGui::Button("About Us")) {
        ImGui::OpenPopup("AboutUsPopup");  // Open the About Us popup
    }

    // About Us popup
    if (ImGui::BeginPopupModal("AboutUsPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Rollinspaghetti Engine\nVersion 1.0\n\nA custom game engine developed by CITM-UPC.");
        ImGui::Text("This engine is designed to simplify game development,");
        ImGui::Text("providing tools for rendering, physics, and more.");

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // System Info button
    if (ImGui::Button("System Info")) {
        ImGui::OpenPopup("SystemInfoPopup");  // Open the System Info popup
    }

    // System Info popup
    if (ImGui::BeginPopupModal("SystemInfoPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display hardware and software information
        ImGui::Text("Hardware and Software Information:");

        // CPU and GPU information
      
        ImGui::Text("CPU Cores: %d", SDL_GetCPUCount());
        ImGui::Text("System RAM: %d MB", SDL_GetSystemRAM());
        ImGui::Text("CPU Cache Line Size: %d bytes", SDL_GetCPUCacheLineSize());

        // SDL Version
        SDL_version compiled, linked;
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        ImGui::Text("SDL Version: %d.%d.%d (compiled), %d.%d.%d (linked)",
            compiled.major, compiled.minor, compiled.patch,
            linked.major, linked.minor, linked.patch);

        // OpenGL Version
        const GLubyte* glVersion = glGetString(GL_VERSION);
        ImGui::Text("OpenGL Version: %s", glVersion);


        // Close button for System Info popup
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();  // End the ImGui window

    // Render the ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ConsoleWindow::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
