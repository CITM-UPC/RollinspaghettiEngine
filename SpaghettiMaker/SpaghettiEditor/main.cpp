#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "IL/il.h"
#include "IL/ilu.h"
#include "SpaghettiEngine/Mywindow.h"
#include "SpaghettiEngine/ConsoleWindow.h"
#include "SpaghettiEngine/Transform.h"
#include "SpaghettiEngine/Camera.h"
#include "SpaghettiEngine/TextureManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include "spaghettiEngine/PrimitiveGenerator.h"
#include "spaghettiEngine/GameObject.h"
#include "spaghettiEngine/Scene.h"
#include "spaghettiEngine/ModelLoader.h"
#include <assimp/DefaultLogger.hpp>  // Add this for logging functions
#include <assimp/LogStream.hpp>      // Add this for logging functions
#include <assimp/cimport.h>          // Add this for C-style functions like aiDetachAllLogStreams
#include "spaghettiEngine/Transform.h"



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
        glMultMatrixf(&transform.mat()[0][0]);
        glColor3ub(color.r, color.g, color.b);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, vBID);
        glVertexPointer(3, GL_DOUBLE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBID);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index_data.size()), GL_UNSIGNED_INT, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
    }
};


static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");

    // Set up OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    // Set default render states
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);

    // Set up default material
    GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat defaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat defaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat defaultShininess = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(1.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void init_devil() {
    // Initialize DevIL with error checking
    ilInit();
    ILenum error = ilGetError();
    if (error != IL_NO_ERROR) {
        std::cerr << "DevIL initialization failed: " << iluErrorString(error) << std::endl;
        return;
    }

    // Set DevIL options
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    // Try to enable PNG loading specifically
    ilEnable(IL_PNG_ALPHA_INDEX);

    std::cout << "DevIL initialized successfully" << std::endl;
    std::cout << "DevIL version: " << ilGetInteger(IL_VERSION_NUM) << std::endl;
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
//static Casa casa;

static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(&camera.view()[0][0]);
    drawFloorGrid(26, 1.0);
    cube.transform.rotate(0.001, vec3(1, 1, 0));
    /*cube.draw();*/
    //casa.transform.rotate(0.001, vec3(1, 1, 0));
    //casa.draw();
}

static void reshape_func(int width, int height) {
    glViewport(0, 0, width, height);
    camera.aspect = static_cast<double>(width) / height;
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(&camera.projection()[0][0]);
}

static Scene* _activeScene = nullptr;
void Update() {
    _activeScene->Update();
    _activeScene->Render();
}

//El rayo para ver donde apunta el raton Camera Zoom
void raycastFromMouse(int mouseX, int mouseY) {
    // Obtener la matriz de proyección y la vista de la cámara
    glm::mat4 projection = camera.projection();
    glm::mat4 view = camera.view();
    glm::vec4 viewport(0.0f, 0.0f, WINDOW_SIZE.x, WINDOW_SIZE.y);

    // Convertir la posición del ratón a coordenadas de espacio normalizado
    glm::vec3 mousePosNear = glm::unProject(
        glm::vec3(mouseX, WINDOW_SIZE.y - mouseY, 0.0f), // Posición del mouse en el "near plane"
        view,
        projection,
        viewport
    );

    glm::vec3 mousePosFar = glm::unProject(
        glm::vec3(mouseX, WINDOW_SIZE.y - mouseY, 1.0f), // Posición del mouse en el "far plane"
        view,
        projection,
        viewport
    );

    // Calcular la dirección del rayo
    glm::vec3 rayDirection = glm::normalize(mousePosFar - mousePosNear);

    // Apuntar la cámara en esa dirección
    camera.lookAt(camera.position() + rayDirection);
}


void cameramovement(const SDL_Event& event, Scene* scene) {
    
    static bool isLeftMouseDown = false;
    static int lastMouseX, lastMouseY;
   
    const Uint8* keystates = SDL_GetKeyboardState(nullptr);

    // Add debug toggle for F1 key
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F1) {
        if (scene) {
            scene->ToggleDebug();
            std::cout << "Debug visualization toggled" << std::endl;
        }
    }
    
    // Free look-around movement
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
        isLeftMouseDown = true;
        lastMouseX = event.button.x;
        lastMouseY = event.button.y;
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
        isLeftMouseDown = false;
    }

    if (isLeftMouseDown && event.type == SDL_MOUSEMOTION) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;

        int deltaX = mouseX - lastMouseX;
        int deltaY = mouseY - lastMouseY;

        // Adjust sensitivity as needed
        float sensitivity = 0.1f;
        float yaw = deltaX * sensitivity;
        float pitch = deltaY * sensitivity;

        // Apply the rotation
        camera.transform().rotate(glm::radians(-yaw), vec3(0, 1, 0));    // Yaw rotation around Y axis
        camera.transform().rotate(glm::radians(-pitch), vec3(-1, 0, 0));  // Pitch rotation around X axis

        // Update last mouse position
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

	// Camera movement
	if (keystates[SDL_SCANCODE_LSHIFT]) {
        if (keystates[SDL_SCANCODE_W]) {
            camera.transform().translate(vec3(0, 0, 2));
        }
        if (keystates[SDL_SCANCODE_S]) {
            camera.transform().translate(vec3(0, 0, -2));
        }
        if (keystates[SDL_SCANCODE_A]) {
            camera.transform().translate(vec3(2, 0, 0));
        }
        if (keystates[SDL_SCANCODE_D]) {
            camera.transform().translate(vec3(-2, 0, 0));
        }
    }
    else {
          if (keystates[SDL_SCANCODE_W]) {
             camera.transform().translate(vec3(0, 0, 1));
          }
          if (keystates[SDL_SCANCODE_S]) {
             camera.transform().translate(vec3(0, 0, -1));
          }
          if (keystates[SDL_SCANCODE_A]) {
                camera.transform().translate(vec3(1, 0, 0));
          }
          if (keystates[SDL_SCANCODE_D]) {
              camera.transform().translate(vec3(-1, 0, 0));
          }
    }

	// Camera rotation For keyboard (debug)
    if (keystates[SDL_SCANCODE_UP]) {
		camera.transform().rotate(glm::radians(-1.0), vec3(1, 0, 0));
    }
    if (keystates[SDL_SCANCODE_DOWN]) { 
		camera.transform().rotate(glm::radians(1.0), vec3(1, 0, 0));
    }
    if (keystates[SDL_SCANCODE_LEFT]) {
		camera.transform().rotate(glm::radians(1.0), vec3(0, 1, 0));
    }
	if (keystates[SDL_SCANCODE_RIGHT]) {
		camera.transform().rotate(glm::radians(-1.0), vec3(0, 1, 0));
	}

	
    //Camera Zoom 

    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
            camera.fov -= glm::radians(2.0f); // Acerca la cámara (zoom in)
        }
        else if (event.wheel.y < 0) {
            camera.fov += glm::radians(2.0f); // Aleja la cámara (zoom out)
        }

        // Limitar el rango de zoom
        if (camera.fov < glm::radians(30.0f)) camera.fov = glm::radians(30.0f);
        if (camera.fov > glm::radians(60.0f)) camera.fov = glm::radians(60.0f);

    }

    //Right Click

    if (keystates[SDL_SCANCODE_1]) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // Calcular el rayo en el espacio 3D a partir de la posición del ratón
        raycastFromMouse(mouseX, mouseY);
    }


}

