#pragma once

#include "types.h"

class Transform {

	union {
		glm::mat4 _mat = glm::mat4(1.0);
		struct {
			glm::vec3 _left; glm::mat4::value_type _left_w;
			glm::vec3 _up; glm::mat4::value_type _up_w;
			glm::vec3 _fwd; glm::mat4::value_type _fwd_w;
			glm::vec3 _pos; glm::mat4::value_type _pos_w;
		};
	};

public:
	const auto& mat() const { return _mat; }
	const auto& left() const { return _left; }
	const auto& up() const { return _up; }
	const auto& fwd() const { return _fwd; }
	const auto& pos() const { return _pos; }
	auto& pos() { return _pos; }

	const auto* data() const { return &_mat[0][0]; }

	Transform() = default;
	Transform(const glm::mat4& mat) : _mat(mat) {}


	void translate(const glm::vec3& v);
	void rotate(double rads, const glm::vec3& v);

	//Funcion SetOrientation
	void setOrientationFromMatrix(const glm::mat4& orientationMatrix) {
		_mat = orientationMatrix;
		_left = glm::vec3(_mat[0]);  // Columna izquierda
		_up = glm::vec3(_mat[1]);    // Columna superior
		_fwd = glm::vec3(_mat[2]);   // Columna frontal
		_pos = glm::vec3(_mat[3]);   // Columna de posición
	}


	Transform operator*(const glm::mat4& other) { return Transform(_mat * other); }
	Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

inline Transform operator*(const glm::mat4& m, const Transform& t) { return Transform(m * t.mat()); }
