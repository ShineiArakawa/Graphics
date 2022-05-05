#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH = 500;              // ウィンドウの幅 / Window width
static const int WIN_HEIGHT = 500;              // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title

// ユーザ定義のOpenGLの初期化
// User-defined OpenGL initialization
void initializeGL() {
    // 背景色の設定
    // Specify background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// ユーザ定義のOpenGL描画
// User-defined OpenGL drawing
void paintGL() {
    // 背景色の描画
    // Background color
    glClear(GL_COLOR_BUFFER_BIT);

    // 三角形の描画
    // Draw a triangle
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 0.0f, 0.0f);  // 赤 / Red
    glVertex2f(-0.5f, -0.5f);
    glColor3f(1.0f,0.0f, 0.0f);  // 緑 / Green
    glVertex2f(-0.5f, 1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);  // 青 / Blue
    glVertex2f(0.5f, -0.5f);

    //glVertex2f(-0.5f, -0.5f);
    //glColor3f(1.0f, 0.0f, 0.0f);  // 赤 / Red
    //glVertex2f(-0.5f, 0.5f);
    //glColor3f(0.0f, 1.0f, 0.0f);  // 緑 / Green
    //glVertex2f(0.5f, -0.5f);
    //glColor3f(0.0f, 0.0f, 1.0f);  // 青 / Blue

    glEnd();
}

int main(int argc, char** argv) {
    // OpenGLを初期化する
    // Initialize OpenGL
    if (glfwInit() == GLFW_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

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