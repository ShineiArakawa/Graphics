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

// OBJ���b�V���ǂݍ��ݗp�̃��C�u����
// Library for loading OBJ file
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// �摜�̃p�X�Ȃǂ������ꂽ�ݒ�t�@�C��
// Config file storing image locations etc.
#include "common.h"

static int WIN_WIDTH = 500;                      // �E�B���h�E�̕� / Window width
static int WIN_HEIGHT = 500;                     // �E�B���h�E�̍��� / Window height
static const char* WIN_TITLE = "OpenGL Course";  // �E�B���h�E�̃^�C�g�� / Window title

// �V�F�[�_����̃\�[�X�t�@�C�� / Shader source files
static std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";

// ���b�V�����f���̃t�@�C��
// Mesh model file
//static const std::string MESH_FILE = std::string(DATA_DIRECTORY) + "bunny.obj";
static const std::string MESH_FILE = std::string(DATA_DIRECTORY) + "12_4/teapot.obj";

// ���_�ԍ��z��̑傫��
// Length of index array buffer
static size_t indexBufferSize = 0;

// ���_�N���X
// Vertex class
struct Vertex {
    Vertex(const glm::vec3& position_, const glm::vec3& normal_)
        : position(position_)
        , normal(normal_) {
    }

    glm::vec3 position;
    glm::vec3 normal;
};

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

// �V�F�[�f�B���O�̂��߂̃}�e���A�����
// Material parameters for shading
// Gold (http://www.barradeau.com/nicoptere/dump/materials.html)
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
static const glm::vec3 diffColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
static const glm::vec3 specColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
static const glm::vec3 ambiColor = glm::vec3(0.24725f, 0.1995f, 0.0745f);
static const float shininess = 51.2f;

// VAO�̏�����
void initVAO() {
    // ���b�V���t�@�C���̓ǂݍ���
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

    // Vertex�z��̍쐬
    // Create vertex array
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (int s = 0; s < shapes.size(); s++) {
        const tinyobj::mesh_t& mesh = shapes[s].mesh;
        for (int i = 0; i < mesh.indices.size(); i++) {
            const tinyobj::index_t& index = mesh.indices[i];

            glm::vec3 position, normal;

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

            const Vertex vertex(position, normal);

            indices.push_back(vertices.size());
            vertices.push_back(vertex);
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // ���_�ԍ��o�b�t�@�I�u�W�F�N�g�̍쐬
    // Create index buffer object
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
        indices.data(), GL_STATIC_DRAW);

    // ���_�o�b�t�@�̃T�C�Y��ϐ��ɓ���Ă���
    // Store size of index array buffer
    indexBufferSize = indices.size();

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

    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;
    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = viewMat;

    // �V�F�[�_�̗L����
    // Enable shader program
    glUseProgram(programId);

    // Uniform�ϐ��̓]��
    // Transfer uniform variables
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

    // VAO�̗L����
    // Enable VAO
    glBindVertexArray(vaoId);

    // �O�p�`�̕`��
    // Draw triangles
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);

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