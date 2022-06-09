#include "common.h"

GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;
GLuint texCoordBufferId;
GLuint programId;
GLuint textureId;

static size_t indexBufferSize = 0;
static float theta = 0.0f;
static bool isEnabledRotation = true;
static int texWidth, texHeight;
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);

static const float diffusionParam = 1.0f;
static const float specularParam = 8.0f;
static const float ambientParam = 0.1f;

// ============================================================================
// 立方体の定義 ===============================================================
// ============================================================================
static const glm::vec3 positions[8] = {
	glm::vec3(-1.0f, -1.0f, -1.0f), //0
	glm::vec3(1.0f, -1.0f, -1.0f),  //1
	glm::vec3(-1.0f,  1.0f, -1.0f), //2
	glm::vec3(-1.0f, -1.0f,  1.0f), //3
	glm::vec3(1.0f,  1.0f, -1.0f),  //4
	glm::vec3(-1.0f,  1.0f,  1.0f), //5
	glm::vec3(1.0f, -1.0f,  1.0f),  //6
	glm::vec3(1.0f,  1.0f,  1.0f)   //7
};

static const glm::vec3 normals[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),   //0
	glm::vec3(0.0f, 1.0f, 0.0f),   //1
	glm::vec3(0.0f, 0.0f, 1.0f),   //2
	glm::vec3(0.0f, 0.0f,  -1.0f), //3
	glm::vec3(0.0f, -1.0f, 0.0f),  //4
	glm::vec3(-1.0f,  0.0f,  0.0f) //5
};

static const glm::vec2 uvKeypointCoords[14] = {
	glm::vec2(0.0, 2.0 / 3.0),       //0
	glm::vec2(1.0 / 4.0, 2.0 / 3.0), //1
	glm::vec2(1.0 / 4.0, 1.0),       //2
	glm::vec2(2.0 / 4.0, 1.0),       //3
	glm::vec2(2.0 / 4.0, 2.0 / 3.0), //4
	glm::vec2(3.0 / 4.0, 2.0 / 3.0), //5
	glm::vec2(1.0, 2.0 / 3.0),       //6
	glm::vec2(1.0, 1.0 / 3.0),       //7
	glm::vec2(3.0 / 4.0, 1.0 / 3.0), //8
	glm::vec2(2.0 / 4.0, 1.0 / 3.0), //9
	glm::vec2(2.0 / 4.0, 0.0),       //10
	glm::vec2(1.0 / 4.0, 0.0),       //11
	glm::vec2(1.0 / 4.0, 1.0 / 3.0), //12
	glm::vec2(0.0, 1.0 / 3.0),       //13
};

static const int faces[12][3] = {
	{ 7, 4, 1 }, { 7, 1, 6 }, // =3, +**
	{ 2, 4, 7 }, { 2, 7, 5 }, // =5, *+*
	{ 5, 7, 6 }, { 5, 6, 3 }, // =1, **+
	{ 4, 2, 0 }, { 4, 0, 1 }, // =6, **-
	{ 3, 6, 1 }, { 3, 1, 0 }, // =2, *-*
	{ 2, 5, 3 }, { 2, 3, 0 }  // =4, -**
};

static const unsigned int faceToUVKeypointIndex[12][3] = {
	{ 4,  9, 12 }, { 4, 12,  1 }, // =3
	{ 8,  9,  4 }, { 8,  4,  5 }, // =5
	{ 3,  4,  1 }, { 3,  1,  2 }, // =1
	{ 9, 10, 11 }, { 9, 11, 12 }, // =6
	{ 0,  1, 12 }, { 0, 12, 13 }, // =2
	{ 8,  5,  6 }, { 8,  6,  7 }  // =4
};
// ============================================================================


void initVAO() {
	// Vertex配列の作成
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			Vertex v(
				positions[faces[i * 2 + 0][j]],
				normals[i],
				uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 0][j]]);
			vertices.push_back(v);
			indices.push_back(idx++);
		}

		for (int j = 0; j < 3; j++) {
			Vertex v(
				positions[faces[i * 2 + 1][j]],
				normals[i],
				uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 1][j]]);
			vertices.push_back(v);
			indices.push_back(idx++);
		}
	}

	// VAOの作成
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	indexBufferSize = indices.size();

	glBindVertexArray(0);
}


void initShaders() {
	programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

void initTexture() {
	int channels;
	// Texture ============================================================================================
	unsigned char* bytesTexture = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
	if (!bytesTexture) {
		fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
		exit(1);
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, bytesTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(bytesTexture);
	std::cout << "texWidth= " + std::to_string(texWidth) + ", texHeight= " + std::to_string(texHeight) << std::endl;
	// ====================================================================================================
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	initVAO();
	initShaders();
	initTexture();
}

void paintGL() {
	// 背景色と深度値のクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 座標の変換
	glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
		(float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);

	glm::mat4 viewMat = glm::lookAt(
		glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
		glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
		glm::vec3(0.0f, 1.0f, 0.0f)    // 視界の上方向
	);

	glm::mat4 modelMat1 = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 modelMat2 = glm::rotate(glm::radians(theta) * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 modelMat = modelMat1 * modelMat2;
	glm::mat4 mvMat = viewMat * modelMat;
	glm::mat4 mvpMat = projMat * viewMat * modelMat;
	glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
	glm::mat4 lightMat = viewMat;

	// シェーダの有効化
	glUseProgram(programId);

	// Uniform変数の転送
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
	uid = glGetUniformLocation(programId, "u_diffusionParam");
	glUniform1f(uid, diffusionParam);
	uid = glGetUniformLocation(programId, "u_specularParam");
	glUniform1f(uid, specularParam);
	uid = glGetUniformLocation(programId, "u_ambientParam");
	glUniform1f(uid, ambientParam);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	uid = glGetUniformLocation(programId, "u_texture");
	glUniform1i(uid, 0);

	glBindVertexArray(vaoId);
	glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void resizeGL(GLFWwindow* window, int width, int height) {
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

void animate() {
	if (isEnabledRotation) {
		theta += 1.0f;
	}
}

void setWindowTitle(GLFWwindow* window, const std::string strTitle) {
	const char* title = strTitle.c_str();
	char winTitle[256];
	sprintf(winTitle, "%s (Mode: %s)", WIN_TITLE, title);
	glfwSetWindowTitle(window, winTitle);
}

/*
* キーボード入力を受け取るための関数である。
* キーボードで「1」を入力すると物体が回転し、「2」を入力すると回転が停止する。
*/
void keyboardActionPerformed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::string title;

	if (action == GLFW_PRESS && key == GLFW_KEY_1) {
		isEnabledRotation = true;

		title = "Enabled Rotation";
		std::cout << title << std::endl;
		setWindowTitle(window, title);
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_2) {
		isEnabledRotation = false;

		title = "Disabled Rotation";
		std::cout << title << std::endl;
		setWindowTitle(window, title);
	}
}

int main(int argc, char** argv) {
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}

	glfwMakeContextCurrent(window);
	setWindowTitle(window, std::string("Enabled Rotation"));

	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));


	glfwSetWindowSizeCallback(window, resizeGL);
	glfwSetKeyCallback(window, keyboardActionPerformed);
	initializeGL();

	float prevTime, currentTime;
	prevTime = glfwGetTime();
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		currentTime = glfwGetTime();

		if ((currentTime - prevTime) >= 1.0f / (float)FPS) {
			paintGL();
			animate();

			glfwSwapBuffers(window);
			glfwPollEvents();

			prevTime = currentTime;
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}