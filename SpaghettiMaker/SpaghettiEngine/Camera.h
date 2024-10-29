#pragma once

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp> // Para glm::lookAt
#include <glm/gtx/quaternion.hpp> // Para convertir matriz a cuaternión

class Camera {

public:
	double fov = glm::radians(60.0);
	double aspect = 16.0 / 9.0;
	double zNear = 0.1;
	double zFar = 128.0;


private:
	Transform _transform;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }

	mat4 projection() const;
	mat4 view() const;

    glm::vec3 position() const { return transform().pos(); }


    // Método lookAt modificado
	void lookAt(const glm::vec3& target) {
		glm::vec3 eyePosition = glm::vec3(transform().pos()); // Asegúrate de que esto sea un glm::vec3
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);           // Esto debe ser un glm::vec3

		glm::mat4 lookAtMatrix = glm::lookAt(eyePosition, target, up);
		transform().setOrientationFromMatrix(lookAtMatrix);
	}

};


