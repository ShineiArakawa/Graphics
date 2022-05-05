#include <cstdio>
#include <vector>

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
	gluLookAt(30.0f, 40.0f, 50.0f,   // 視点の位置 / Eye position
		0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
		0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

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

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		startTime = glfwGetTime();
		paintGL();
		endTime = glfwGetTime();

		glfwSwapBuffers(window);
		glfwPollEvents();

		double elapsedTime = endTime - startTime;
		std::cout << "elapsedTime= " + std::to_string(elapsedTime) + " [sec]" << std::endl;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}