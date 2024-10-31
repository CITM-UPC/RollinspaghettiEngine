#pragma once

#include "MyWindow.h"
#include <vector>
#include <chrono>
#include "Scene.h"
#include <string>

class ConsoleWindow : public IEventProcessor
{
public:
	ConsoleWindow(SDL_Window* window, void* context);
	ConsoleWindow(ConsoleWindow&&) noexcept = delete;
	ConsoleWindow(const ConsoleWindow&) = delete;
	ConsoleWindow& operator=(const ConsoleWindow&) = delete;
	~ConsoleWindow();
	void render();
	bool _shouldQuit = false;
	void processEvent(const SDL_Event& event) override;
	// Declaracion del metodo getMemoryUsage
	size_t getMemoryUsage();
	void addLog(const std::string& message); // Method to add log messages
	// Add Scene management
	void SetActiveScene(Scene* scene) { _activeScene = scene; }
	Scene* GetActiveScene() const { return _activeScene; }
private:
	Scene* _activeScene = nullptr;  // Add this member
	bool _showEditorWindows = true;
	std::vector<std::string> logBuffer; // Log buffer for storing console messages
	float sliderValue = 0.5f;  // Initial value for the slider
	bool checkboxValue = false;  // Initial value for the checkbox
	bool buttonPressed = false;  // Track if button was pressed
	// Maximum number of FPS values to store for the graph
	static const int maxFPSHistorySize = 100;

	// Variables for FPS calculation and history storage
	std::chrono::high_resolution_clock::time_point lastFrameTime;  // Last frame time
	std::vector<float> fpsHistory;  // Stores recent FPS values for the graph

	bool useWireframe= false;
	bool displayPerTriangleNormals = false;
	bool displayPerFaceNormals = false;
	bool showCheckerboardTexture = false;
	int textureWidth = 1024;
	int textureHeight = 1024;
};

