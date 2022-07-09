#ifndef _COMMON_H_
#include "common.h"
#endif

#include "Physics.h"


// バッファを参照する番号
// Indices for vertex/index buffers
GLuint vaoId_base, vaoIds_sphere, vaoId_background, vaoId_target, base_textureId, background_textureId, start_textureId, gameOver_textureId, clear_textureId, target_textureId;
int nIndices_base;
int nIndices_sphere;

GLuint programId;

Physics* _physics;


bool isDragging = false;

glm::ivec2 oldPos;
glm::ivec2 newPos;

// 操作の種類
// Type of control
enum ArcballMode {
	ARCBALL_MODE_NONE = 0x00,
	ARCBALL_MODE_TRANSLATE = 0x01,
	ARCBALL_MODE_ROTATE = 0x02,
	ARCBALL_MODE_SCALE = 0x04
};

int stage = 0;

int arcballMode = ARCBALL_MODE_NONE;
glm::mat4 viewMat, projMat;
glm::mat4 acRotMat, acTransMat, acScaleMat;
float elapsedTime = 0.0f;

void initVAOTarget() {
	GLuint indexBufferId, vertexBufferId;

	glm::vec3 positions[4] = {
		glm::vec3(1.0f, -0.20f, -1.0f) * 2.0f + glm::vec3(-8.0, 0.0, 0.0),
		glm::vec3(1.0f, -0.20f, 1.0f) * 2.0f + glm::vec3(-8.0, 0.0, 0.0),
		glm::vec3(-1.0f, -0.20f, -1.0f) * 2.0f + glm::vec3(-8.0, 0.0, 0.0),
		glm::vec3(-1.0f, -0.20f, 1.0f) * 2.0f + glm::vec3(-8.0, 0.0, 0.0)
	};
	glm::vec2 uvCoords[4] = {
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;

	for (int j = 0; j < 3; j++) {
		Vertex v(positions[j], glm::vec3(0), glm::vec3(0), uvCoords[j], -1.0);
		vertices.push_back(v);
		indices.push_back(idx++);
	}

	for (int j = 0; j < 3; j++) {
		Vertex v(positions[j + 1], glm::vec3(0), glm::vec3(0), uvCoords[j + 1], -1.0);
		vertices.push_back(v);
		indices.push_back(idx++);
	}


	glGenVertexArrays(1, &vaoId_target);
	glBindVertexArray(vaoId_target);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void initVAOBackground() {
	GLuint indexBufferId, vertexBufferId;

	glm::vec3 positions[4] = {
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f)
	};

	glm::vec2 uvCoords[4] = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f)
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;

	for (int j = 0; j < 3; j++) {
		Vertex v(positions[j], glm::vec3(0), glm::vec3(0), uvCoords[j], -1.0);
		vertices.push_back(v);
		indices.push_back(idx++);
	}

	for (int j = 0; j < 3; j++) {
		Vertex v(positions[j + 1], glm::vec3(0), glm::vec3(0), uvCoords[j + 1], -1.0);
		vertices.push_back(v);
		indices.push_back(idx++);
	}


	glGenVertexArrays(1, &vaoId_background);
	glBindVertexArray(vaoId_background);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void initVAOBase(GLuint& vaoID, int& nIndeces) {
	GLuint indexBufferId, vertexBufferId;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MESH_FILE.c_str());
	if (!err.empty()) {
		std::cerr << "[WARNING] " << err << std::endl;
	}

	if (!success) {
		std::cerr << "Failed to load OBJ file: " << MESH_FILE << std::endl;
		exit(1);
	}

	// Vertex配列の作成
	// Create vertex array
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::array<glm::vec2, 3> range;
	for (int s = 0; s < shapes.size(); s++) {
		const tinyobj::mesh_t& mesh = shapes[s].mesh;
		for (int i = 0; i < mesh.indices.size(); i++) {
			const tinyobj::index_t& index = mesh.indices[i];

			glm::vec3 position, normal;
			glm::vec2 texCoord;


			if (index.vertex_index >= 0) {
				float x = attrib.vertices[index.vertex_index * 3 + 0];
				float y = attrib.vertices[index.vertex_index * 3 + 1] * 0.5 - 0.5;
				float z = attrib.vertices[index.vertex_index * 3 + 2];
				position = glm::vec3(
					x,
					y,
					z
				);

				if (i == 0) {
					range[0] = glm::vec2(x, x);
					range[1] = glm::vec2(y, y);
					range[2] = glm::vec2(z, z);
				}
				else {
					if (x < range[0][0]) {
						range[0][0] = x;
					}
					if (x > range[0][1]) {
						range[0][1] = x;
					}
					if (y < range[1][0]) {
						range[1][0] = y;
					}
					if (y > range[1][1]) {
						range[1][1] = y;
					}
					if (z < range[2][0]) {
						range[2][0] = z;
					}
					if (z > range[2][1]) {
						range[2][1] = z;
					}
				}
			}

			if (index.normal_index >= 0) {
				normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0],
					attrib.normals[index.normal_index * 3 + 1],
					attrib.normals[index.normal_index * 3 + 2]);
			}

			if (index.texcoord_index >= 0) {
				texCoord = glm::vec2(
					attrib.texcoords[index.texcoord_index * 2 + 0],
					attrib.texcoords[index.texcoord_index * 2 + 1]
				) * COEFF_FOR_UV_COORDS;
			}

			const Vertex vertex(position, glm::vec3(0.5, 0, 0), normal, texCoord, 1.0);

			indices.push_back(vertices.size());
			vertices.push_back(vertex);
		}
	}

	nIndeces = indices.size();

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

	std::cout << "(minX, maxX)= (" + std::to_string(range[0][0]) + ", " + std::to_string(range[0][1]) + ", (minY, maxY)= (" + std::to_string(range[1][0]) + ", " + std::to_string(range[1][1]) + "), (minZ, maxZ)= (" + std::to_string(range[2][0]) + ", " + std::to_string(range[2][1]) + ")" << std::endl;
	_physics->setRange(range);
}

