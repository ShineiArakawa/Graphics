#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

//static const char* SOURCE_DIRECTORY = "C:/cygwin64/home/Insight_Arakawa/Projects/Graphics/src/Day3/NormalMapping/";
//static const char* SHADER_DIRECTORY = "C:/cygwin64/home/Insight_Arakawa/Projects/Graphics/src/Day3/NormalMapping/";
//static const char* DATA_DIRECTORY = "C:/cygwin64/home/Insight_Arakawa/Projects/Graphics/data/12_4/";

static const char* SOURCE_DIRECTORY = "D:/Github/Graphics/src/Day3/NormalMapping/";
static const char* SHADER_DIRECTORY = "D:/Github/Graphics/src/Day3/NormalMapping/";
static const char* DATA_DIRECTORY = "D:/Github/Graphics/data/12_4/";

static int                WIN_WIDTH = 2000;
static int                WIN_HEIGHT = 2000;
static const char* WIN_TITLE = "OpenGL Course";
static const int          FPS = 60;
static const float        COEFF_FOR_UV_COORDS = 5.0f;
static std::string        VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static std::string        FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";
static const std::string  MESH_FILE = std::string(DATA_DIRECTORY) + "teapot.obj";
static const std::string  TEX_FILE = std::string(DATA_DIRECTORY) + "sweater_diff.png";
static const std::string  NORMAL_MAP_FILE = std::string(DATA_DIRECTORY) + "sweater_normal.png";

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