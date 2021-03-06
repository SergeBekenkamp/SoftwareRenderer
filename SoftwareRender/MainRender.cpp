#include <iostream>
#include "mesh.h"
#include "face.h"
#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "mainRenderer.h"
#include "meshLoader.h"
#include "linkedList.h"
#include "math.h"
#include "performance.h"

int screenWidth = 0;
int screenHeight = 0;
HDC hdcr = 0;

byte* backBuffer;
float* depthBuffer;



//Draw the actual point, 
void putPixel(int x, int y, float z, glm::vec4 color) {

	int index = (x + y * screenWidth);
	int index4 = index * 4;
	//if the pixel drawn has a lower depth discard the current draw
	if (depthBuffer[index] < z) {
		return;
	}
	else {
		depthBuffer[index] = z;
		backBuffer[index4] = (byte)(color.x * 255);
		backBuffer[index4 + 1] = (byte)(color.y * 255);
		backBuffer[index4 + 2] = (byte)(color.z * 255);
		backBuffer[index4 + 3] = (byte)(color.w * 255);
	}
}

//draw a point and clip everything outside the width/ height of the screen
void drawPoint(glm::vec3 point, glm::vec4 color) {
	if (point.x >= 0 && point.y >= 0 && point.x < screenWidth && point.y < screenHeight) {
		putPixel(point.x, point.y, point.z, color);
	}
}

void ProcessScanLine(int y, glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pd, glm::vec4 color) {
	// Thanks to current Y, we can compute the gradient to compute others values like
	// the starting X (sx) and ending X (ex) to draw between
	// if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
	float gradient1 = pa.y != pb.y ? (y - pa.y) / (pb.y - pa.y) : 1;
	float gradient2 = pc.y != pd.y ? (y - pc.y) / (pd.y - pc.y) : 1;

	int sx = (int)Interpolate(pa.x, pb.x, gradient1);
	int ex = (int)Interpolate(pc.x, pd.x, gradient2);

	// starting Z & ending Z
	float z1 = Interpolate(pa.z, pb.z, gradient1);
	float z2 = Interpolate(pc.z, pd.z, gradient2);

	// drawing a line from left (sx) to right (ex) 
	for (int x = sx; x < ex; x++)
	{
		float gradient = (x - sx) / (float)(ex - sx);
		float z = Interpolate(z1, z2, gradient);

		drawPoint(glm::vec3(x, y, z), color);
	}
}

float Cross2D(float x0, float y0, float x1, float y1) {
	return x0 * y1 - x1 * y0;
}

float LineSide2D(glm::vec3 p, glm::vec3 lineFrom, glm::vec3 lineTo)
{

	return Cross2D(p.x - lineFrom.x, p.y - lineFrom.y, lineTo.x - lineFrom.x, lineTo.y - lineFrom.y);

}

void DrawTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec4 color)
{
	// Sorting the points in order to always have this order on screen p1, p2 & p3
	// with p1 always up (thus having the Y the lowest possible to be near the top screen)
	// then p2 between p1 & p3
	if (p1.y > p2.y)
	{
		glm::vec3 temp = p2;
		p2 = p1;
		p1 = temp;
	}

	if (p2.y > p3.y)
	{
		glm::vec3 temp = p2;
		p2 = p3;
		p3 = temp;
	}

	if (p1.y > p2.y)
	{
		glm::vec3 temp = p2;
		p2 = p1;
		p1 = temp;
	}

		if (LineSide2D(p2, p1, p3) > 0)
	{
		for (int y = (int)p1.y; y <= (int)p3.y; y++)
		{
			if (y < p2.y)
			{
				ProcessScanLine(y, p1, p3, p1, p2, color);
			}
			else
			{
				ProcessScanLine(y, p1, p3, p2, p3, color);
			}
		}
	}	
	else
	{
		for (int y = (int)p1.y; y <= (int)p3.y; y++)
		{
			if (y < p2.y)
			{
				ProcessScanLine(y, p1, p2, p1, p3, color);
			}
			else
			{
				ProcessScanLine(y, p2, p3, p1, p3, color);
			}
		}
	}
}

void ClearBuffers() {
	for (int i = 0; i < screenWidth * screenHeight * 4; i++) {
		backBuffer[i] = 255;
	}
	for (int i = 0; i < screenWidth* screenHeight; i++) {
		depthBuffer[i] = FLT_MAX;
	}
}


