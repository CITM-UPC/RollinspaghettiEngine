#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "IL/il.h"
#include "IL/ilu.h"
#include "spaghettiEngine/MyGUI.h"
#include "SpaghettiEngine/Mywindow.h"
#include "SpaghettiEngine/ConsoleWindow.h"
#include "SpaghettiEngine/Transform.h"
#include "SpaghettiEngine/Camera.h"
#include "SpaghettiEngine/TextureManager.h"

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static Camera camera;

struct Cube {
    Transform transform;
    glm::u8vec3 color;

    vector<vec3> vertex_data = {
        vec3(-1,-1,-1),
        vec3(1,-1,-1),
        vec3(1,1,-1),
        vec3(-1,1,-1),
        vec3(-1,-1,1),
        vec3(1,-1,1),
        vec3(1,1,1),
        vec3(-1,1,1)
    };

    vector<unsigned int> index_data = {
        0, 1, 2, 0, 2, 3,
        1, 5, 6, 1, 6, 2,
        5, 4, 7, 5, 7, 6,
        4, 0, 3, 4, 3, 7,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0
    };

    unsigned int vBID;
    unsigned int iBID;

    void Init() {
        glGenBuffers(1, &vBID);
        glBindBuffer(GL_ARRAY_BUFFER, vBID);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(vec3), vertex_data.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &iBID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(unsigned int), index_data.data(), GL_STATIC_DRAW);
    }

    void draw() const {
        glPushMatrix();
        glMultMatrixd(&transform.mat()[0][0]);
        glColor3ub(color.r, color.g, color.b);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, vBID);
        glVertexPointer(3, GL_DOUBLE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBID);
        glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
    }
};

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


static Cube cube;

static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(&camera.view()[0][0]);
    drawFloorGrid(26, 1.0);
    cube.transform.rotate(0.001, vec3(1, 1, 0));
    cube.draw();
}

static void reshape_func(int width, int height) {
    glViewport(0, 0, width, height);
    camera.aspect = static_cast<double>(width) / height;
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(&camera.projection()[0][0]);
}


int main(int argc, char** argv) {
    MyWindow window("ImGUI with SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    //MyGUI gui(window.windowPtr(), window.contextPtr());
    ConsoleWindow console(window.windowPtr(), window.contextPtr());


    init_openGL();
    //DevIL initialization
    ilInit();
    iluInit();

    // Initialize the texture manager
    TEXTURE_MANAGER->Initialize();


    //Cube
    cube.color = glm::u8vec3(0, 0, 255);
    cube.Init();
    cube.transform.translate(vec3(0, 1, 1));

    // Init camera
    camera.transform().pos() = vec3(0, 1, 4);
    camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));



    while (window.processEvents(&console) && window.isOpen()) {
        const auto t0 = hrclock::now();
        display_func();
        reshape_func(WINDOW_SIZE.x, WINDOW_SIZE.y);
        console.render();
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }



    return 0;
}