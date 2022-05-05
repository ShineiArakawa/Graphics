#include <cstdio>
#include <cmath>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "MeshDrawer.h"

static int WIN_WIDTH = 1920;                      // ウィンドウの幅
static int WIN_HEIGHT = 1080;                     // ウィンドウの高さ
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル

// 立方体の頂点位置
// Vertex positions of a cube
// clang-format off
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
// clang-format on

// 立方体の面の色
// Face colors of a cube
// clang-format off
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
// Vertex indices to form triangles of a cube
// clang-format off
static const unsigned int faces[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 },
	{ 2, 4, 7 }, { 2, 7, 5 },
	{ 5, 7, 6 }, { 5, 6, 3 },
	{ 4, 2, 0 }, { 4, 0, 1 },
	{ 3, 6, 1 }, { 3, 1, 0 },
	{ 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

// OpenGLの初期化関数
// OpenGL initialization
void initializeGL() {
	// 背景色の設定 (黒)
	// Background color (black)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 深度テストの有効化
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
}

// OpenGLの描画関数
// OpenGL drawing function
void paintGL() {
	// 背景色と深度値のクリア
	// Clear background color and depth values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ビューポート変換の指定
	// Viewport transform
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	// 投影変換行列
	// Specify projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 平行投影
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	 
	// 透視投影
	//gluPerspective(130.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);

	// モデルビュー変換行列
	// Specify model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3.0f, 4.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	// 立方体の描画
	// Draw a cube
		//glBegin(GL_TRIANGLES);
		//for (int face = 0; face < 6; face++) {
		//	// 面の色
		//	// Face color
		//	glColor3fv(colors[face]);

		//	// 1つの面(四角形)は2つの三角形から成る
		//	// A square face of a cube consists of two triangles
		//	for (int i = 0; i < 3; i++) {
		//		glVertex3fv(positions[faces[face * 2 + 0][i]]);
		//	}

		//	for (int i = 0; i < 3; i++) {
		//		glVertex3fv(positions[faces[face * 2 + 1][i]]);
		//	}
		//}
		//glEnd();
}

void resizeGL(GLFWwindow* window, int width, int height) {
	// ユーザ管理のウィンドウサイズを変更
	// Update user-managed window size
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	// GLFW管理のウィンドウサイズを変更
	// Update GLFW-managed window size
	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	// 実際のウィンドウサイズ (ピクセル数) を取得
	// Get actual window size by pixels
	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	// ビューポート変換の更新
	// Update viewport transform
	glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

int main(int argc, char** argv) {
	std::string filePathMesh = "C:/cygwin64/home/Insight_Arakawa/Projects/Graphics/data/bun.pch";
	MeshDrawer meshDrawer(filePathMesh);

	// OpenGLを初期化する
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// Windowの作成
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}
	glfwMakeContextCurrent(window);
	initializeGL();
	glfwSetWindowSizeCallback(window, resizeGL);

	// メインループ
	// Main loop
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// 描画 / Draw
		paintGL();
		meshDrawer.draw();

		// 描画用バッファの切り替え
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 後処理 / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}