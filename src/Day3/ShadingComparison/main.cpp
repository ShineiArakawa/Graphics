#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS        // ���W�A���P�ʂ̊p�x���g�����Ƃ���������
#define GLM_ENABLE_EXPERIMENTAL  // glm/gtx/**.hpp���g���̂ɕK�v
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "common.h"

static int WIN_WIDTH = 500;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title
static const int FPS = 120;

static std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";

static const int NUM_DIVISIONS_THETA = 1000;
static const int NUM_DIVISIONS_PHI = 1000;
static const float RADIUS = 3.0f;

static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
static const glm::vec3 diffColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
static const glm::vec3 specColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
static const glm::vec3 ambiColor = glm::vec3(0.24725f, 0.1995f, 0.0745f);
static const float shininess = 51.2f;

GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

GLuint programId;
GLuint vertShaderId;
GLuint fragShaderId;

static float theta = 0.0f;
static int algorithmID = 0;

struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& normal_)
		: position(position_)
		, normal(normal_) {
	}

	glm::vec3 position;
	glm::vec3 normal;
};


void cartecian2Polar(glm::vec3& vec, float radius, float theta, float phi) {
	vec[0] = radius * std::sin(theta) * std::cos(phi);
	vec[1] = radius * std::sin(theta) * std::sin(phi);
	vec[2] = radius * std::cos(theta);
}

void initVAO() {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;

	float deltaTheta = M_PI / (float)NUM_DIVISIONS_THETA;
	float deltaPhi = 2.0f * M_PI / (float)NUM_DIVISIONS_PHI;

	glm::vec3 pos1, pos2, pos3, pos4, normal;
	for (int i = 0; i < NUM_DIVISIONS_THETA; i++) {
		float theta1 = (float)i * deltaTheta;
		float theta2 = (float)(i + 1) * deltaTheta;
		for (int j = 0; j < NUM_DIVISIONS_PHI; j++) {
			float phi1 = (float)j * deltaPhi;
			float phi2 = (float)(j + 1) * deltaPhi;

			cartecian2Polar(pos1, RADIUS, theta1, phi1);
			cartecian2Polar(pos2, RADIUS, theta1, phi2);
			cartecian2Polar(pos3, RADIUS, theta2, phi1);
			cartecian2Polar(pos4, RADIUS, theta2, phi2);

			cartecian2Polar(normal, 1.0, theta1 + deltaTheta / 2.0f, phi1 + deltaPhi / 2.0f);

			Vertex v1(pos1, normal);
			vertices.push_back(v1);
			indices.push_back(idx++);

			Vertex v2(pos2, normal);
			vertices.push_back(v2);
			indices.push_back(idx++);

			Vertex v3(pos3, normal);
			vertices.push_back(v3);
			indices.push_back(idx++);

			Vertex v21(pos2, normal);
			vertices.push_back(v21);
			indices.push_back(idx++);

			Vertex v31(pos3, normal);
			vertices.push_back(v31);
			indices.push_back(idx++);

			Vertex v4(pos4, normal);
			vertices.push_back(v4);
			indices.push_back(idx++);
		}
	}

	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

// �V�F�[�_�̃\�[�X�t�@�C�����R���p�C������
// Compile a shader source
GLuint compileShader(const std::string& filename, GLuint type) {
	// �V�F�[�_�̍쐬
	// Create a shader
	GLuint shaderId = glCreateShader(type);

	// �t�@�C���̓ǂݍ���
	// Load source file
	std::ifstream reader;
	std::string code;

	// �t�@�C�����J��
	// Open source file
	reader.open(filename.c_str(), std::ios::in);
	if (!reader.is_open()) {
		// �t�@�C�����J���Ȃ�������G���[���o���ďI��
		// Finish with error message if source file could not be opened
		fprintf(stderr, "Failed to load a shader: %s\n", filename.c_str());
		exit(1);
	}

	// �t�@�C�������ׂēǂ�ŕϐ��Ɋi�[
	// Load entire contents of a file and store to a string variable
	{
		// �t�@�C���ǂݎ��ʒu���I�[�Ɉړ� / Move seek position to the end
		reader.seekg(0, std::ios::end);
		// �R�[�h���i�[����ϐ��̑傫����\�� / Reserve memory location for code characters
		code.reserve(reader.tellg());
		// �t�@�C���̓ǂݎ��ʒu��擪�Ɉړ� / Move seek position back to the beginning
		reader.seekg(0, std::ios::beg);

		// �擪����t�@�C���T�C�Y����ǂ�ŃR�[�h�̕ϐ��Ɋi�[
		// Load entire file and copy to "code" variable
		code.assign(std::istreambuf_iterator<char>(reader),
			std::istreambuf_iterator<char>());
	}

	// �t�@�C�������
	// Close file
	reader.close();

	// �R�[�h�̃R���p�C��
	// Compile a source code
	const char* codeChars = code.c_str();
	glShaderSource(shaderId, 1, &codeChars, NULL);
	glCompileShader(shaderId);

	// �R���p�C���̐��ۂ𔻒肷��
	// Check whther compile is successful
	GLint compileStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		// �R���p�C�������s������G���[���b�Z�[�W�ƃ\�[�X�R�[�h��\�����ďI��
		// Terminate with error message if compilation failed
		fprintf(stderr, "Failed to compile a shader!\n");

		// �G���[���b�Z�[�W�̒������擾����
		// Get length of error message
		GLint logLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			// �G���[���b�Z�[�W���擾����
			// Get error message
			GLsizei length;
			std::string errMsg;
			errMsg.resize(logLength);
			glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

			// �G���[���b�Z�[�W�ƃ\�[�X�R�[�h�̏o��
			// Print error message and corresponding source code
			fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
			fprintf(stderr, "%s\n", code.c_str());
		}
		exit(1);
	}

	return shaderId;
}

