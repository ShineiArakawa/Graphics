#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_GLU

#include <cstdio>
#include <vector>

#include <math.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>


static int WIN_WIDTH = 500;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title
static const int NUM_CUBES_1_DIM = 10;
static const int NUM_PREMITIVES_TABLE = 3600;
static const double RADIUS = 10.0;
static int hour = -1;
static int minute = -1;
static int second = -1;

// ���_�N���X
// Vertex class
struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& color_)
		: position(position_)
		, color(color_) {
	}

	glm::vec3 position;
	glm::vec3 color;
};

// clang-format off

static const float handColors[3][3] = {
	{1.0f, 0.0f, 0.0f},  // ��
	{0.0f, 1.0f, 0.0f},  // ��
	{0.0f, 0.0f, 1.0f},  // ��
};
// clang-format on

GLuint vertexBufferId;
GLuint indexBufferId;

void updateTime(void) {
	time_t time_type;
	tm* time_obj;

	time(&time_type);
	time_obj = localtime(&time_type);

	hour = time_obj->tm_hour;
	minute = time_obj->tm_min;
	second = time_obj->tm_sec;

	//printf("���݂̎�����%d��%d��%d�b�ł��B\n", hour, minute, second);
}

void drawHand(const float color[], float width) {
	static const float handPositions[4][3] = {
		{ width / 2.0f, (float)RADIUS, -0.1f },
		{ width / 2.0f, 0.0f, -0.1f },
		{ -width / 2.0f, (float)RADIUS, -0.1f },
		{ -width / 2.0f, 0.0f, -0.1f }
	};


	glBegin(GL_TRIANGLES);

	glColor3fv(color);

	for (int i = 0; i < 3; i++) {
		glVertex3fv(handPositions[i]);
	}

	for (int i = 0; i < 3; i++) {
		glVertex3fv(handPositions[i + 1]);
	}

	glEnd();
}

void initializeGL() {
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// ���v�̕����Ղ͒��_�o�b�t�@��p���ĕ`�悷��
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	glm::vec3 centerVertex = { 0.0, 0.0, 0.0 };
	vertices.push_back(Vertex(centerVertex, color));
	glm::vec3 beginVertex = { RADIUS, 0.0, 0.0 };
	vertices.push_back(Vertex(beginVertex, color));

	double diffTheta = (double)(2 * M_PI / NUM_PREMITIVES_TABLE);
	double theta = 0.0;
	glm::vec3 vertex;
	for (int i = 0; i < NUM_PREMITIVES_TABLE; i++) {
		theta += (double)diffTheta * i;
		vertex = { RADIUS * std::cos(theta), RADIUS * std::sin(theta), 0.0 };

		vertices.push_back(Vertex(vertex, color));
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);
}

void paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, -30.0f,   // ���_�̈ʒu / Eye position
		0.0f, 0.0f, 0.0f,   // ���Ă���� / Looking position
		0.0f, 1.0f, 0.0f);  // ���E�̏���� / Upward direction

	float angle;
	// ===================================================================================
	// ���j�̕`�� ========================================================================
	// ===================================================================================
	angle = 30.0f * ((float)hour + (float)minute / 60.0f);
	glPushMatrix();
	glScaled(0.5, 0.5, 1.0);
	glRotated(angle, 0.0, 0.0, 1.0);
	drawHand(handColors[0], 0.5f);
	 glPopMatrix();

	// ===================================================================================
	// ���j�̕`�� ========================================================================
	// ===================================================================================
	angle = 6.0f * ((float)minute + (float)second / 60.0f);
	glPushMatrix();
	glScaled(0.8, 0.8, 1.0);
	glRotated(angle, 0.0, 0.0, 1.0);
	drawHand(handColors[1], 0.3f);
	glPopMatrix();

	// ===================================================================================
	// �b�j�̕`�� ========================================================================
	// ===================================================================================
	angle = 6.0f * (float)second;
	glPushMatrix();
	glRotated(angle, 0.0, 0.0, 1.0);
	drawHand(handColors[2], 0.1f);
	glPopMatrix();

	// ===================================================================================
	// �����Ղ̕`�� ======================================================================
	// ===================================================================================

	// ���_�o�b�t�@�̗L����
	// Enable vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	// ���_���̏ڍׂ�ݒ�
	// Setup details for vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

	// �O�p�`�̕`��
	// Draw triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, NUM_PREMITIVES_TABLE*3, GL_UNSIGNED_INT, 0);

	// ���_�o�b�t�@�̖�����
	// Disable vertex buffer object
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	if (glfwInit() == GLFW_FALSE) {
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

	// OpenGL 3.x/4.x�̊֐������[�h���� (glfwMakeContextCurrent�̌�łȂ��Ƃ����Ȃ�)
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}

	// �o�[�W�������o�͂��� / Check OpenGL version
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// �E�B���h�E�̃��T�C�Y�������֐��̓o�^
	// Register a callback function for window resizing
	glfwSetWindowSizeCallback(window, resizeGL);

	// ���[�U�w��̏�����
	// User-specified initialization
	initializeGL();

	// ���C�����[�v
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		updateTime();
		paintGL();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// �㏈�� / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}