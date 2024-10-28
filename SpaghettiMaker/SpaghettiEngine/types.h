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
using vec2 = glm::dvec2; // Added vec2 definition
using quat = glm::dquat; // Added quaternion definition for convenience

// Integer vector types
using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;

// Float vector types (for cases where double precision isn't needed)
using fvec2 = glm::vec2;
using fvec3 = glm::vec3;
using fvec4 = glm::vec4;
using fmat4 = glm::mat4;

namespace Colors {
    const glm::u8vec3 Red(255, 0, 0);
    const glm::u8vec3 Green(0, 255, 0);
    const glm::u8vec3 Blue(0, 0, 255);
    const glm::u8vec3 White(255, 255, 255);
    const glm::u8vec3 Black(0, 0, 0);
    const glm::u8vec3 Yellow(255, 255, 0);
    const glm::u8vec3 Magenta(255, 0, 255);
    const glm::u8vec3 Cyan(0, 255, 255);
};