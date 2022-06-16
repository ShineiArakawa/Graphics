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
// glm::vec�^���|�C���^�ɕϊ� / Convert glm::vec types to pointer
#include <glm/gtc/type_ptr.hpp>
// GLM�̍s��ϊ��̂��߂̃��[�e�B���e�B�֐� GLM's utility functions for matrix transformation
#include <glm/gtx/transform.hpp>

// �摜�̃p�X�Ȃǂ������ꂽ�ݒ�t�@�C��
// Config file storing image locations etc.
#include "common.h"

static int WIN_WIDTH = 500;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title


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
static const glm::vec3 positions[8] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f,  1.0f)
};

static const glm::vec3 colors[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // ��
	glm::vec3(0.0f, 1.0f, 0.0f),  // ��
	glm::vec3(0.0f, 0.0f, 1.0f),  // ��
	glm::vec3(1.0f, 1.0f, 0.0f),  // �C�G���[
	glm::vec3(0.0f, 1.0f, 1.0f),  // �V�A��
	glm::vec3(1.0f, 0.0f, 1.0f),  // �}�[���^
};

static const unsigned int faces[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 },
	{ 2, 4, 7 }, { 2, 7, 5 },
	{ 5, 7, 6 }, { 5, 6, 3 },
	{ 4, 2, 0 }, { 4, 0, 1 },
	{ 3, 6, 1 }, { 3, 1, 0 },
	{ 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

// �o�b�t�@���Q�Ƃ���ԍ�
// Indices for vertex/index buffers
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// �V�F�[�_�v���O�������Q�Ƃ���ԍ�
// Index for a shader program
GLuint programId;

// �����̂̉�]�p�x
// Rotation angle for animating a cube
static float theta = 0.0f;

static bool isDragging = false;

// VAO�̏�����
// Initialize VAO
void initVAO() {
	// Vertex�z��̍쐬
	// Create vertex array
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			Vertex v(positions[faces[i * 2 + 0][j]], colors[i]);
			vertices.push_back(v);
			indices.push_back(idx++);
		}

		for (int j = 0; j < 3; j++) {
			Vertex v(positions[faces[i * 2 + 1][j]], colors[i]);
			vertices.push_back(v);
			indices.push_back(idx++);
		}
	}

	// VAO�̍쐬
	// Create VAO
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// ���_�o�b�t�@�I�u�W�F�N�g�̍쐬
	// Create vertex buffer object
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// ���_�o�b�t�@�ɑ΂��鑮�����̐ݒ�
	// Setup attributes for vertex buffer object
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	// ���_�ԍ��o�b�t�@�I�u�W�F�N�g�̍쐬
	// Create index buffer object
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// VAO��OFF�ɂ��Ă���
	// Temporarily disable VAO
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
	GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
	GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);

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

// ���[�U��`��OpenGL�̏�����
// User-define OpenGL initialization
void initializeGL() {
	// �[�x�e�X�g�̗L����
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// �w�i�F�̐ݒ� (��)
	// Background color (black)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// VAO�̏�����
	// Initialize VAO
	initVAO();

	// �V�F�[�_�̗p��
	// Prepare shader program
	initShaders();
}

// ���[�U��`��OpenGL�`��
// User-defined OpenGL drawing
void paintGL() {
	// �w�i�F�Ɛ[�x�l�̃N���A
	// Clear background color and depth values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���W�̕ϊ�
	// Coordinate transformation
	glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
		(float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // ���_�̈ʒu
		glm::vec3(0.0f, 0.0f, 0.0f),   // ���Ă����
		glm::vec3(0.0f, 1.0f, 0.0f));  // ���E�̏����

	glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mvpMat = projMat * viewMat * modelMat;

	// �V�F�[�_�̗L����
	// Enable shader program
	glUseProgram(programId);

	// Uniform�ϐ��̓]��
	// Transfer uniform variables
	GLuint mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat");
	glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

	// VAO�̗L����
	// Enable VAO
	glBindVertexArray(vaoId);

	// �O�p�`�̕`��
	// Draw triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// VAO�̖�����
	// Disable VAO
	glBindVertexArray(0);

	// �V�F�[�_�̖�����
	// Disable shader program
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

// �L�[�{�[�h�̉��������������R�[���o�b�N�֐�
// Callback for handling press/release keyboard keys
void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// �L�[�{�[�h�̏�ԂƉ����ꂽ�L�[��\������
	// Key pressed/released, which key is interacted
	printf("Keyboard: %s\n", action == GLFW_PRESS ? "Press" : "Release");
	if (key >= 0 && key < 127) {
		// ASCII������127�����̐���
		// ASCII chars correspond to integer less than 127
		printf("Key: %c (%d)\n", (char)key, key);
	}

	// ����L�[��������Ă��邩�̔���
	// Check special keys are pressed
	int specialKeys[] = { GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER };
	const char* specialKeyNames[] = { "Shift", "Ctrl", "Alt", "Super" };

	// ����L�[�̏�Ԃ�W���o��
	// Standard output for special key states
	printf("Special Keys: ");
	for (int i = 0; i < 4; i++) {
		if ((mods & specialKeys[i]) != 0) {
			printf("%s ", specialKeyNames[i]);
		}
	}
	printf("\n");
}

