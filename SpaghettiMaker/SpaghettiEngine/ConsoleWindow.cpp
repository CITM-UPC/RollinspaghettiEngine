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

// Constructor
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
    _showEditorWindows = true; // Initialize visibility state
}

// Destructor
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

void ConsoleWindow::addLog(const std::string& message) {
    logBuffer.push_back(message); // Add a new log entry to the buffer
    if (logBuffer.size() > 100) { // Optional: limit the log buffer size
        logBuffer.erase(logBuffer.begin());
    }
}
// Render method
void ConsoleWindow::render() {
    // Start new frames for OpenGL and SDL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("GameObjects")) {
            PrimitiveMenu::ShowPrimitiveMenu(_activeScene);
            ImGui::EndMenu();
        }

        // Toggle Editor Windows Button
        if (ImGui::Button(_showEditorWindows ? "Hide Editor Windows" : "Show Editor Windows")) {
            _showEditorWindows = !_showEditorWindows; // Toggle visibility
        }

        // GitHub Link button
        if (ImGui::Button("GitHub Link")) {
            SDL_OpenURL("https://github.com/CITM-UPC/RollinspaghettiEngine");  // Replace with your actual URL
        }

        if (ImGui::Button("About Us")) {
            ImGui::OpenPopup("AboutUsPopup");  // Open the About Us popup
        }

        if (ImGui::BeginPopupModal("AboutUsPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Rollinspaghetti Engine\nVersion 1.0\n\nA custom game engine developed by Yiwei Ye, Andrea Dona y Pablo Longaron");
            ImGui::Text("This engine is designed to show our learning in the programming area,");
            ImGui::Text("providing tools for rendering and loading textures");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Config Window button
        if (ImGui::Button("Config Window")) {
            ImGui::OpenPopup("ConfigWindow");  // Open the Config Window popup
        }

        // Handle Config Window popup
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


            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Console")) {
            ImGui::OpenPopup("ConsolePopup");  
        }

        if (ImGui::BeginPopupModal("ConsolePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

            ImGui::Text("Console Log:");

            ImGui::BeginChild("LogScrollingRegion", ImVec2(300, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (const auto& logEntry : logBuffer) {
                ImGui::TextUnformatted(logEntry.c_str());
            }
            ImGui::EndChild();

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        // Quit Button
        if (ImGui::Button("Quit")) {
            _shouldQuit = true; // Set the quit flag
        }

        ImGui::EndMainMenuBar();
    }

    // Render editor windows only if they are visible
    if (_showEditorWindows) {

        // Set fixed position and size
        ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);   // Set a fixed position (10, 10)
        ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Always); // Set a fixed size (300x500)

        ImGui::Begin("Editor");  // Open a new ImGui window with a custom title

        // Add custom content to the window
        ImGui::Text("Hierarchy");  // Display some text

        ImGui::Text("GameObject1");
        ImGui::Text("GameObject2");
        ImGui::Text("GameObject3");

        ImGui::End();  // End the ImGui window
    }

    if (_showEditorWindows) {

        // Set fixed position and size
        ImGui::SetNextWindowPos(ImVec2(980, 20), ImGuiCond_Always);   // Set a fixed position (10, 10)
        ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Always); // Set a fixed size (300x500)

        ImGui::Begin("Inspector  (Currently does not detect gameobjects)");  // Open a new ImGui window with a custom title

        // Add an expandable panel for "Transform"
        if (ImGui::CollapsingHeader("Transform")) {
            // Content inside the "Transform" panel
            ImGui::Text("Position: (x, y, z)");  // Example position
            ImGui::Text("Rotation: (pitch, yaw, roll)");  // Example rotation
            ImGui::Text("Scale: (x, y, z)");  // Example scale

            float position[3] = { 0.0f, 0.0f, 0.0f };
            float rotation[3] = { 0.0f, 0.0f, 0.0f };
            float scale[3] = { 0.0f, 0.0f, 0.0f };

            // Sliders for position, rotation, and scale
            ImGui::SliderFloat3("Position", position, -10.0f, 10.0f); // Add sliders for position
            ImGui::SliderFloat3("Rotation", rotation, 0.0f, 360.0f); // Add sliders for rotation
            ImGui::SliderFloat3("Scale", scale, 0.1f, 5.0f); // Add sliders for scale
        }

        // Add an expandable panel for "Mesh"
        if (ImGui::CollapsingHeader("Mesh")) {
            // Content inside the "Mesh" panel
            ImGui::Text("Mesh File: "); // Display mesh file name
            ImGui::SameLine();
            

            ImGui::Text("Mesh Properties: ");
            
            ImGui::Checkbox("Use Wireframe", &useWireframe); // Example checkbox for wireframe mode

            // Normals display options
            ImGui::Checkbox("Display Per-Triangle Normals", &displayPerTriangleNormals);
            ImGui::Checkbox("Display Per-Face Normals", &displayPerFaceNormals);
            
        }

        // Add an expandable panel for "Texture"
        if (ImGui::CollapsingHeader("Texture")) {
			// Content inside the "Texture" panel
            ImGui::Text("Texture File: "); // Display texture file name
            ImGui::SameLine();

            // Declare textureFilePath variable
            char textureFilePath[256] = "";

            ImGui::InputText("##TextureFile", textureFilePath, sizeof(textureFilePath)); // Input field for texture file path

            // Display texture size
            ImGui::Text("Texture Size: %dx%d", textureWidth, textureHeight); // Display texture dimensions


            // Option to view with checkerboard texture
            ImGui::Checkbox("Show Checkerboard Texture", &showCheckerboardTexture); // Checkbox for checkerboard option
           
        }


        ImGui::End();  // End the ImGui window
    }
    // Render the ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Event processing
void ConsoleWindow::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
