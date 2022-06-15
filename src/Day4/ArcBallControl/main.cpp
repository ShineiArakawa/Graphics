#include "TransformVariables.h"

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

static std::vector<GLuint> vaoIds;
static std::vector<GLuint> vertexBufferIds;
static std::vector<GLuint> indexBufferIds;

static GLuint programId;

static glm::ivec2 oldPos;
static glm::ivec2 newPos;

enum ArcballMode {
	ARCBALL_MODE_NONE = 0x00,
	ARCBALL_MODE_TRANSLATE = 0x01,
	ARCBALL_MODE_ROTATE = 0x02,
	ARCBALL_MODE_SCALE = 0x04
};

static bool isDragging = false;
static int selectMode = false;
static int selectedCubeID = 0;
static int arcballMode = ARCBALL_MODE_NONE;
static Parameters* parameters;


void initVAO() {
	std::vector<glm::vec3> centerOfGravity;

	int iCube = 0;
	for (int indexX = 0; indexX < NUM_CUBES_1D; indexX++) {
		for (int indexY = 0; indexY < NUM_CUBES_1D; indexY++) {
			for (int indexZ = 0; indexZ < NUM_CUBES_1D; indexZ++) {
				GLuint vaoId = iCube;
				GLuint vertexBufferId = iCube;
				GLuint indexBufferId = iCube;

				float centerX = DISTANCE_OF_CUBES * (float)(indexX)-DISTANCE_OF_CUBES * (float)NUM_CUBES_1D / 2.0f;
				float centerY = DISTANCE_OF_CUBES * (float)(indexY)-DISTANCE_OF_CUBES * (float)NUM_CUBES_1D / 2.0f;
				float centerZ = DISTANCE_OF_CUBES * (float)(indexZ)-DISTANCE_OF_CUBES * (float)NUM_CUBES_1D / 2.0f;
				glm::vec3 offset(centerX, centerY, centerZ);
				centerOfGravity.push_back(offset);

				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;

				int idx = 0;
				for (int i = 0; i < 6; i++) {
					for (int j = 0; j < 3; j++) {
						Vertex v(positions[faces[i * 2 + 0][j]] + offset, colors[i], iCube);
						vertices.push_back(v);
						indices.push_back(idx++);
					}

					for (int j = 0; j < 3; j++) {
						Vertex v(positions[faces[i * 2 + 1][j]] + offset, colors[i], iCube);
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

				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, cubeID));

				glGenBuffers(1, &indexBufferId);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
					indices.data(), GL_STATIC_DRAW);

				glBindVertexArray(0);

				vaoIds.push_back(vaoId);
				vertexBufferIds.push_back(vertexBufferId);
				indexBufferIds.push_back(indexBufferId);

				iCube++;
			}
		}
	}

	parameters = new Parameters(centerOfGravity, &WIN_WIDTH, &WIN_HEIGHT);
}

void initShaders() {
	programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}


void initializeGL() {
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initVAO();
	initShaders();

	glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
	parameters->setProjMat(projMat);
	glm::mat4 viewMat = glm::lookAt(
		glm::vec3(0.0f, 30.0f, 30.0f),   // 視点の位置 / Eye position
		glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先 / Looking position
		glm::vec3(0.0f, 1.0f, 0.0f)    // 視界の上方向 / Upward vector
	);
	parameters->setViewMat(viewMat);
}

void paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId);
	for (int iCube = 0; iCube < NUM_CUBES; iCube++) {
		glBindVertexArray(vaoIds[iCube]);
		glm::mat4 mvpMat = parameters->getMvpMat(iCube);

		GLuint mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat");
		glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

		GLuint uid;
		uid = glGetUniformLocation(programId, "u_selectMode");
		glUniform1i(uid, selectMode);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void resizeGL(GLFWwindow* window, int width, int height) {
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	glViewport(0, 0, renderBufferWidth, renderBufferHeight);

	glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
	parameters->setProjMat(projMat);
}

void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		double px, py;
		glfwGetCursorPos(window, &px, &py);
		const int cx = (int)px;
		const int cy = (int)py;

		selectMode = 1;
		paintGL();
		selectMode = 0;

		int renderBufferWidth, renderBufferHeight;
		glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);
		int pixelSize = std::max(renderBufferWidth / WIN_WIDTH, renderBufferHeight / WIN_HEIGHT);

		unsigned char byte[4];
		glReadPixels(cx * pixelSize, (WIN_HEIGHT - cy - 1) * pixelSize, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, byte);
		printf("Mouse position: %d %d\n", cx, cy);
		printf("Select object %d\n", (int)byte[0]);
		selectedCubeID = (int)byte[0] - 1;

		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			arcballMode = ARCBALL_MODE_ROTATE;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			arcballMode = ARCBALL_MODE_SCALE;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			arcballMode = ARCBALL_MODE_TRANSLATE;
		}

		if (!isDragging) {
			isDragging = true;
			oldPos = glm::ivec2(px, py);
			newPos = glm::ivec2(px, py);
		}
	}
	else {
		isDragging = false;
		oldPos = glm::ivec2(0, 0);
		newPos = glm::ivec2(0, 0);
		arcballMode = ARCBALL_MODE_NONE;
	}
}

void updateTransform() {
	switch (arcballMode) {
	case ARCBALL_MODE_ROTATE:
		parameters->updateRotate(selectedCubeID, oldPos, newPos);
		break;

	case ARCBALL_MODE_TRANSLATE:
		parameters->updateTranslate(selectedCubeID, oldPos, newPos);
		break;

	case ARCBALL_MODE_SCALE:
		parameters->addAcScale(selectedCubeID, (float)(oldPos.y - newPos.y) / WIN_HEIGHT);
		parameters->updateScale(selectedCubeID);
		break;
	}
}

// マウスの動きを処理するコールバック関数
// Callback for mouse move events
void motionEvent(GLFWwindow* window, double xpos, double ypos) {
	if (isDragging) {
		newPos = glm::ivec2(xpos, ypos);

		const double dx = newPos.x - oldPos.x;
		const double dy = newPos.y - oldPos.y;
		const double length = dx * dx + dy * dy;
		if (length < 2.0 * 2.0) {
			return;
		}
		else {
			updateTransform();
			oldPos = glm::ivec2(xpos, ypos);
		}
	}
}

void wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
	parameters->addAcScale(selectedCubeID, yoffset / 10.0);
	parameters->updateScale(selectedCubeID);
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

	// マウスのイベントを処理する関数を登録
	// Register a callback function for mouse click events
	glfwSetMouseButtonCallback(window, mouseEvent);

	// マウスの動きを処理する関数を登録
	// Register a callback function for mouse move events
	glfwSetCursorPosCallback(window, motionEvent);

	// マウスホイールを処理する関数を登録
	// Register a callback function for mouse wheel
	glfwSetScrollCallback(window, wheelEvent);

	// ユーザ指定の初期化
	// User-specified initialization
	initializeGL();

	// メインループ
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		// 描画 / Draw
		paintGL();

		// 描画用バッファの切り替え
		// Swap drawing target buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 後処理 / Postprocess
	glfwDestroyWindow(window);
	glfwTerminate();
}