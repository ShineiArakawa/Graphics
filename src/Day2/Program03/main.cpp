#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH = 500;                // �E�B���h�E�̕� / Window width
static const int WIN_HEIGHT = 500;               // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title

// ���[�U��`��OpenGL�̏�����
// User-defined OpenGL initialization
void initializeGL() {
    // �w�i�F�̐ݒ�
    // Specify background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// ���[�U��`��OpenGL�`��
// User-defined OpenGL drawing
void paintGL() {
    // �w�i�F�̕`��
    // Background color
    glClear(GL_COLOR_BUFFER_BIT);

    // ���W�ϊ��̎w��
    // Specify coordinate transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glScalef(0.5f, 0.5f, 1.0f);  // �g��E�k�� / Scaling
    // glTranslatef(1.0f, 1.0f, 0.0f);  // ���s�ړ� / Translation
    // glRotatef(45.0f, 0.0f, 0.0f, 1.0f);  // ��] / Rotation
    glOrtho(-3.0f, 1.0f, -3.0f, 1.0f, -1.0f, 1.0f);

    // �O�p�`�̕`��
    // Draw a triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);  // �� / Red
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);  // �� / Green
    glVertex2f(-0.5f, 0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);  // �� / Blue
    glVertex2f(0.5f, -0.5f);
    glEnd();
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
    double time = glfwGetTime();
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // �`��
        // Draw
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