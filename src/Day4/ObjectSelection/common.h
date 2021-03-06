#pragma once
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS        // ラジアン単位の角度を使うことを強制する
#define GLM_ENABLE_EXPERIMENTAL  // glm/gtx/**.hppを使うのに必要
#include <glm/glm.hpp>
// glm::vec型をポインタに変換 / Convert glm::vec types to pointer
#include <glm/gtc/type_ptr.hpp>
// GLMの行列変換のためのユーティリティ関数 GLM's utility functions for matrix transformation
#include <glm/gtx/transform.hpp>

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static double FPS = 30.0;
static int WIN_WIDTH = 500;                      // ウィンドウの幅 / Window width
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title


static std::string        VERT_SHADER_FILE = std::filesystem::current_path().string() + "/render.vert";
static std::string        FRAG_SHADER_FILE = std::filesystem::current_path().string() + "/render.frag";
// static const std::string DATA_DIRECTORY = std::filesystem::current_path().string() + "/../../../data/12_2/";
static const std::string DATA_DIRECTORY = "/Users/shin/Github/Graphics/data/12_2/";
static const std::string  TEX_FILE = DATA_DIRECTORY + "dice.png";

struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& normal_, const glm::vec2& uv_, const int faceGroup_)
		: position(position_)
		, normal(normal_)
		, uv(uv_)
		, faceGroup(faceGroup_) {
	}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	float faceGroup;
};

// シェーダのソースファイルをコンパイルする
// Compile a shader source
GLuint compileShader(const std::string& filename, GLuint type) {
	// シェーダの作成
	// Create a shader
	GLuint shaderId = glCreateShader(type);

	// ファイルの読み込み
	// Load source file
	std::ifstream reader;
	std::string code;

	// ファイルを開く
	// Open source file
	reader.open(filename.c_str(), std::ios::in);
	if (!reader.is_open()) {
		// ファイルを開けなかったらエラーを出して終了
		// Finish with error message if source file could not be opened
		fprintf(stderr, "Failed to load a shader: %s\n", filename.c_str());
		exit(1);
	}

	// ファイルをすべて読んで変数に格納
	// Load entire contents of a file and store to a string variable
	{
		// ファイル読み取り位置を終端に移動 / Move seek position to the end
		reader.seekg(0, std::ios::end);
		// コードを格納する変数の大きさを予約 / Reserve memory location for code characters
		code.reserve(reader.tellg());
		// ファイルの読み取り位置を先頭に移動 / Move seek position back to the beginning
		reader.seekg(0, std::ios::beg);

		// 先頭からファイルサイズ分を読んでコードの変数に格納
		// Load entire file and copy to "code" variable
		code.assign(std::istreambuf_iterator<char>(reader),
			std::istreambuf_iterator<char>());
	}

	// ファイルを閉じる
	// Close file
	reader.close();

	// コードのコンパイル
	// Compile a source code
	const char* codeChars = code.c_str();
	glShaderSource(shaderId, 1, &codeChars, NULL);
	glCompileShader(shaderId);

	// コンパイルの成否を判定する
	// Check whther compile is successful
	GLint compileStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		// コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
		// Terminate with error message if compilation failed
		fprintf(stderr, "Failed to compile a shader!\n");

		// エラーメッセージの長さを取得する
		// Get length of error message
		GLint logLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			// エラーメッセージを取得する
			// Get error message
			GLsizei length;
			std::string errMsg;
			errMsg.resize(logLength);
			glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

			// エラーメッセージとソースコードの出力
			// Print error message and corresponding source code
			fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
			fprintf(stderr, "%s\n", code.c_str());
		}
		exit(1);
	}

	return shaderId;
}

// シェーダプログラムのビルド (=コンパイル＋リンク)
// Build a shader program (build = compile + link)
GLuint buildShaderProgram(const std::string& vShaderFile, const std::string& fShaderFile) {
	// 各種シェーダのコンパイル
	// Compile shader files
	GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
	GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);

	// シェーダプログラムへのリンク
	// Link shader objects to the program
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertShaderId);
	glAttachShader(programId, fragShaderId);
	glLinkProgram(programId);

	// リンクの成否を判定する
	// Check whether link is successful
	GLint linkState;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
	if (linkState == GL_FALSE) {
		// リンクに失敗したらエラーメッセージを表示して終了
		// Terminate with error message if link is failed
		fprintf(stderr, "Failed to link shaders!\n");

		// エラーメッセージの長さを取得する
		// Get length of error message
		GLint logLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			// エラーメッセージを取得する
			// Get error message
			GLsizei length;
			std::string errMsg;
			errMsg.resize(logLength);
			glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

			// エラーメッセージを出力する
			// Print error message
			fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
		}
		exit(1);
	}

	// シェーダを無効化した後にIDを返す
	// Disable shader program and return its ID
	glUseProgram(0);
	return programId;
}