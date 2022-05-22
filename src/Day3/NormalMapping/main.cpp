#define _CRT_SECURE_NO_WARNINGS
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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "common.h"

struct Vertex {
    Vertex(const glm::vec3& position_, const glm::vec3& normal_, const glm::vec2& uv_)
        : position(position_)
        , normal(normal_)
        , uv(uv_) {

    }

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;
GLuint texCoordBufferId;

GLuint programId;
GLuint textureId;
GLuint normalMapId;

static size_t indexBufferSize = 0;
static float theta = 0.0f;
static int algorithmID = 0;
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
static const glm::vec3 diffColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
static const glm::vec3 specColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
static const glm::vec3 ambiColor = glm::vec3(0.24725f, 0.1995f, 0.0745f);
static const float shininess = 1000.0f;

// VAOの初期化
void initVAO() {
    // メッシュファイルの読み込み
    // Load mesh file
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
    for (int s = 0; s < shapes.size(); s++) {
        const tinyobj::mesh_t& mesh = shapes[s].mesh;
        for (int i = 0; i < mesh.indices.size(); i++) {
            const tinyobj::index_t& index = mesh.indices[i];

            glm::vec3 position, normal;
            glm::vec2 texCoord;

            if (index.vertex_index >= 0) {
                position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0],
                    attrib.vertices[index.vertex_index * 3 + 1],
                    attrib.vertices[index.vertex_index * 3 + 2]);
            }

            if (index.normal_index >= 0) {
                normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0],
                    attrib.normals[index.normal_index * 3 + 1],
                    attrib.normals[index.normal_index * 3 + 2]);
            }

            if (index.texcoord_index >= 0) {
                texCoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2 + 0],
                    attrib.texcoords[index.texcoord_index * 2 + 1]) * COEFF_FOR_UV_COORDS;
            }

            const Vertex vertex(position, normal, texCoord);

            indices.push_back(vertices.size());
            vertices.push_back(vertex);
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    // 頂点番号バッファオブジェクトの作成
    // Create index buffer object
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
        indices.data(), GL_STATIC_DRAW);

    // 頂点バッファのサイズを変数に入れておく
    // Store size of index array buffer
    indexBufferSize = indices.size();

    // VAOをOFFにしておく
    // Temporarily disable VAO
    glBindVertexArray(0);
}

void initShaders() {
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

void initTexture() {
    int texWidth, texHeight, channels;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(bytesTexture);
    // ====================================================================================================
    
    // Normal Map =========================================================================================
    unsigned char* bytesNormalMap = stbi_load(NORMAL_MAP_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytesNormalMap) {
        fprintf(stderr, "Failed to load image file: %s\n", NORMAL_MAP_FILE.c_str());
        exit(1);
    }
    glGenTextures(1, &normalMapId);
    glBindTexture(GL_TEXTURE_2D, normalMapId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, bytesNormalMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(bytesNormalMap);
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

    glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
        glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
        glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

    glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;
    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = viewMat;
    
    // シェーダの有効化
    // Enable shader program
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
    uid = glGetUniformLocation(programId, "u_algorithmID");
    glUniform1i(uid, algorithmID);

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

    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    uid = glGetUniformLocation(programId, "u_texture");
    glUniform1i(uid, 0);

    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapId);
    uid = glGetUniformLocation(programId, "u_normalMap");
    glUniform1i(uid, 1);

    // VAOの有効化
    glBindVertexArray(vaoId);

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
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
    theta += 1.0f;
}

void setWindowTitle(GLFWwindow* window, const std::string strTitle) {
    const char* title = strTitle.c_str();
    char winTitle[256];
    sprintf(winTitle, "%s (Mode: %s)", WIN_TITLE, title);
    glfwSetWindowTitle(window, winTitle);
}

void keyboardActionPerformed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::string title;
    
    if (action == GLFW_PRESS && key == GLFW_KEY_0) {
        algorithmID = 0;

        title = "Standard";
        std::cout << title << std::endl;
        setWindowTitle(window, title);
    }
    else if (action == GLFW_PRESS && key == GLFW_KEY_1) {
        algorithmID = 1;

        title = "Normal Mapping + Blinn-Phong Shading";
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
    setWindowTitle(window, std::string("Standard"));

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