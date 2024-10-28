#include "TransformComponent.h"
#include "GameObject.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

void TransformComponent::OnStart() {
    // Initial world matrix calculation
    UpdateWorldMatrix();
}

void TransformComponent::OnUpdate() {
    // Update world matrix if needed
    if (_isDirty) {
        UpdateWorldMatrix();
    }
}

void TransformComponent::SetLocalPosition(const vec3& position) {
    _localPosition = position;
    MarkDirty();
}

void TransformComponent::SetLocalRotation(const glm::dquat& rotation) {
    _localRotation = rotation;
    MarkDirty();
}

void TransformComponent::SetLocalScale(const vec3& scale) {
    _localScale = scale;
    MarkDirty();
}

void TransformComponent::SetLocalEulerAngles(const vec3& eulerAngles) {
    _localRotation = glm::dquat(eulerAngles);
    MarkDirty();
}

vec3 TransformComponent::GetWorldPosition() const {
    return vec3(_worldMatrix[3]);
}

glm::dquat TransformComponent::GetWorldRotation() const {
    vec3 scale;
    glm::dquat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
    glm::decompose(_worldMatrix, scale, rotation, translation, skew, perspective);
    return rotation;
}

vec3 TransformComponent::GetWorldScale() const {
    return vec3(
        glm::length(vec3(_worldMatrix[0])),
        glm::length(vec3(_worldMatrix[1])),
        glm::length(vec3(_worldMatrix[2]))
    );
}

vec3 TransformComponent::GetWorldEulerAngles() const {
    return glm::eulerAngles(GetWorldRotation());
}

void TransformComponent::Translate(const vec3& delta) {
    _localPosition += delta;
    MarkDirty();
}

void TransformComponent::Rotate(double radians, const vec3& axis) {
    glm::dquat rotation = glm::angleAxis(radians, glm::normalize(axis));
    _localRotation = rotation * _localRotation;
    MarkDirty();
}

void TransformComponent::RotateAround(const vec3& point, double radians, const vec3& axis) {
    vec3 worldPos = GetWorldPosition();
    vec3 toPoint = worldPos - point;

    // Create rotation quaternion
    glm::dquat rotation = glm::angleAxis(radians, glm::normalize(axis));

    // Rotate position around point
    toPoint = rotation * toPoint;
    SetLocalPosition(point + toPoint);

    // Apply the same rotation to our orientation
    _localRotation = rotation * _localRotation;
    MarkDirty();
}

void TransformComponent::LookAt(const vec3& target, const vec3& up) {
    vec3 pos = GetWorldPosition();
    vec3 direction = glm::normalize(target - pos);

    // Create look-at matrix
    mat4 lookAt = glm::lookAt(pos, target, up);

    // Extract rotation from look-at matrix
    vec3 scale;
    glm::dquat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
    glm::decompose(lookAt, scale, rotation, translation, skew, perspective);

    _localRotation = rotation;
    MarkDirty();
}

const mat4& TransformComponent::GetLocalMatrix() const {
    return glm::translate(mat4(1.0), _localPosition) *
        glm::mat4_cast(_localRotation) *
        glm::scale(mat4(1.0), _localScale);
}

const mat4& TransformComponent::GetWorldMatrix() {
    if (_isDirty) {
        UpdateWorldMatrix();
    }
    return _worldMatrix;
}

vec3 TransformComponent::GetLocalEulerAngles() const {
    return glm::degrees(glm::eulerAngles(_localRotation));
}

void TransformComponent::MarkDirty() {
    _isDirty = true;

    // Mark all child transforms as dirty
    for (const auto& child : GetOwner()->GetChildren()) {
        if (auto childTransform = child->GetComponent<TransformComponent>()) {
            childTransform->MarkDirty();
        }
    }
}

void TransformComponent::UpdateWorldMatrix() {
    mat4 localMat = GetLocalMatrix();

    // If we have a parent, multiply with parent's world matrix
    if (auto parent = GetOwner()->GetParent()) {
        if (auto parentTransform = parent->GetComponent<TransformComponent>()) {
            _worldMatrix = parentTransform->GetWorldMatrix() * localMat;
        }
        else {
            _worldMatrix = localMat;
        }
    }
    else {
        _worldMatrix = localMat;
    }

    _isDirty = false;
}

#ifdef IMGUI_API
#include "imgui.h"

void TransformComponent::OnInspectorGUI() {
    // Position
    vec3 position = GetLocalPosition();
    if (ImGui::DragScalarN("Position", ImGuiDataType_Double, glm::value_ptr(position), 3, 0.1f)) {
        SetLocalPosition(position);
    }

    // Rotation (in degrees for easier editing)
    vec3 rotationDegrees = glm::degrees(GetLocalEulerAngles());
    if (ImGui::DragScalarN("Rotation", ImGuiDataType_Double, glm::value_ptr(rotationDegrees), 3, 0.1f)) {
        SetLocalEulerAngles(glm::radians(rotationDegrees));
    }

    // Scale
    vec3 scale = GetLocalScale();
    if (ImGui::DragScalarN("Scale", ImGuiDataType_Double, glm::value_ptr(scale), 3, 0.1f)) {
        SetLocalScale(scale);
    }
}
#endif