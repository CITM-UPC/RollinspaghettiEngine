#pragma once
#include "PrimitiveGenerator.h"
#include "Scene.h"
#include "imgui.h"
#include "GameObject.h"

class PrimitiveMenu {
public:
    static void ShowPrimitiveMenu(Scene* activeScene) {
        if (!activeScene) return;

        if (ImGui::BeginMenu("3D Object")) {
            if (ImGui::MenuItem("Cube")) {
                GameObject* cube = PrimitiveGenerator::CreateCube();
                activeScene->CreateGameObject(cube->GetName().c_str(), cube);
            }
            if (ImGui::MenuItem("Sphere")) {
                GameObject* sphere = PrimitiveGenerator::CreateSphere();
                activeScene->CreateGameObject(sphere->GetName().c_str(), sphere);
            }
            if (ImGui::MenuItem("Cylinder")) {
                GameObject* cylinder = PrimitiveGenerator::CreateCylinder();
                activeScene->CreateGameObject(cylinder->GetName().c_str(), cylinder);
            }
            if (ImGui::MenuItem("Plane")) {
                GameObject* plane = PrimitiveGenerator::CreatePlane();
                activeScene->CreateGameObject(plane->GetName().c_str(), plane);
            }
            if (ImGui::MenuItem("Cone")) {
                GameObject* cone = PrimitiveGenerator::CreateCone();
                activeScene->CreateGameObject(cone->GetName().c_str(), cone);
            }
            if (ImGui::MenuItem("Torus")) {
                /* GameObject* torus = PrimitiveGenerator::CreateTorus();
                 activeScene->CreateGameObject(torus->GetName().c_str(), torus);*/
            } 
            if (ImGui::MenuItem("Capsule")) {
                /* GameObject* capsule = PrimitiveGenerator::CreateCapsule();
                 activeScene->CreateGameObject(capsule->GetName().c_str(), capsule);*/
            }
            ImGui::EndMenu();
        }
    }
};