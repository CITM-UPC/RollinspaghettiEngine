#pragma once

#include "MyWindow.h"
#include <vector>
#include <chrono>
#include "Scene.h"

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

	// Add Scene management
	void SetActiveScene(Scene* scene) { _activeScene = scene; }
	Scene* GetActiveScene() const { return _activeScene; }
private:
	Scene* _activeScene = nullptr;  // Add this member
	bool _showEditorWindows = true;
	
	float sliderValue = 0.5f;  // Initial value for the slider
	bool checkboxValue = false;  // Initial value for the checkbox
	bool buttonPressed = false;  // Track if button was pressed
	// Maximum number of FPS values to store for the graph
	static const int maxFPSHistorySize = 100;

	// Variables for FPS calculation and history storage
	std::chrono::high_resolution_clock::time_point lastFrameTime;  // Last frame time
	std::vector<float> fpsHistory;  // Stores recent FPS values for the graph
};

