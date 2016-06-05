#include <iostream>
#include "mesh.h"
#include "face.h"
#include "camera.h"
#include "gtc\matrix_transform.hpp"
#include "gtc\quaternion.hpp"
#include "glm.hpp"
#include "ext.hpp"
#include "mainRenderer.hpp"
#include "meshLoader.h"
#include "linkedList.h"
#include <vector>

int width = 0;
int height = 0;
HDC hdcr = 0;

Mesh* createCube() {
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

glm::vec3 transformCoordinate(glm::vec3 coordinate, glm::mat4 transform)
{
	float x = (coordinate.x * transform[0][0]) + (coordinate.y * transform[1][0]) + (coordinate.z * transform[2][0]) + transform[3][0];
	float y = (coordinate.x * transform[0][1]) + (coordinate.y * transform[1][1]) + (coordinate.z * transform[2][1]) + transform[3][1];
	float z = (coordinate.x * transform[0][2]) + (coordinate.y * transform[1][2]) + (coordinate.z * transform[2][2]) + transform[3][2];
	float w = 1.0f / ((coordinate.x * transform[0][3]) + (coordinate.y * transform[1][3]) + (coordinate.z * transform[2][3]) + transform[3][3]);
	return glm::vec3(x * w, y * w, z * w);
}

glm::vec2 project(glm::vec3 vec, glm::mat4 trans) {
	glm::vec3 point = transformCoordinate(vec, trans);
	int x = point.x * width + width / 2.0f;
	int y = -point.y * height + height / 2.0f;
	return glm::vec2(x, y);
}

void drawPoint(glm::vec2 point, glm::vec3 color) {
	COLORREF kleur = RGB(color.x, color.y, color.z);
	SetPixel(hdcr, point.x, point.y, kleur);
}


void drawBLine(glm::vec2 point0, glm::vec2 point1) {
	int x0 = point0.x;
	int y0 = point0.y;
	int x1 = point1.x;
	int y1 = point1.y;

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;
	while (true) {
		drawPoint(glm::vec2(x0, y0), glm::vec3(0,0,0));
		if ((x0 == x1) && (y0 == y1)) break;
		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; x0 += sx; }
		if (e2 < dx) { err += dx; y0 += sy; }
	}

}

void Render(Camera c, Mesh* m)
{
	//Make a vector which points up (y)
	glm::vec3 yvec = glm::vec3(0, 1, 0);
	//Create a viewMatrix from the cameras position and target
	glm::mat4 viewMatrix = glm::lookAtLH(c.Position, c.Target, yvec);
	//Create a projection matrix for converting a vector to a point on screen
	glm::mat4 projMatrix = glm::perspectiveFovLH(0.78f, (float)width, (float)height, 0.01f, 1.0f);

	if (m != nullptr) {
		//Create a worldMatrix to transform an objects vectors to that of the world also rotate it
		glm::mat4 worldMatrix = glm::toMat4(glm::quat(m->Rotation)) * glm::translate(glm::mat4(1.0f), m->Position);
		//Create the final transformation matrix which can be applied to every object
		glm::mat4 transformMatrix = projMatrix * viewMatrix * worldMatrix;

		for (int i = 0; i < m->faceCount - 1; i++) {
			Face f = m->Faces[i];
			glm::vec2 point0 = project(m->Vertices[f.A], transformMatrix);
			glm::vec2 point1 = project(m->Vertices[f.B], transformMatrix);
			glm::vec2 point2 = project(m->Vertices[f.C], transformMatrix);
			//Project the vertice to a point on screen

			drawBLine(point0, point1);
			drawBLine(point1, point2);
			drawBLine(point2, point0);
			//drawPoint(point0);
		}
	}
}

void Rotate(Mesh* m, float x) {
	m->Rotation = glm::vec3(m->Rotation.x + x, m->Rotation.y + x, m->Rotation.z);
}

float rotation = 0.01f;
double lastTime, lastRenderTime;
double desiredFps = 144.0;

void StartRendering(HDC hdc, Camera* c, Mesh** m, int allocated) {

	while (true) {
		double currentTime = GetCurrentTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		
		//if (lastRenderTime <= currentTime - (1000.0 / desiredFps)) {
		int i = 0;
		while (i < allocated) {
			double renderDelta = currentTime - lastRenderTime;
			lastRenderTime = currentTime;
			//if(frame % 2)
			Rectangle(hdcr, 0, 0, width, height);
			Rotate(m[i], rotation);
			Render((*c), m[i]);
			i++;
		}
	}
}

void MainRender(HDC hdc, int w, int h)
{
	hdcr = hdc;
	width = w;
	height = h;
	Mesh* meshes[1024];
	meshes[0] = LoadMesh("resources\\monkey.babylon");
	
	Camera* c = &Camera();
	c->Position = glm::vec3(0, 0, 10.0f);
	c->Target = glm::vec3(0, 0, 0);
	
	StartRendering(hdc, c, meshes, 1);
	delete meshes;
}

