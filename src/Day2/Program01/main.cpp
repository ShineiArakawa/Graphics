#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH = 500;                // �E�B���h�E�̕� / Window width
static const int WIN_HEIGHT = 500;               // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title

// ���[�U��`��OpenGL�̏�����
// User-define OpenGL initialization
void initializeGL() {
    // �w�i�F�̐ݒ�
    // Specify background color
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

// ���[�U��`��OpenGL�`��
// User-defined OpenGL drawing
void paintGL() {
    // �w�i�F�̕`��
    // Background color
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv) {
    // OpenGL������������
    // Initialize OpenGL
    if (glfwInit() == GLFW_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Window�̍쐬
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // OpenGL�̕`��Ώۂ�window���w��
    // Specify window as an OpenGL context
    glfwMakeContextCurrent(window);

    // ������
    // Initialization
    initializeGL();

    // ���C�����[�v
    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // �`��
        paintGL();

        // �`��p�o�b�t�@�̐؂�ւ�
        // Swap drawing target buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �㏈��
    // Postprocess
    glfwDestroyWindow(window);
    glfwTerminate();
}