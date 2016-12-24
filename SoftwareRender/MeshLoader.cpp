#include "json\json\json.h"
#include <iostream>
#include <fstream>
#include "mesh.h"


using namespace std;



Mesh* LoadMesh(string fileLocation) {
	Json::Value root;
	Json::Reader reader;
	std::ifstream fileReader(fileLocation, std::ifstream::binary);
	bool readSucces = reader.parse(fileReader, root, false);
	if (!readSucces) {
		string s = reader.getFormattedErrorMessages();
		std::cout << s << "\n";
	}
	else {
		Json::Value Meshes = root["meshes"];
		//Check if the node meshes exists
		//if (Meshes != nullptr) {
			//Create an iterator for all values in meshes
			for (Json::Value::iterator it = Meshes.begin(); it != Meshes.end(); it++) {
				Json::Value vertices = (*it)["vertices"];
				Json::Value indices = (*it)["indices"];
				int uvCount = (*it)["uvCount"].asInt();
				int verticeSteps = 1;
				switch (uvCount) {
				case 0: verticeSteps = 6; break;
				case 1: verticeSteps = 8; break;
				case 2: verticeSteps = 10; break;
				}
				int verticesCount = vertices.size() / verticeSteps;
				int facesCount = indices.size() / 3;
				Mesh* m = new Mesh((*it)["name"].asString(), verticesCount, facesCount);

				for (int index = 0; index < verticesCount; index++)
				{
					float x = vertices[index * verticeSteps].asFloat();
					float y = vertices[index * verticeSteps + 1].asFloat();
					float z = vertices[index * verticeSteps + 2].asFloat();

					float nx = vertices[index * verticeSteps + 3].asFloat();
					float ny = vertices[index * verticeSteps + 4].asFloat();
					float nz = vertices[index * verticeSteps + 5].asFloat();
					m->Vertices[index] = Vertex(glm::vec3(nx, ny, nz), glm::vec3(x, y, z));
				}

				// Then filling the Faces array
				for (int index = 0; index < facesCount; index++)
				{
					int a = indices[index * 3].asInt();
					int b = indices[index * 3 + 1].asInt();
					int c = indices[index * 3 + 2].asInt();
					m->Faces[index] = Face(a, b, c);
				}
				return m;

			}
		//}

	}
}

