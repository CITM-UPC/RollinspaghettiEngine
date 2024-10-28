#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "spaghettiEngine/MyGUI.h"
#include "SpaghettiEngine/Mywindow.h"
#include "SpaghettiEngine/ConsoleWindow.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 1.0);

	glMatrixMode(GL_MODELVIEW);
}

static void drawFloorGrid(int size, double step) {
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}


static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawFloorGrid(26, 1.0);
}

int main(int argc, char** argv) {
	MyWindow window("ImGUI with SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
	//MyGUI gui(window.windowPtr(), window.contextPtr());
	ConsoleWindow console(window.windowPtr(), window.contextPtr());


	init_openGL();

	while (window.processEvents(&console) && window.isOpen()) {
		const auto t0 = hrclock::now();
		display_func();
		console.render();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}



	return 0;
}