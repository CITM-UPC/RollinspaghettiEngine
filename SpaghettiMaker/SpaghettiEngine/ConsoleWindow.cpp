#include "ConsoleWindow.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h> // Add this for SDL_OpenURL
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




    if (ImGui::Button("GitHub Link")) {  // Example button
        // Redirect to a URL when the button is pressed
        SDL_OpenURL("https://github.com/CITM-UPC/RollinspaghettiEngine");  // Replace with your actual URL
    }

    ImGui::End();  // End the ImGui window

    // Render the ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ConsoleWindow::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
