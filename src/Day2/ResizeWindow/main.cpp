#include <cstdio>
#include <cmath>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>

static int WIN_WIDTH = 500;                      // ウィンドウの幅
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル
static float aspectRatio = (float)WIN_WIDTH / (float)WIN_HEIGHT;

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
static const unsigned int indices[12][3] = {
    { 7, 4, 1 }, { 7, 1, 6 },
    { 2, 4, 7 }, { 2, 7, 5 },
    { 5, 7, 6 }, { 5, 6, 3 },
    { 4, 2, 0 }, { 4, 0, 1 },
    { 3, 6, 1 }, { 3, 1, 0 },
    { 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

void myPerspective(float angle, float near, float rear) {
    angle = M_PI * angle / 180.0;

    float height, width;

    if (aspectRatio > 1.0f) {
        height = 2.0 * std::tan(angle / 2.0) * near;
        width = height * aspectRatio;
    }
    else {
        width = 2.0 * std::tan(angle / 2.0) * near;
        height = width / aspectRatio;
    }


    float matrix[16] = { 0 };
    float depth = rear - near;

    matrix[0] = 2.0 * near / width;
    matrix[5] = 2.0 * near / height;
    matrix[10] = -(rear + near) / depth;
    matrix[11] = -1.0;
    matrix[14] = -2.0 * rear * near / depth;

    glMultMatrixf(matrix);
}

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
    // 背景色の描画
    // Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    // Specify projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 平行投影 / Orthographic projection
    // glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    // 透視投影 / Perspective projection with view frustum
    // glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
    // 透視投影 / Perspective projection
    //gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 10.0f);
    myPerspective(45.0f, 1.0f, 10.0f);

    // モデルビュー変換行列
    // Specify model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,   // 視点の位置 / Eye position
        0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
        0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

// 立方体の描画
// Draw a cube
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        // 面の色
        // Face color
        glColor3fv(colors[face]);

        // 1つの面(四角形)は2つの三角形から成る
        // A square face of a cube consists of two triangles
        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 0][i]]);
        }

        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 1][i]]);
        }
    }
    glEnd();
}

// ウィンドウサイズ変更のコールバック関数
// Callback function for window resizing
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

    aspectRatio = (float)WIN_WIDTH / (float)WIN_HEIGHT;
    std::cout << "aspectRatio= " + std::to_string(aspectRatio) << std::endl;
}

int main(int argc, char** argv) {
    // OpenGLを初期化する
    // OpenGL initialization
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Windowの作成
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
        NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にwindowを指定
    // Specify window as an OpenGL context
    glfwMakeContextCurrent(window);

    // ウィンドウのリサイズを扱う関数の登録
    // Register a callback function for window resizing
    glfwSetWindowSizeCallback(window, resizeGL);

    // ユーザ指定の初期化
    // User-specified initialization
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