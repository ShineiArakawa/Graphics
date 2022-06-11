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

#define GLM_FORCE_RADIANS        // ラジアン単位の角度を使うことを強制する
#define GLM_ENABLE_EXPERIMENTAL  // glm/gtx/**.hppを使うのに必要
#include <glm/glm.hpp>
// glm::vec型をポインタに変換 / Convert glm::vec types to pointer
#include <glm/gtc/type_ptr.hpp>
// GLMの行列変換のためのユーティリティ関数 GLM's utility functions for matrix transformation
#include <glm/gtx/transform.hpp>

// 画像のパスなどが書かれた設定ファイル
// Config file storing image locations etc.
#include "common.h"

static int WIN_WIDTH = 500;                      // ウィンドウの幅 / Window width
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ / Window height
static const char* WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title


// 頂点クラス
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
	glm::vec3(1.0f, 0.0f, 0.0f),  // 赤
	glm::vec3(0.0f, 1.0f, 0.0f),  // 緑
	glm::vec3(0.0f, 0.0f, 1.0f),  // 青
	glm::vec3(1.0f, 1.0f, 0.0f),  // イエロー
	glm::vec3(0.0f, 1.0f, 1.0f),  // シアン
	glm::vec3(1.0f, 0.0f, 1.0f),  // マゼンタ
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

// バッファを参照する番号
// Indices for vertex/index buffers
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// シェーダプログラムを参照する番号
// Index for a shader program
GLuint programId;

// 立方体の回転角度
// Rotation angle for animating a cube
static float theta = 0.0f;

static bool isDragging = false;

// VAOの初期化
// Initialize VAO
void initVAO() {
	// Vertex配列の作成
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

	// VAOの作成
	// Create VAO
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// 頂点バッファオブジェクトの作成
	// Create vertex buffer object
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// 頂点バッファに対する属性情報の設定
	// Setup attributes for vertex buffer object
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	// 頂点番号バッファオブジェクトの作成
	// Create index buffer object
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// VAOをOFFにしておく
	// Temporarily disable VAO
	glBindVertexArray(0);
}

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

// シェーダの初期化
// Initialization related to shader programs
void initShaders() {
	programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

// ユーザ定義のOpenGLの初期化
// User-define OpenGL initialization
void initializeGL() {
	// 深度テストの有効化
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// 背景色の設定 (黒)
	// Background color (black)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// VAOの初期化
	// Initialize VAO
	initVAO();

	// シェーダの用意
	// Prepare shader program
	initShaders();
}

// ユーザ定義のOpenGL描画
// User-defined OpenGL drawing
void paintGL() {
	// 背景色と深度値のクリア
	// Clear background color and depth values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 座標の変換
	// Coordinate transformation
	glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
		(float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
		glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
		glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

	glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mvpMat = projMat * viewMat * modelMat;

	// シェーダの有効化
	// Enable shader program
	glUseProgram(programId);

	// Uniform変数の転送
	// Transfer uniform variables
	GLuint mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat");
	glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

	// VAOの有効化
	// Enable VAO
	glBindVertexArray(vaoId);

	// 三角形の描画
	// Draw triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// VAOの無効化
	// Disable VAO
	glBindVertexArray(0);

	// シェーダの無効化
	// Disable shader program
	glUseProgram(0);
}

// ウィンドウサイズ変更のコールバック関数
// Callback function for window resizing
void resizeGL(GLFWwindow* window, int width, int height) {
	// ユーザ管理のウィンドウサイズを変更
	// Update user-managed window size
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	// GLFW管理のウィンドウサイズを変更
	// Update GLFW-managed window size
	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	// 実際のウィンドウサイズ (ピクセル数) を取得
	// Get actual window size by pixels
	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	// ビューポート変換の更新
	// Update viewport transform
	glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

// キーボードの押し離しを扱うコールバック関数
// Callback for handling press/release keyboard keys
void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// キーボードの状態と押されたキーを表示する
	// Key pressed/released, which key is interacted
	printf("Keyboard: %s\n", action == GLFW_PRESS ? "Press" : "Release");
	if (key >= 0 && key < 127) {
		// ASCII文字は127未満の整数
		// ASCII chars correspond to integer less than 127
		printf("Key: %c (%d)\n", (char)key, key);
	}

	// 特殊キーが押されているかの判定
	// Check special keys are pressed
	int specialKeys[] = { GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER };
	const char* specialKeyNames[] = { "Shift", "Ctrl", "Alt", "Super" };

	// 特殊キーの状態を標準出力
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
// マウスのクリックを処理するコールバック関数
// Callback for mouse click events
void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
	// マウスが押されたかどうかの判定
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

	//// 特殊キーが押されているかの判定
	//// Check special keys are pressed
	//int specialKeys[] = { GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER };
	//const char* specialKeyNames[] = { "Shift", "Ctrl", "Alt", "Super" };

	//// 特殊キーの状態を標準出力
	//// Standard output for special key states
	//printf("Special Keys: ");
	//for (int i = 0; i < 4; i++) {
	//    if ((mods & specialKeys[i]) != 0) {
	//        printf("%s ", specialKeyNames[i]);
	//    }
	//}
	//printf("\n");
}

// マウスの動きを処理するコールバック関数
// Callback for mouse move events
void motionEvent(GLFWwindow* window, double px, double py) {
	if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT) {
		if (isDragging) {
			printf("Move: (%d, %d)\n", (int)px, (int)py);
		}
	}
}

// アニメーションのためのアップデート
// Update parameters for animation
void animate() {
	theta += 1.0f;  // 1度だけ回転 / Rotate 1 degree of angle
}

int main(int argc, char** argv) {
	// OpenGLを初期化する
	// OpenGL initialization
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// OpenGLのバージョン設定 (Macの場合には必ず必要)
	// Specify OpenGL version (mandatory for Mac)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Windowの作成
	// Create a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}

	// OpenGLの描画対象にwindowを指定
	// Specify window as an OpenGL context
	glfwMakeContextCurrent(window);

	// OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
	// Load OpenGL 3.x/4.x methods (must be loaded after "glfwMakeContextCurrent")
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}

	// バージョンを出力する / Check OpenGL version
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// ウィンドウのリサイズを扱う関数の登録
	// Register a callback function for window resizing
	glfwSetWindowSizeCallback(window, resizeGL);

	// キーボードのイベントを処理する関数を登録
	// Register a callback function for keyboard events
	glfwSetKeyCallback(window, keyboardEvent);

	// マウスのイベントを処理する関数を登録
	// Register a callback function for mouse click events
	glfwSetMouseButtonCallback(window, mouseEvent);

	// マウスの動きを処理する関数を登録
	// Register a callback function for mouse move events
	glfwSetCursorPosCallback(window, motionEvent);

	// ユーザ指定の初期化
	// User-specified initialization
	initializeGL();

	// メインループ
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		// 描画 / Draw
		paintGL();

		// アニメーション / Animation
		animate();

		// 描画用バッファの切り替え
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 後処理 / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}