void initVAO() {
	_physics = new Physics();

	initVAOBase(vaoId_base, nIndices_base);
	initVAOBackground();
	initVAOTarget();

	glm::vec3 offset(8.0, 0.0, 0.0);
	_physics->initSphere(offset);
}

void initTextureImple(GLuint& texID, std::string filePath) {
	int texWidth, texHeight, channels;
	// Texture ============================================================================================
	unsigned char* bytesTexture = stbi_load(filePath.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
	if (!bytesTexture) {
		fprintf(stderr, "Failed to load image file: %s\n", filePath.c_str());
		exit(1);
	}

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, bytesTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(bytesTexture);
}
void initTexture() {
	initTextureImple(base_textureId, BASE_TEX_FILE);
	initTextureImple(background_textureId, BACKGOUND_TEX_FILE);
	initTextureImple(start_textureId, START_TEX_FILE);
	initTextureImple(gameOver_textureId, GAME_OVER_TEX_FILE);
	initTextureImple(target_textureId, TARGET_TEX_FILE);
	initTextureImple(clear_textureId, CLEAR_TEX_FILE);
}

void initShaders() {
	programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initVAO();
	initShaders();
	initTexture();

	projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	viewMat = glm::lookAt(
		glm::vec3(20.0f, 20.0f, 0.0f),   // 視点の位置 / Eye position
		glm::vec3(0.0f, 0.0f, 0.0f),     // 見ている先 / Looking position
		glm::vec3(0.0f, 1.0f, 0.0f)      // 視界の上方向 / Upward vector
	);

	acRotMat = glm::mat4(1.0);
	acTransMat = glm::mat4(1.0);
	acScaleMat = glm::mat4(1.0);

	stage = STAGE_START;
	elapsedTime = 0.0f;
}

void paintBackground(GLuint textureId) {
	GLuint uid;

	glDisable(GL_DEPTH_TEST);

	uid = glGetUniformLocation(programId, "u_mvpMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	uid = glGetUniformLocation(programId, "u_toUseTexture");
	glUniform1f(uid, 1.0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	uid = glGetUniformLocation(programId, "u_texture");
	glUniform1i(uid, 0);

	glBindVertexArray(vaoId_background);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}

void paintGL_start() {
	paintBackground(start_textureId);
}

void paintGL_main() {
	GLuint uid;
	glm::mat4 modelMat = acTransMat * acRotMat * acScaleMat;
	glm::mat4 vpMat = projMat * viewMat;
	glm::mat4 mvpMat = vpMat * modelMat;

	{
		paintBackground(background_textureId);
	}

	{
		_physics->paintGL(programId, modelMat, vpMat, acRotMat);
	}

	{
		uid = glGetUniformLocation(programId, "u_mvpMat");
		glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

		uid = glGetUniformLocation(programId, "u_toUseTexture");
		glUniform1f(uid, 1.0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, base_textureId);
		uid = glGetUniformLocation(programId, "u_texture");
		glUniform1i(uid, 0);

		glBindVertexArray(vaoId_base);
		glDrawElements(GL_TRIANGLES, nIndices_base, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	{
		uid = glGetUniformLocation(programId, "u_mvpMat");
		glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

		uid = glGetUniformLocation(programId, "u_toUseTexture");
		glUniform1f(uid, 1.0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, target_textureId);
		uid = glGetUniformLocation(programId, "u_texture");
		glUniform1i(uid, 0);

		glBindVertexArray(vaoId_target);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void paintGL_clear() {
	paintBackground(clear_textureId);
}

void paintGL_gameOver() {
	paintBackground(gameOver_textureId);
}


void paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId);
	if (stage == STAGE_START) {
		paintGL_start();
	}
	else if (stage == STAGE_PLAYING) {
		paintGL_main();
	}
	else if (stage == STAGE_CLEAR) {
		paintGL_clear();
	}
	else if (stage == STAGE_GAME_OVER) {
		paintGL_gameOver();
	}
	glUseProgram(0);

	int currentState = _physics->getState();
	if (currentState == Physics::STATE_GAME_OVER || currentState == Physics::STATE_CLEAR) {
		elapsedTime += 0.01f;
	}

	if (elapsedTime > 1.0f && currentState == Physics::STATE_GAME_OVER) {
		stage = STAGE_GAME_OVER;
	}
	else if (elapsedTime > 0.5f && currentState == Physics::STATE_CLEAR) {
		stage = STAGE_CLEAR;
	}
}

void resizeGL(GLFWwindow* window, int width, int height) {
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	glViewport(0, 0, renderBufferWidth, renderBufferHeight);

	projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
}

void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		arcballMode = ARCBALL_MODE_ROTATE;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		arcballMode = ARCBALL_MODE_SCALE;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		arcballMode = ARCBALL_MODE_TRANSLATE;
	}

	double px, py;
	glfwGetCursorPos(window, &px, &py);

	if (action == GLFW_PRESS) {
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

glm::vec3 getVector(double x, double y) {
	const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
	glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

	const double xySquared = pt.x * pt.x + pt.y * pt.y;
	if (xySquared <= 1.0) {
		pt.z = std::sqrt(1.0 - xySquared);
	}
	else {
		pt = glm::normalize(pt);
	}

	return pt;
}

void updateRotate() {
	const glm::vec3 u = getVector(oldPos.x, oldPos.y);
	const glm::vec3 v = getVector(newPos.x, newPos.y);

	const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));

	const glm::vec3 rotAxis = glm::cross(u, v);

	const glm::mat4 c2wMat = glm::inverse(viewMat);
	const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));

	acRotMat = glm::rotate((float)(4.0 * angle), rotAxisWorldSpace) * acRotMat;
}


void updateTransform() {
	switch (arcballMode) {
	case ARCBALL_MODE_ROTATE:
		updateRotate();
		break;
	}
}

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

}

void exit(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
	std::exit(0);
}

void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key == GLFW_KEY_R) {
		initializeGL();
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
		if (stage == STAGE_START) {
			stage = STAGE_PLAYING;
		}
		else if (stage == STAGE_CLEAR) {
			initializeGL();
		}
		else if (stage == STAGE_GAME_OVER) {
			initializeGL();
			stage = STAGE_PLAYING;
		}
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		exit(window);
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

	GLFWwindow* window = glfwCreateWindow(
		WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL
	);
	if (window == NULL) {
		glfwTerminate();
		fprintf(stderr, "Window creation failed!\n");
		return 1;
	}

	glfwMakeContextCurrent(window);
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glfwSetWindowSizeCallback(window, resizeGL);
	glfwSetMouseButtonCallback(window, mouseEvent);
	glfwSetCursorPosCallback(window, motionEvent);
	glfwSetScrollCallback(window, wheelEvent);
	glfwSetKeyCallback(window, keyboardEvent);

	initializeGL();

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		paintGL();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	exit(window);
}