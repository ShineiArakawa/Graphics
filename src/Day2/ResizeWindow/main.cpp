#include <cstdio>
#include <cmath>

#define GLFW_INCLUDE_GLU  // GLU���C�u�������g�p����̂ɕK�v
#include <GLFW/glfw3.h>

static int WIN_WIDTH = 500;                      // �E�B���h�E�̕�
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍���
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g��

// �����̂̒��_�ʒu
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

// �����̖̂ʂ̐F
// Face colors of a cube
// clang-format off
static const float colors[6][3] = {
    { 1.0f, 0.0f, 0.0f },  // ��
    { 0.0f, 1.0f, 0.0f },  // ��
    { 0.0f, 0.0f, 1.0f },  // ��
    { 1.0f, 1.0f, 0.0f },  // �C�G���[
    { 0.0f, 1.0f, 1.0f },  // �V�A��
    { 1.0f, 0.0f, 1.0f },  // �}�[���^
};
// clang-format on

// �����̖̂ʂƂȂ�O�p�`�̒�`
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

// OpenGL�̏������֐�
// OpenGL initialization
void initializeGL() {
    // �w�i�F�̐ݒ� (��)
    // Background color (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // �[�x�e�X�g�̗L����
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

// OpenGL�̕`��֐�
// OpenGL drawing function
void paintGL() {
    // �w�i�F�̕`��
    // Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ���e�ϊ��s��
    // Specify projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // ���s���e / Orthographic projection
    // glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    // �������e / Perspective projection with view frustum
    // glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
    // �������e / Perspective projection
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 10.0f);

    // ���f���r���[�ϊ��s��
    // Specify model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,   // ���_�̈ʒu / Eye position
        0.0f, 0.0f, 0.0f,   // ���Ă���� / Looking position
        0.0f, 1.0f, 0.0f);  // ���E�̏���� / Upward direction

// �����̂̕`��
// Draw a cube
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        // �ʂ̐F
        // Face color
        glColor3fv(colors[face]);

        // 1�̖�(�l�p�`)��2�̎O�p�`���琬��
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

// �E�B���h�E�T�C�Y�ύX�̃R�[���o�b�N�֐�
// Callback function for window resizing
void resizeGL(GLFWwindow* window, int width, int height) {
    // ���[�U�Ǘ��̃E�B���h�E�T�C�Y��ύX
    // Update user-managed window size
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    // GLFW�Ǘ��̃E�B���h�E�T�C�Y��ύX
    // Update GLFW-managed window size
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

    // ���ۂ̃E�B���h�E�T�C�Y (�s�N�Z����) ���擾
    // Get actual window size by pixels
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

    // �r���[�|�[�g�ϊ��̍X�V
    // Update viewport transform
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

int main(int argc, char** argv) {
    // OpenGL������������
    // OpenGL initialization
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Window�̍쐬
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
        NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // OpenGL�̕`��Ώۂ�window���w��
    // Specify window as an OpenGL context
    glfwMakeContextCurrent(window);

    // �E�B���h�E�̃��T�C�Y�������֐��̓o�^
    // Register a callback function for window resizing
    glfwSetWindowSizeCallback(window, resizeGL);

    // ���[�U�w��̏�����
    // User-specified initialization
    initializeGL();

    // ���C�����[�v
    // Main loop
    while (glfwWindowShouldClose(window) == GL_FALSE) {
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