#pragma once

// Enable experimental GLM features
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS

// Core GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/component_wise.hpp>

using mat4 = glm::dmat4;
using vec4 = glm::dvec4;
using vec3 = glm::dvec3;

namespace Colors {
    const glm::u8vec3 Red(255, 0, 0);
    const glm::u8vec3 Green(0, 255, 0);
    const glm::u8vec3 Blue(0, 0, 255);
};