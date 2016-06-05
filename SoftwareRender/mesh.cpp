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

Mesh* Mesh::CreateCube() {
	Mesh* mesh = new Mesh("Cube", 9, 12);
	mesh->Vertices[0] = glm::vec3(-1, 1, 1);
	mesh->Vertices[1] = glm::vec3(1, 1, 1);
	mesh->Vertices[2] = glm::vec3(-1, -1, 1);
	mesh->Vertices[3] = glm::vec3(1, -1, 1);
	mesh->Vertices[4] = glm::vec3(-1, 1, -1);
	mesh->Vertices[5] = glm::vec3(1, 1, -1);
	mesh->Vertices[6] = glm::vec3(1, -1, -1);
	mesh->Vertices[7] = glm::vec3(-1, -1, -1);
	mesh->Vertices[8] = glm::vec3(-1, -1, -1);

	mesh->Faces[0] = Face(0, 1, 2);
	mesh->Faces[1] = Face(1, 2, 3);
	mesh->Faces[2] = Face(1, 3, 6);
	mesh->Faces[3] = Face(1, 5, 6);;
	mesh->Faces[4] = Face(0, 1, 4);
	mesh->Faces[5] = Face(1, 4, 5);

	mesh->Faces[6] = Face(2, 3, 7);
	mesh->Faces[7] = Face(3, 6, 7);
	mesh->Faces[8] = Face(0, 2, 7);
	mesh->Faces[9] = Face(0, 4, 7);
	mesh->Faces[10] = Face(4, 5, 6);
	mesh->Faces[11] = Face(4, 6, 7);
	return mesh;
}
