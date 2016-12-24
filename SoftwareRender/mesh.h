#pragma once

#include "glm/glm.hpp"
#include "face.h"
#include <string>
#include <memory>
#include "vertex.h"

using namespace std;

class Mesh {
public:
	string Name;
	glm::vec3 Position;
	glm::vec3 Rotation;
	int verticeCount;
	int faceCount;
	std::unique_ptr<Vertex[]> Vertices;
	std::unique_ptr<Face[]> Faces;
	Mesh(string name, int verticeCount, int faceCount);
	~Mesh();
	static Mesh* CreateCube();
};