int main(int argc, char** argv) {

    MyWindow window("ImGUI with SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    //MyGUI gui(window.windowPtr(), window.contextPtr());
    ConsoleWindow console(window.windowPtr(), window.contextPtr());

    // Create and initialize scene
    Scene* scene = new Scene("Main Scene");
    console.SetActiveScene(scene);  // Set the active scene
	scene->SetCamera(&camera);     // Set the camera for the scene

    init_openGL();
    init_devil();
    // Initialize DevIL with error checking
    ILuint error;
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
        std::cerr << "Wrong DevIL version" << std::endl;
        return -1;
    }

    //ilInit();
    //error = ilGetError();
    //if (error != IL_NO_ERROR) {
    //    std::cerr << "DevIL initialization error: " << iluErrorString(error) << std::endl;
    //    return -1;
    //}

    // Configure DevIL settings
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_FILE_OVERWRITE);

    // Initialize the texture manager
    TEXTURE_MANAGER->Initialize();

    // Set up assimp logging
    Assimp::Importer importer;
    // Stream log messages to Debug window
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);

    // Create a cube and add it to the scene
    GameObject* cube = PrimitiveGenerator::CreateCube("TestCube", 5.0f);
    scene->CreateGameObject("TestCube", cube);

    //const char* modelPath = "../SpaghettiEngine/BakerHouse.fbx";
    //GameObject* model = ModelLoader::LoadModel(scene, modelPath);
    //Cube
    //cube.color = glm::u8vec3(0, 0, 255);
    //cube.Init();
    //cube.transform.translate(vec3(0, 1, 1));

    // Init camera
    camera.transform().pos() = vec3(0, 5, 10);
    camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));
 

	//Camera movement


	//Casa
    //casa.Geometry("../SpaghettiEngine/BakerHouse.fbx");
   // casa.Init();
	//casa.transform.translate(vec3(0, 1, -1));

    // After scene creation and before the main loop
    if (scene) {
        // Load initial model (baker house)
        const char* modelPath = "../SpaghettiEngine/BakerHouse.fbx";  // Adjust path as needed
		const char* texturePath = "BakerHouse.png";  // Adjust path as needed

        GameObject* model = ModelLoader::LoadModel(scene, modelPath, texturePath);
        if (model) {
            std::cout << "Successfully loaded baker house model" << std::endl;

            // Verify material and textureg
            if (auto material = model->GetComponent<MaterialComponent>()) {
                std::cout << "Material found on model" << std::endl;
                if (material->GetDiffuseTexture()) {
                    std::cout << "Texture loaded successfully" << std::endl;
                }
            }


            // Optionally position the model
            if (auto transform = model->GetComponent<TransformComponent>()) {
                transform->SetLocalPosition(vec3(0, 0, 0));
                transform->SetLocalScale(vec3(1, 1, 1));
            }

            scene->SetSelectedGameObject(model);  // Auto-select the loaded model
            scene->FocusOnGameObject(model);      // Focus camera on it
        }
        else {
            std::cerr << "Failed to load baker house model" << std::endl;
        }

        std::ifstream test("Baker_house.png", std::ios::binary);
        if (test.good()) {
            std::cout << "Can open PNG file directly" << std::endl;
            test.seekg(0, std::ios::end);
            std::cout << "PNG file size: " << test.tellg() << " bytes" << std::endl;
        }
        else {
            std::cout << "Cannot open PNG file!" << std::endl;
        }
    }


    
    IEventProcessor* event_processor = &console;

    while (window.isOpen()) {
        if (console._shouldQuit) break;

        const auto t0 = hrclock::now();

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up camera matrices
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(glm::value_ptr(camera.projection()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(glm::value_ptr(camera.view()));

        // Update and render scene
        if (scene) {
            scene->Update();
            // Draw floor grid
            drawFloorGrid(26, 1.0);
            scene->Render();
        }



        // Render UI
        console.render();
        window.swapBuffers();

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);

        //Camera Zoom
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            event_processor->processEvent(event);
            cameramovement(event, scene); // Llama a cameramovement pasando el evento

            switch (event.type) {
            case SDL_QUIT:
                window.close();
                break;

            case SDL_DROPFILE: {
                char* droppedFilePath = event.drop.file;
                if (droppedFilePath) {
                    // Log file drop
                    std::cout << "File dropped: " << droppedFilePath << std::endl;

                    // Handle the dropped file
                    if (scene) {
                        scene->HandleFileDrop(droppedFilePath);
                    }
                    SDL_free(droppedFilePath);
                }
                break;
            }
                if (event.type == SDL_QUIT) window.close(); // Otras condiciones para cerrar la ventana             // ... other event cases
            }


           
        }
    }

    // At the end of main(), before return
    // Cleanup
    TEXTURE_MANAGER->Cleanup();
    TEXTURE_MANAGER->Destroy();
    aiDetachAllLogStreams();
    delete scene;
    return 0;
}