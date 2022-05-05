#include <cstdio>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

static int WIN_WIDTH = 500;                      // ウィンドウの幅 / Window width
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title
static const int NUM_CUBES_1_DIM = 10;

static float theta = 0.0f;
static bool isRunning = true;

// clang-format off
static const float positions[8][3] = {
	{ -1.0f, -1.0f, -1.0f},
	{  1.0f, -1.0f, -1.0f},
	{ -1.0f,  1.0f, -1.0f},
	{ -1.0f, -1.0f,  1.0f},
	{  1.0f,  1.0f, -1.0f},
	{ -1.0f,  1.0f,  1.0f},
	{  1.0f, -1.0f,  1.0f},
	{  1.0f,  1.0f,  1.0f}
};

static const float colors[6][3] = {
	{ 1.0f, 0.0f, 0.0f },  // 赤
	{ 0.0f, 1.0f, 0.0f },  // 緑
	{ 0.0f, 0.0f, 1.0f },  // 青
	{ 1.0f, 1.0f, 0.0f },  // イエロー
	{ 0.0f, 1.0f, 1.0f },  // シアン
	{ 1.0f, 0.0f, 1.0f },  // マゼンタ
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

void drawBox() {
	glBegin(GL_TRIANGLES);
	for (int face = 0; face < 6; face++) {
		// 面の色
		// Face color
		glColor3fv(colors[face]);

		// 1つの面(四角形)は2つの三角形から成る
		// A square face of a cube consists of two triangles
		for (int i = 0; i < 3; i++) {
			glVertex3fv(positions[faces[face * 2 + 0][i]]);
		}

		for (int i = 0; i < 3; i++) {
			glVertex3fv(positions[faces[face * 2 + 1][i]]);
		}
	}
	glEnd();
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

	for (int i = 0; i < NUM_CUBES_1_DIM; i++) {
		for (int j = 0; j < NUM_CUBES_1_DIM; j++) {
			for (int k = 0; k < NUM_CUBES_1_DIM; k++) {
				float centerX = 2.5 * i;
				float centerY = 2.5 * j;
				float centerZ = 2.5 * k;

				glPushMatrix();
				glTranslatef(centerX, centerY, centerZ);
				glRotated(2.0f * theta, 0.0f, 1.0f, 0.0f);
				glRotatef(theta, 1.0f, 0.0f, 0.0f);
				glScalef(0.5f, 0.5f, 0.5f);
				drawBox();
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

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {

		startTime = glfwGetTime();
		paintGL();
		endTime = glfwGetTime();

		animate(window);

		glfwSwapBuffers(window);
		glfwPollEvents();

		double elapsedTime = endTime - startTime;
		std::cout << "elapsedTime= " + std::to_string(elapsedTime) + " [sec]" << std::endl;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}