#pragma once
#include "glm.hpp"

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Target;
	Camera(glm::vec3 pos, glm::vec3 target);
};