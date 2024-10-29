#include "ConsoleWindow.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h> // For SDL_OpenURL and hardware information
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <chrono>
#include "PrimitiveGenerator.h"
#include "Scene.h"
#include "PrimitiveMenu.h"
#ifdef _WIN32
#include <windows.h>  // Include Windows API header for memory info
#include <psapi.h> 
#endif

ConsoleWindow::ConsoleWindow(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    // Initialize FPS tracking variables
    lastFrameTime = std::chrono::high_resolution_clock::now();
}

ConsoleWindow::~ConsoleWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

// Function to get memory usage (Windows-specific)
size_t ConsoleWindow::getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize; // Return the working set size (in bytes)
    }
#endif
    return 0; // Return 0 if we can't get the memory usage
}

void ConsoleWindow::render() {
    // Start new frames for OpenGL and SDL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Begin a new ImGui window
    ImGui::Begin("Editor");  // Open a new ImGui window with a custom title

    // Add custom content to the window
    ImGui::Text("Main Menu");  // Display some text

    // GitHub Link button
    if (ImGui::Button("GitHub Link")) {
        SDL_OpenURL("https://github.com/CITM-UPC/RollinspaghettiEngine");  // Replace with your actual URL
    }


    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("GameObject")) {
            PrimitiveMenu::ShowPrimitiveMenu(_activeScene);
            ImGui::EndMenu();
        }
        // ... rest of your menu items ...
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Button("Create cube"))
    {
        GameObject* cube = PrimitiveGenerator::CreateCube();
    }

    //if (ImGui::BeginMainMenuBar()) {
    //    if (ImGui::BeginMenu("GameObject")) {
    //        PrimitiveMenu::ShowPrimitiveMenu(activeScene);
    //        ImGui::EndMenu();
    //    }
    //    
    //    // ... rest of your menu items ...
    //    ImGui::EndMainMenuBar();
    //}
    ImGui::SameLine();
    // About Us button
    if (ImGui::Button("About Us")) {
        ImGui::OpenPopup("AboutUsPopup");  // Open the About Us popup
    }

    ImGui::SameLine();
    // Config Window button
    if (ImGui::Button("Config Window")) {
        ImGui::OpenPopup("ConfigWindow");  // Open the Config Window popup
    }

    // About Us popup
    if (ImGui::BeginPopupModal("AboutUsPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Rollinspaghetti Engine\nVersion 1.0\n\nA custom game engine developed by Yiwei Ye, Andrea Dona y Pablo Longaron");
        ImGui::Text("This engine is designed to show our learning in the programming area,");
        ImGui::Text("providing tools for rendering and loading textures");

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Config Window popup (contains FPS graph and system info)
    if (ImGui::BeginPopupModal("ConfigWindow", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display the FPS graph in the popup
       
        // FPS calculation
         auto now = std::chrono::high_resolution_clock::now();
         float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(now - lastFrameTime).count();
         lastFrameTime = now;

          // Calculate FPS and store in buffer
          float fps = 1.0f / deltaTime;
          if (fpsHistory.size() >= maxFPSHistorySize) {
             fpsHistory.erase(fpsHistory.begin());
          }
          fpsHistory.push_back(fps);

    // Display current FPS
         ImGui::Text("FPS: %.1f", fps);  // Display current FPS
        ImGui::PlotLines("FPS History", fpsHistory.data(), fpsHistory.size(), 0, NULL, 0.0f, 120.0f, ImVec2(0, 100));

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

        // Memory consumption information
        size_t memoryUsage = getMemoryUsage(); // Get memory usage
        ImGui::Text("Memory Usage: %zu bytes (%.2f MB)", memoryUsage, memoryUsage / (1024.0f * 1024.0f));

        // Close button for Config Window popup
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