void printMouseButton(int button) {
	std::string strButton;
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		strButton = "Right Button";
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT) {
		strButton = "Left Button";
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		strButton = "Middle Button";
	}
	std::cout << "Mouse Button: " + strButton << std::endl;
}
// �}�E�X�̃N���b�N����������R�[���o�b�N�֐�
// Callback for mouse click events
void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
	// �}�E�X�������ꂽ���ǂ����̔���
	// Check whether mouse is pressed or released

	if (!isDragging && action == GLFW_PRESS) {
		isDragging = true;

		double px, py;
		glfwGetCursorPos(window, &px, &py);
		printf("Drag start at: (%d, %d)\n", (int)px, (int)py);
		char strButton;
		if (button == GLFW_MOUSE_BUTTON_LEFT) {

		}
		printMouseButton(button);
	}

	if (isDragging && action == GLFW_RELEASE) {
		isDragging = false;

		double px, py;
		glfwGetCursorPos(window, &px, &py);
		printf("Drag end at: (%d, %d)\n", (int)px, (int)py);
		printMouseButton(button);
	}

	//// ����L�[��������Ă��邩�̔���
	//// Check special keys are pressed
	//int specialKeys[] = { GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER };
	//const char* specialKeyNames[] = { "Shift", "Ctrl", "Alt", "Super" };

	//// ����L�[�̏�Ԃ�W���o��
	//// Standard output for special key states
	//printf("Special Keys: ");
	//for (int i = 0; i < 4; i++) {
	//    if ((mods & specialKeys[i]) != 0) {
	//        printf("%s ", specialKeyNames[i]);
	//    }
	//}
	//printf("\n");
}

// �}�E�X�̓�������������R�[���o�b�N�֐�
// Callback for mouse move events
void motionEvent(GLFWwindow* window, double px, double py) {
	if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT) {
		if (isDragging) {
			printf("Move: (%d, %d)\n", (int)px, (int)py);
		}
	}
}

// �A�j���[�V�����̂��߂̃A�b�v�f�[�g
// Update parameters for animation
void animate() {
	theta += 1.0f;  // 1�x������] / Rotate 1 degree of angle
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

	// OpenGL 3.x/4.x�̊֐������[�h���� (glfwMakeContextCurrent�̌�łȂ��Ƃ����Ȃ�)
	// Load OpenGL 3.x/4.x methods (must be loaded after "glfwMakeContextCurrent")
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

	// �L�[�{�[�h�̃C�x���g����������֐���o�^
	// Register a callback function for keyboard events
	glfwSetKeyCallback(window, keyboardEvent);

	// �}�E�X�̃C�x���g����������֐���o�^
	// Register a callback function for mouse click events
	glfwSetMouseButtonCallback(window, mouseEvent);

	// �}�E�X�̓�������������֐���o�^
	// Register a callback function for mouse move events
	glfwSetCursorPosCallback(window, motionEvent);

	// ���[�U�w��̏�����
	// User-specified initialization
	initializeGL();

	// ���C�����[�v
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		// �`�� / Draw
		paintGL();

		// �A�j���[�V���� / Animation
		animate();

		// �`��p�o�b�t�@�̐؂�ւ�
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// �㏈�� / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}