#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cmath>
#include <string>
#include <iostream>
#include <string>
#include <vector>


#define GLFW_INCLUDE_GLU  // GLU���C�u�������g�p����̂ɕK�v / Required to use GLU
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION  // �摜�̃��[�h�ɕK�v / Required to load images
#include "stb_image.h"

#define GL_CLAMP_TO_EDGE                        0x812F
#define GL_CLAMP                                0x2900
#define GL_CLAMP_TO_BORDER                      0x812D

#include "common.h"

static int WIN_WIDTH = 500;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title

static const double PI = 4.0 * atan(1.0);  // �~�����̒�` / Circular constant
static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "checker.png";
static GLuint textureId = 0u;

static double FPS = 240.0;

// clang-format off
static const float positions[4][3] = {
	{ -1.0f,  0.0f, -1.0f },
	{  1.0f,  0.0f, -1.0f },
	{ -1.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  1.0f },
};

static const float texcoords[4][2] = {
	{  0.0f,  0.0f },
	{  1.0f,  0.0f },
	{  0.0f,  1.0f },
	{  1.0f,  1.0f }
};

static const unsigned int faces[2][3] = {
	{ 0, 1, 3 }, { 0, 3, 2 }
};
// clang-format on


void generateCustomMIP() {
	int nLevels = 9;
	int texWidth, texHeight, channels;

	for (int i = 0; i < nLevels; i++) {
		std::string filePath = std::string(DATA_DIRECTORY) + "level" + std::to_string(i) + ".png";
		std::cout << filePath << std::endl;
		unsigned char* bytes = stbi_load(filePath.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
		if (!bytes) {
			fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
			exit(1);
		}
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, texWidth, texHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		stbi_image_free(bytes);
	}

}

// OpenGL�̏������֐�
void initializeGL() {
	// �w�i�F�̐ݒ� (��)
	// Background color (black)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// �[�x�e�X�g�̗L����
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// �e�N�X�`���̗L����
	// Enable texture mapping
	glEnable(GL_TEXTURE_2D);

	// �e�N�X�`���̐ݒ�
	// Setup texture
	/*int texWidth, texHeight, channels;
	unsigned char* bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
	if (!bytes) {
		fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
		exit(1);
	}*/

	// �e�N�X�`���̐����ƗL����
	// Generate and bind texture
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	generateCustomMIP();

	// �e�N�X�`���̉�f�l�Q�ƕ��@�̐ݒ� (MIP map�Ȃ�)
	// Texture filtering operations (w/o MIP map)
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// �e�N�X�`���̉�f�l�Q�ƕ��@�̐ݒ� (MIP map����)
	// Texture filtering operation (with MIP map)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// �e�N�X�`�����E�̐܂�Ԃ��ݒ�
	// Texture wrapping (to process pixels outside border)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// �e�N�X�`���̖�����
	// Disable texture binding
	glBindTexture(GL_TEXTURE_2D, 0);

	// ���[�h������f���̔j�� (CPU���̃f�[�^��j�����邾���Ȃ̂�GPU��̃e�N�X�`���ɂ͉e�����Ȃ�)
	// Free image data (it's on CPU side, so it does not affect texture on GPU)
	//stbi_image_free(bytes);
}

// OpenGL�̕`��֐�
// OpenGL drawing function
void paintGL() {
	// �w�i�F�Ɛ[�x�l�̃N���A
	// Clear background color and depth values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���e�ϊ��s��
	// Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);

	// ���f���r���[�s��
	// Model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 3.0f, 5.0f,   // ���_�̈ʒu / Eye position
		0.0f, 0.0f, 0.0f,   // ���Ă���� / Looking direction
		0.0f, 1.0f, 0.0f);  // ���E�̏���� / Upward direction

	// �O��Ɉړ� / Move back and forth
	glTranslatef(0.0f, 0.0f, std::cos(PI * theta / 180.0f) * 4.0f - 4.0f);
	// Y�����S��theta������] / Rotate by theta around Y axis
	glRotatef(theta, 0.0f, 1.0f, 0.0f);

	// ���ʂ̕`��
	// Draw a flat plane
	glBindTexture(GL_TEXTURE_2D, textureId);  // �e�N�X�`���̗L���� / Enable texture mapping
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 3; i++) {
		glTexCoord2fv(texcoords[faces[0][i]]);
		glVertex3fv(positions[faces[0][i]]);
	}

	for (int i = 0; i < 3; i++) {
		glTexCoord2fv(texcoords[faces[1][i]]);
		glVertex3fv(positions[faces[1][i]]);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);  // �e�N�X�`���̖����� / Disable texture mapping
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

// �A�j���[�V�����̂��߂̃A�b�v�f�[�g
// Update for animating object
void animate() {
	theta += 1.0f;  // 1�x������] / Rotate by 1 degree of angle
}

int main(int argc, char** argv) {
	// OpenGL������������
	// OpenGL initialization
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// Window�̍쐬
	// Create a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}

	// OpenGL�̕`��Ώۂ�Window��ǉ�
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
	double prevTime, currentTime, diffTime;
	prevTime = glfwGetTime();
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		// �`�� / Draw
		currentTime = glfwGetTime();
		diffTime = currentTime - prevTime;
		if (diffTime >= 1.0 / FPS) {
			double realFPS = 1.0 / diffTime;
			char winTitle[256];
			sprintf(winTitle, "%s (%.3f)", WIN_TITLE, realFPS);
			glfwSetWindowTitle(window, winTitle);

			paintGL();

			// �A�j���[�V���� / Animation
			animate();

			// �`��p�o�b�t�@�̐؂�ւ�
			// Swap drawing target buffers
			glfwSwapBuffers(window);
			glfwPollEvents();

			prevTime = currentTime;
		}
	}

	// �㏈�� / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}