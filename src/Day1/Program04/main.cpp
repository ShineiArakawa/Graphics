#include <cstdio>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <array>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

static const int WIN_WIDTH = 500;                // ウィンドウの幅 / Window width
static const int WIN_HEIGHT = 500;               // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title

// clang-format off
// 立方体の頂点位置
static const float positions[8][3] = {
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f }
};

// 立方体の面の色
static const float colors[6][3] = {
	{ 1.0f, 0.0f, 0.0f },  // 赤
	{ 0.0f, 1.0f, 0.0f },  // 緑
	{ 0.0f, 0.0f, 1.0f },  // 青
	{ 1.0f, 1.0f, 0.0f },  // イエロー
	{ 0.0f, 1.0f, 1.0f },  // シアン
	{ 1.0f, 0.0f, 1.0f },  // マゼンタ
};
// clang-format on

// 立方体の面となる三角形の定義
static const unsigned int faces[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 },
	{ 2, 4, 7 }, { 2, 7, 5 },
	{ 5, 7, 6 }, { 5, 6, 3 },
	{ 4, 2, 0 }, { 4, 0, 1 },
	{ 3, 6, 1 }, { 3, 1, 0 },
	{ 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

void calcUnitVector(std::array<float, 3>& vector) {
	float length = vector[0] * vector[0] + \
		vector[1] * vector[1] + \
		vector[2] * vector[2];
	length = std::sqrt(length);

	vector[0] = vector[0] / length;
	vector[1] = vector[1] / length;
	vector[2] = vector[2] / length;
}

void outerProduct(const std::array<float, 3> vec1, const std::array<float, 3> vec2, std::array<float, 3>& result) {
	result[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	result[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	result[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

float innerProduct(const std::array<float, 3> vec1, const  std::array<float, 3> vec2) {
	float innerProduct = vec1[0] * vec2[0];
	innerProduct += vec1[1] * vec2[1];
	innerProduct += vec1[2] * vec2[2];
	return innerProduct;
}

void myLookAt(float xCenter, float yCenter, float zCenter, float xDirection, float yDirection, float zDirection, float xUp, float yUp, float zUp) {
	std::array<float, 3> cameraCoordinateX;
	std::array<float, 3> cameraCoordinateY;
	std::array<float, 3> cameraCoordinateZ;
	std::array<float, 3> vecCenter({ xCenter, yCenter, zCenter });
	std::array<float, 3> vecDirection({ xDirection, yDirection, zDirection });
	std::array<float, 3> vecUp({ xUp, yUp, zUp });

	cameraCoordinateZ[0] = (xDirection - xCenter);
	cameraCoordinateZ[1] = (yDirection - yCenter);
	cameraCoordinateZ[2] = (zDirection - zCenter);
	calcUnitVector(cameraCoordinateZ);
	calcUnitVector(vecUp);
	outerProduct(cameraCoordinateZ, vecUp, cameraCoordinateX);
	outerProduct(cameraCoordinateX, cameraCoordinateZ, cameraCoordinateY);


	float matrix[16] = { 0 };
	matrix[0] = cameraCoordinateX[0];
	matrix[1] = cameraCoordinateY[0];
	matrix[2] = -cameraCoordinateZ[0];
	matrix[4] = cameraCoordinateX[1];
	matrix[5] = cameraCoordinateY[1];
	matrix[6] = -cameraCoordinateZ[1];
	matrix[8] = cameraCoordinateX[2];
	matrix[9] = cameraCoordinateY[2];
	matrix[10] = -cameraCoordinateZ[2];
	matrix[12] = -innerProduct(vecCenter, cameraCoordinateX);
	matrix[13] = -innerProduct(vecCenter, cameraCoordinateY);
	matrix[14] = innerProduct(vecCenter, cameraCoordinateZ);
	matrix[15] = 1.0;

	glMultMatrixf(matrix);
}

void myPerspective(float angle, float aspect, float near, float rear) {
	angle = M_PI * angle / 180.0;

	float height = 2.0 * std::tan(angle / 2.0) * near;
	float width = height * aspect;

	float matrix[16] = { 0 };
	float depth = rear - near;

	matrix[0] = 2.0 * near / width;
	matrix[5] = 2.0 * near / height;
	matrix[10] = -(rear + near) / depth;
	matrix[11] = -1.0;
	matrix[14] = -2.0 * rear * near / depth;

	glMultMatrixf(matrix);
}

void myOrtho(float left, float right, float bottom, float top, float near, float rear) {
	float matrix[16] = { 0 };
	float width = right - left;
	float height = top - bottom;
	float depth = rear - near;

	matrix[0] = 2.0 / width;
	matrix[5] = 2.0 / height;
	matrix[10] = -2.0 / depth;
	matrix[12] = -(right + left) / width;
	matrix[13] = -(top + bottom) / height;
	matrix[14] = -(rear + near) / depth;
	matrix[15] = 1;

	glMultMatrixf(matrix);
}

void myFrustum(float left, float right, float bottom, float top, float near, float rear) {
	float matrix[16] = { 0 };
	float width = right - left;
	float height = top - bottom;
	float depth = rear - near;

	matrix[0] = 2.0 * near / width;
	matrix[5] = 2.0 * near / height;
	matrix[8] = (right + left) / width;
	matrix[9] = (top + bottom) / height;
	matrix[10] = -(rear + near) / depth;
	matrix[11] = -1.0;
	matrix[14] = -2.0 * rear * near / depth;

	glMultMatrixf(matrix);
}

void initializeGL() {
	// 背景色の設定 (黒)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void paintGL() {
	// 背景色の描画
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ビューポート変換の指定
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	// 投影変換行列
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 平行投影
	glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
	//myOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
	
	// 透視投影
	//glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
	//myFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
	//gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);
	//myPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);

	// モデルビュー変換行列
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(3.0f, 4.0f, 5.0f,   // 視点の位置 / Eye position
	//	0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
	//	0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction
	myLookAt(3.0f, 4.0f, 5.0f,   // 視点の位置 / Eye position
		0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
		0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

	// 立方体の描画
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

int main(int argc, char** argv) {
	if (glfwInit() == GL_FALSE) {
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
	initializeGL();


	// メインループ
	// Main loop
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// 描画
		// Draw
		paintGL();

		// 描画用バッファの切り替え
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 後処理
	// Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}