#pragma once
#include "glm/common.hpp"

struct Vertex {
	glm::vec3 Normal;
	glm::vec3 Coordinates;
	glm::vec3 WorldCoordinates;
	Vertex::Vertex() {
		Normal = glm::vec3();
		Coordinates = glm::vec3();
		WorldCoordinates = glm::vec3();
	}
	Vertex::Vertex(glm::vec3 normal, glm::vec3 coord) {
		Normal = normal;
		Coordinates = coord;
	}
	Vertex::Vertex(glm::vec3 normal, glm::vec3 coord, glm::vec3 world) {
		Normal = normal;
		Coordinates = coord;
		WorldCoordinates = world;
	}
};