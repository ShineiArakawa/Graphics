#include <cstdio>
#include <cmath>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "MeshDrawer.h"

static int WIN_WIDTH = 1920;                      // �E�B���h�E�̕�
static int WIN_HEIGHT = 1080;                     // �E�B���h�E�̍���
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
static const unsigned int faces[12][3] = {
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
	// �w�i�F�Ɛ[�x�l�̃N���A
	// Clear background color and depth values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// �r���[�|�[�g�ϊ��̎w��
	// Viewport transform
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	// ���e�ϊ��s��
	// Specify projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// ���s���e
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	 
	// �������e
	//gluPerspective(130.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);

	// ���f���r���[�ϊ��s��
	// Specify model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3.0f, 4.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	// �����̂̕`��
	// Draw a cube
		//glBegin(GL_TRIANGLES);
		//for (int face = 0; face < 6; face++) {
		//	// �ʂ̐F
		//	// Face color
		//	glColor3fv(colors[face]);

		//	// 1�̖�(�l�p�`)��2�̎O�p�`���琬��
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
	std::string filePathMesh = "C:/cygwin64/home/Insight_Arakawa/Projects/Graphics/data/bun.pch";
	MeshDrawer meshDrawer(filePathMesh);

	// OpenGL������������
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// Window�̍쐬
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

	// ���C�����[�v
	// Main loop
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// �`�� / Draw
		paintGL();
		meshDrawer.draw();

		// �`��p�o�b�t�@�̐؂�ւ�
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// �㏈�� / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}