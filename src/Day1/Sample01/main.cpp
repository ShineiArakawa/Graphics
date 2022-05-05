#include <cstdio>
#include <GLFW/glfw3.h>
#include <vec3.hpp>
#include <vec4.hpp>
#include <iostream>
#include <string>

constexpr double DURATION_FPS60 = 0.1666;

static const int WIN_WIDTH = 500;                // ウィンドウの幅 / Window width
static const int WIN_HEIGHT = 500;               // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title

// ユーザ定義のOpenGLの初期化
// User-define OpenGL initialization
void initializeGL() {
	// 背景色の設定
	// Specify background color
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

void HSVtoRGB(float H, float S, float V, float& R, float& G, float& B) {
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
		std::cout << "The givem HSV values are not in valid range" << std::endl;
		return;
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}

	R = (r + m) * 255;
	G = (g + m) * 255;
	B = (b + m) * 255;
}

void changeColor(float &H, double& prevTime, double coeff) {
	double currentTime = glfwGetTime();

	H = H + (float)(currentTime - prevTime) * coeff;
	float S = 100.0;
	float V = 100.0;
	float R, G, B;
	
	if (H > 360) {
		H = 0.0;
	}

	std::cout << "H= " + std::to_string(H) << std::endl;

	HSVtoRGB(H, S, V, R, G, B);
	R = R / 255.0;
	G = G / 255.0;
	B = B / 255.0;
	glClearColor(R, G, B, 0.0f);

	prevTime = currentTime;
}

// ユーザ定義のOpenGL描画
// User-defined OpenGL drawing
void paintGL() {
	// 背景色の描画
	// Background color
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv) {
	double time = glfwGetTime();
	double coeff = 30;
	float H = 1.0;

	// OpenGLを初期化する
	// Initialize OpenGL
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	// Windowの作成
	// Create a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Window creation failed!\n");
		glfwTerminate();
		return 1;
	}

	// OpenGLの描画対象にwindowを指定
	// Specify window as an OpenGL context
	glfwMakeContextCurrent(window);

	// 初期化
	// Initialization
	initializeGL();

	// メインループ
	// Main loop
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		// 描画
		changeColor(H, time, coeff);
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