void Render(Camera c, Mesh* m)
{
	//Make a vector which points up (y)
	glm::vec3 yvec = glm::vec3(0, 1, 0);
	//Create a viewMatrix from the cameras position and target
	//c.Target = m[0].Position;
	glm::mat4 viewMatrix = glm::lookAtLH(c.Position, c.Target, yvec);
	//Create a projection matrix for converting a vector to a point on screen
	glm::mat4 projMatrix = glm::perspectiveFovRH(0.78f, (float)screenWidth, (float)screenHeight, 0.01f, 1.0f);
	if (m != nullptr) {
		//Create a worldMatrix to transform an objects vectors to that of the world also rotate it
		glm::mat4 modelMatrix = glm::mat4(1.0f) * glm::eulerAngleXYZ(m->Rotation.x, m->Rotation.y, m->Rotation.z);
		//Create the final transformation matrix which can be applied to every object
		glm::mat4 transformMatrix = modelMatrix * viewMatrix * projMatrix;

		int faceIndex = 0;
		for (int i = 0; i < m->faceCount - 1; i++) {
			Face f = m->Faces[i];
			glm::vec3 point0 = glm::project(m->Vertices[f.A], viewMatrix * modelMatrix, projMatrix, glm::vec4(0, 0, screenWidth, screenHeight));
			glm::vec3 point1 = glm::project(m->Vertices[f.B], viewMatrix * modelMatrix, projMatrix, glm::vec4(0, 0, screenWidth, screenHeight));
			glm::vec3 point2 = glm::project(m->Vertices[f.C], viewMatrix * modelMatrix, projMatrix, glm::vec4(0, 0, screenWidth, screenHeight));
			
			//glm::vec3 point0 = project(m->Vertices[f.A], transformMatrix);
			/*glm::vec3 point1 = project(m->Vertices[f.B], transformMatrix);
			glm::vec3 point2 = project(m->Vertices[f.C], transformMatrix);*/
			//Project the vertice to a point on screen

			float color = 0.25f + (faceIndex % m->faceCount) * 0.75f / m->faceCount;
			DrawTriangle(point0, point1, point2, glm::vec4(color, color, color, 1));
			faceIndex++;
		}
	}
}

//rotate the mesh by x amount
void Rotate(Mesh* m, float x) {
	m->Rotation = glm::vec3(m->Rotation.x, m->Rotation.y + x, m->Rotation.z);
}

float rotation = 0.01f;

void StartRendering(HDC hdc, Camera* c, Mesh** m, int allocated) {

	while (true) {
		speedtest__("Render speed: ") {
			int i = 0;
			while (i < allocated) {
				Rotate(m[i], rotation);
				Render((*c), m[i]);
				i++;
			}

			HBITMAP map = CreateBitmap(screenWidth, screenHeight, 1, 8 * 4, backBuffer);
			HDC src = CreateCompatibleDC(hdc);

			SelectObject(src, map); // Inserting picture into our temp HDC
									// Copy image from temp HDC to window
			BitBlt(hdc, // Destination
				0,  // x and
				0,  // y - upper-left corner of place, where we'd like to copy
				screenWidth, // width of the region
				screenHeight, // height
				src, // source
				0,   // x and
				0,   // y of upper left corner  of part of the source, from where we'd like to copy
				SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;

			DeleteDC(src); // Deleting temp HDC
			ClearBuffers();
		}
	}
}

//does some setup, initializes the meshes to use for the render, creates a camera and sets its position and target
void MainRender(HDC hdc, int w, int h)
{
	hdcr = hdc;
	screenWidth = w;
	screenHeight = h;
	backBuffer = (byte*) malloc(w*h * 4 * sizeof(byte));
	depthBuffer = (float*) malloc(w*h*sizeof(float));

	Mesh* meshes[1024];
	meshes[0] = LoadMesh("resources\\monkey.babylon");
	meshes[1] = Mesh::CreateCube();
	Camera* c = &Camera(glm::vec3(0, 0, 10.0f), glm::vec3(0, 0, 0));

	StartRendering(hdc, c, meshes, 1);
	free(backBuffer);
	free(depthBuffer);
	delete meshes;
}

