#include "mesh.h"
#include "glm.hpp"

Mesh::Mesh(string name, int verticeCount, int faceCount) :
	Name{ name },
	verticeCount{ verticeCount },
	Vertices{ new glm::vec3[verticeCount] },
	faceCount {faceCount},
	Faces { new Face[faceCount] },
	Position{ glm::vec3(1,0,0) },
	Rotation{ glm::vec3(0,0,0) }
{}

Mesh::~Mesh()
{
	Vertices.reset();
}
