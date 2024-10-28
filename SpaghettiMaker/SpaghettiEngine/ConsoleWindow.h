#pragma once

#include "MyWindow.h"

class ConsoleWindow : public IEventProcessor
{
public:
	ConsoleWindow(SDL_Window* window, void* context);
	ConsoleWindow(ConsoleWindow&&) noexcept = delete;
	ConsoleWindow(const ConsoleWindow&) = delete;
	ConsoleWindow& operator=(const ConsoleWindow&) = delete;
	~ConsoleWindow();
	void render();

	void processEvent(const SDL_Event& event) override;

private:
	float sliderValue = 0.5f;  // Initial value for the slider
	bool checkboxValue = false;  // Initial value for the checkbox
	bool buttonPressed = false;  // Track if button was pressed

};