// �V�F�[�_�v���O�����̃r���h (=�R���p�C���{�����N)
// Build a shader program (build = compile + link)
GLuint buildShaderProgram(const std::string& vShaderFile, const std::string& fShaderFile) {
	// �e��V�F�[�_�̃R���p�C��
	// Compile shader files
	vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
	fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);

	// �V�F�[�_�v���O�����ւ̃����N
	// Link shader objects to the program
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertShaderId);
	glAttachShader(programId, fragShaderId);
	glLinkProgram(programId);

	// �����N�̐��ۂ𔻒肷��
	// Check whether link is successful
	GLint linkState;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
	if (linkState == GL_FALSE) {
		// �����N�Ɏ��s������G���[���b�Z�[�W��\�����ďI��
		// Terminate with error message if link is failed
		fprintf(stderr, "Failed to link shaders!\n");

		// �G���[���b�Z�[�W�̒������擾����
		// Get length of error message
		GLint logLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			// �G���[���b�Z�[�W���擾����
			// Get error message
			GLsizei length;
			std::string errMsg;
			errMsg.resize(logLength);
			glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

			// �G���[���b�Z�[�W���o�͂���
			// Print error message
			fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
		}
		exit(1);
	}

	// �V�F�[�_�𖳌����������ID��Ԃ�
	// Disable shader program and return its ID
	glUseProgram(0);
	return programId;
}

// �V�F�[�_�̏�����
// Initialization related to shader programs
void initShaders() {
	programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}


void initializeGL() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initVAO();
	initShaders();
}

void paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
		(float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glm::mat4 viewMat = glm::lookAt(glm::vec3(8.0f, 8.0f, 8.0f),   // ���_�̈ʒu
		glm::vec3(0.0f, 0.0f, 0.0f),   // ���Ă����
		glm::vec3(0.0f, 1.0f, 0.0f));  // ���E�̏����

	glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mvMat = viewMat * modelMat;
	glm::mat4 mvpMat = projMat * viewMat * modelMat;
	glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
	glm::mat4 lightMat = viewMat;

	glUseProgram(programId);

	GLuint uid;
	uid = glGetUniformLocation(programId, "u_mvMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
	uid = glGetUniformLocation(programId, "u_mvpMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
	uid = glGetUniformLocation(programId, "u_normMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(normMat));
	uid = glGetUniformLocation(programId, "u_lightMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));

	uid = glGetUniformLocation(programId, "u_lightPos");
	glUniform3fv(uid, 1, glm::value_ptr(lightPos));
	uid = glGetUniformLocation(programId, "u_diffColor");
	glUniform3fv(uid, 1, glm::value_ptr(diffColor));
	uid = glGetUniformLocation(programId, "u_specColor");
	glUniform3fv(uid, 1, glm::value_ptr(specColor));
	uid = glGetUniformLocation(programId, "u_ambiColor");
	glUniform3fv(uid, 1, glm::value_ptr(ambiColor));
	uid = glGetUniformLocation(programId, "u_shininess");
	glUniform1f(uid, shininess);
	uid = glGetUniformLocation(programId, "u_algorithmID");
	glUniform1i(uid, algorithmID);


	glBindVertexArray(vaoId);
	glDrawElements(GL_TRIANGLES, 6 * NUM_DIVISIONS_THETA * NUM_DIVISIONS_PHI, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

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
// Update parameters for animation
void animate() {
	theta += 1.0f;  // 1�x������] / Rotate 1 degree of angle
}

void setWindowTitle(GLFWwindow* window, const std::string strTitle) {
	const char* title = strTitle.c_str();
	char winTitle[256];
	sprintf(winTitle, "%s (Mode: %s)", WIN_TITLE, title);
	glfwSetWindowTitle(window, winTitle);
}

void keyboardActionPerformed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::string title;

	if (action == GLFW_PRESS && key == GLFW_KEY_1) {
		algorithmID = 1;

		title = "Blinn-Phong Shading (Phong Shading)";
		std::cout << title << std::endl;
		setWindowTitle(window, title);
	} else if (action == GLFW_PRESS && key == GLFW_KEY_2) {
		algorithmID = 2;

		title = "Blinn-Phong Shading (Gouraud Shading)";
		std::cout << title << std::endl;
		setWindowTitle(window, title);
	}
	else if (action == GLFW_PRESS) {
		algorithmID = 0;

		title = "Normal Shading";
		std::cout << title << std::endl;
		setWindowTitle(window, title);
	}
}

int main(int argc, char** argv) {
	// OpenGL������������
	// OpenGL initialization
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// OpenGL�̃o�[�W�����ݒ� (Mac�̏ꍇ�ɂ͕K���K�v)
	// Specify OpenGL version (mandatory for Mac)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window�̍쐬
	// Create a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}

	// OpenGL�̕`��Ώۂ�window���w��
	// Specify window as an OpenGL context
	glfwMakeContextCurrent(window);
	setWindowTitle(window, std::string("Normal Shading"));

	// OpenGL 3.x/4.x�̊֐������[�h���� (glfwMakeContextCurrent�̌�łȂ��Ƃ����Ȃ�)
	// Load OpenGL 3.x/4.x methods (must be loaded after "glfwMakeContextCurrent")
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}

	// �o�[�W�������o�͂��� / Check OpenGL version
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glfwSetWindowSizeCallback(window, resizeGL);
	glfwSetKeyCallback(window, keyboardActionPerformed);
	initializeGL();

	double prevTime, currentTIme;
	prevTime = glfwGetTime();
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		currentTIme = glfwGetTime();
		if ((currentTIme - prevTime) >= 1.0 / (double)FPS) {

			paintGL();
			// �A�j���[�V���� / Animation
			animate();

			// �`��p�o�b�t�@�̐؂�ւ�
			// Swap drawing target buffers
			glfwSwapBuffers(window);
			glfwPollEvents();

			prevTime = currentTIme;
		}

	}

	// �㏈�� / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}