#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <string>

#ifndef _GLAD_LIB_
#define _GLAD_LIB_
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#endif

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS        // ���W�A���P�ʂ̊p�x���g�����Ƃ���������
#define GLM_ENABLE_EXPERIMENTAL  // glm/gtx/**.hpp���g���̂ɕK�v
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


static std::string        VERT_SHADER_FILE = std::filesystem::current_path().string() + "/render.vert";
static std::string        FRAG_SHADER_FILE = std::filesystem::current_path().string() + "/render.frag";
static const std::string DATA_DIRECTORY = std::filesystem::current_path().string() + "/../../../data/12_2/";
static const std::string PARTICLE_FILE_PATH = std::filesystem::current_path().string() + "/../../../data/particles.dat";

static int WIN_WIDTH = 1000;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 1000;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title
static const int NUM_DIVISIONS_THETA = 100;
static const int NUM_DIVISIONS_PHI = 100;
static const int NUM_SPHERES_1D = 1;
static const int DISTANCE_OF_SPHERES = 2.0;
static const float RADIUS = 0.5f;

struct Vertex {
	Vertex(
		const glm::vec3& position_,
		const glm::vec3& color_,
		const glm::vec3& normal_,
		const float id_)
		: position(position_)
		, color(color_)
		, normal(normal_)
		, id(id_) {
	}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	float id;
};

static const glm::vec3 positions_base[8] = {
	glm::vec3(-5.0f, -0.1f, -5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(5.0f, -0.1f, -5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(-5.0f,  0.1f, -5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(-5.0f, -0.1f,  5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(5.0f,  0.1f, -5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(-5.0f,  0.1f,  5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(5.0f, -0.1f,  5.0f) + glm::vec3(0.0, -0.6, 0.0),
	glm::vec3(5.0f,  0.1f,  5.0f) + glm::vec3(0.0, -0.6, 0.0)
};

static const glm::vec3 colors_base[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // ��
	glm::vec3(0.0f, 1.0f, 0.0f),  // ��
	glm::vec3(0.0f, 0.0f, 1.0f),  // ��
	glm::vec3(1.0f, 1.0f, 0.0f),  // �C�G���[
	glm::vec3(0.0f, 1.0f, 1.0f),  // �V�A��
	glm::vec3(1.0f, 0.0f, 1.0f),  // �}�[���^
};

static const unsigned int faces_base[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 },
	{ 2, 4, 7 }, { 2, 7, 5 },
	{ 5, 7, 6 }, { 5, 6, 3 },
	{ 4, 2, 0 }, { 4, 0, 1 },
	{ 3, 6, 1 }, { 3, 1, 0 },
	{ 2, 5, 3 }, { 2, 3, 0 }
};

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

#endif  // _COMMON_H_