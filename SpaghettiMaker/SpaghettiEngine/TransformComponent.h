#pragma once
#include "Component.h"
#include "types.h"
#include <glm/gtx/quaternion.hpp>

class TransformComponent : public Component {
private:
    // Local space transformations
    vec3 _localPosition = vec3(0.0);
    glm::dquat _localRotation = glm::dquat(1.0, 0.0, 0.0, 0.0); // Identity quaternion
    vec3 _localScale = vec3(1.0);

    // Cached world space matrix
    mat4 _worldMatrix = mat4(1.0);
    bool _isDirty = true;

    // Helper function declarations
    void MarkDirty();
    void UpdateWorldMatrix();

public:
    TransformComponent() : Component("Transform") {}

    // Component interface implementation
    void OnStart() override;
    void OnUpdate() override;

    // Local space transformations
    void SetLocalPosition(const vec3& position);
    void SetLocalRotation(const glm::dquat& rotation);
    void SetLocalScale(const vec3& scale);
    void SetLocalEulerAngles(const vec3& eulerAngles);

    // World space transformations
    vec3 GetWorldPosition() const;
    glm::dquat GetWorldRotation() const;
    vec3 GetWorldScale() const;
    vec3 GetWorldEulerAngles() const;

    // Transform operations
    void Translate(const vec3& delta);
    void Rotate(double radians, const vec3& axis);
    void RotateAround(const vec3& point, double radians, const vec3& axis);
    void LookAt(const vec3& target, const vec3& up = vec3(0, 1, 0));

    // Direction vectors
    vec3 Right() const { return glm::normalize(vec3(_worldMatrix[0])); }
    vec3 Up() const { return glm::normalize(vec3(_worldMatrix[1])); }
    vec3 Forward() const { return glm::normalize(-vec3(_worldMatrix[2])); } // -Z forward like OpenGL

    // Matrix access
    const mat4& GetLocalMatrix() const;
    const mat4& GetWorldMatrix();

    // Getters for local transforms
    const vec3& GetLocalPosition() const { return _localPosition; }
    const glm::dquat& GetLocalRotation() const { return _localRotation; }
    const vec3& GetLocalScale() const { return _localScale; }
    vec3 GetLocalEulerAngles() const;

    // Optional: Editor GUI
    void OnInspectorGUI() override;
};