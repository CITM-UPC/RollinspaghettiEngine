#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

void Transform::translate(const glm::vec3& v) {
	_mat = glm::translate(_mat, v);
}

void Transform::rotate(double rads, const glm::vec3& v) {
	_mat = glm::rotate(_mat, static_cast<float>(rads), v);
}
