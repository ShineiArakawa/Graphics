#include <cstdio>
#include <vector>
#include <array>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <chrono>
#include <string>
#include <iostream>

static int WIN_WIDTH = 500;                      // ウィンドウの幅 / Window width
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title
static const int NUM_CUBES_1_DIM = 10;

static float theta = 0.0f;
static bool isRunning = true;

// clang-format off
static const glm::vec3 positions[8] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f,  1.0f)
};

static const glm::vec3 colors[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // 赤
	glm::vec3(0.0f, 1.0f, 0.0f),  // 緑
	glm::vec3(0.0f, 0.0f, 1.0f),  // 青
	glm::vec3(1.0f, 1.0f, 0.0f),  // イエロー
	glm::vec3(0.0f, 1.0f, 1.0f),  // シアン
	glm::vec3(1.0f, 0.0f, 1.0f),  // マゼンタ
};

static const unsigned int faces[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 },
	{ 2, 4, 7 }, { 2, 7, 5 },
	{ 5, 7, 6 }, { 5, 6, 3 },
	{ 4, 2, 0 }, { 4, 0, 1 },
	{ 3, 6, 1 }, { 3, 1, 0 },
	{ 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& color_)
		: position(position_)
		, color(color_) {
	}

	glm::vec3 position;
	glm::vec3 color;
};

void drawBox(glm::vec3 centerCoord) {
	GLuint vertexBufferId;
	GLuint indexBufferId;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;

	for (int face = 0; face < 6; face++) {
		for (int i = 0; i < 3; i++) {
			vertices.push_back(Vertex(positions[faces[face * 2 + 0][i]] + centerCoord, colors[face]));
			indices.push_back(idx++);
		}

		for (int i = 0; i < 3; i++) {
			vertices.push_back(Vertex(positions[faces[face * 2 + 1][i]] + centerCoord, colors[face]));
			indices.push_back(idx++);
		}
	}

	// 頂点バッファオブジェクトの作成
	// Create vertex buffer object
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);

	// 頂点番号バッファオブジェクトの作成
	// Create index buffer object
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(50.0f, 50.0f, 50.0f,   // 視点の位置 / Eye position
		0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
		0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

	glm::vec3 centerCoord;
	for (int i = 0; i < NUM_CUBES_1_DIM; i++) {
		for (int j = 0; j < NUM_CUBES_1_DIM; j++) {
			for (int k = 0; k < NUM_CUBES_1_DIM; k++) {
				centerCoord[0] = 2.5f * (float)i;
				centerCoord[1] = 2.5f * (float)j;
				centerCoord[2] = 2.5f * (float)k;

				glPushMatrix();
				drawBox(centerCoord);
				glPopMatrix();
			}
		}
	}
}

void animate(GLFWwindow* window) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		isRunning = false;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		isRunning = true;
	}

	if (isRunning) {
		theta += 1.0;
	}
}

void mouseButtonGL() {

}

void resizeGL(GLFWwindow* window, int width, int height) {
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

int main(int argc, char** argv) {
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Window creation failed!\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));


	glfwSetWindowSizeCallback(window, resizeGL);

	initializeGL();

	double startTime, endTime;
	int counter = 0;
	const int maxFrame = 500;
	double sumElapsedTime = 0.0;

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		if (counter >= maxFrame) {
			break;
		}

		startTime = glfwGetTime();
		paintGL();
		endTime = glfwGetTime();

		// animate(window);

		glfwSwapBuffers(window);
		glfwPollEvents();

		double elapsedTime = endTime - startTime;
		sumElapsedTime += elapsedTime;
		std::cout << std::to_string(counter + 1) + ": elapsedTime= " + std::to_string(elapsedTime) + " [sec]" << std::endl;
		counter++;
	}

	double avgElapsedTime = sumElapsedTime / (double)counter;
	std::cout << "avgElapsedTime= " + std::to_string(avgElapsedTime) + " [sec]" << std::endl;

	glfwDestroyWindow(window);
	glfwTerminate();
}