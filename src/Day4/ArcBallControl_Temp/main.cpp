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

// �}�E�X�h���b�O�����ǂ���
// Flag to check mouse is dragged or not
bool isDragging = false;

// �}�E�X�̃N���b�N�ʒu
// Mouse click position
glm::ivec2 oldPos;
glm::ivec2 newPos;

// ����̎��
// Type of control
enum ArcballMode {
    ARCBALL_MODE_NONE = 0x00,
    ARCBALL_MODE_TRANSLATE = 0x01,
    ARCBALL_MODE_ROTATE = 0x02,
    ARCBALL_MODE_SCALE = 0x04
};

// ���W�ϊ��̂��߂̕ϐ�
// Variables for coordinate transformation
int arcballMode = ARCBALL_MODE_NONE;
glm::mat4 viewMat, projMat;
glm::mat4 acRotMat, acTransMat, acScaleMat;
float acScale = 1.0f;

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
            Vertex v(positions[faces[i * 2 + 0][j]] + glm::vec3(1.0f, 0.0f, 0.0f), colors[i]);
            vertices.push_back(v);
            indices.push_back(idx++);
        }

        for (int j = 0; j < 3; j++) {
            Vertex v(positions[faces[i * 2 + 1][j]] + glm::vec3(1.0f, 0.0f, 0.0f), colors[i]);
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

    // �J�����̎p�������肷��ϊ��s��̏�����
    // Initialize transformation matrices for camera pose
    projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    viewMat = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f),   // ���_�̈ʒu / Eye position
        glm::vec3(0.0f, 0.0f, 0.0f),   // ���Ă���� / Looking position
        glm::vec3(0.0f, 1.0f, 0.0f));  // ���E�̏���� / Upward vector

// �A�[�N�{�[������̂��߂̕ϊ��s���������
// Initialize transformation matrices for arcball control
    acRotMat = glm::mat4(1.0);
    acTransMat = glm::mat4(1.0);
    acScaleMat = glm::mat4(1.0);
}

// ���[�U��`��OpenGL�`��
// User-defined OpenGL drawing
void paintGL() {
    // �w�i�F�Ɛ[�x�l�̃N���A
    // Clear background color and depth values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ���W�̕ϊ�. �A�[�N�{�[�����삩�烂�f���s������肷��
    // Coordinate transformation. Model matrix is determined by arcball control
    glm::mat4 modelMat = acTransMat * acRotMat * acScaleMat;
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

    // ���f�ϊ��s��̍X�V
    // Update projection matrix
    projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
}

// �}�E�X�̃N���b�N����������R�[���o�b�N�֐�
// Callback for mouse click events
void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
    // �N���b�N�����{�^���ŏ�����؂�ւ���
    // Switch following operation depending on a clicked button
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        arcballMode = ARCBALL_MODE_ROTATE;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        arcballMode = ARCBALL_MODE_SCALE;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        arcballMode = ARCBALL_MODE_TRANSLATE;
    }

    // �N���b�N���ꂽ�ʒu���擾
    // Acquire a click position
    double px, py;
    glfwGetCursorPos(window, &px, &py);

    // �}�E�X�h���b�O�̏�Ԃ��X�V
    // Update state of mouse dragging
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

// �X�N���[����̈ʒu���A�[�N�{�[������̈ʒu�ɕϊ�����֐�
// Convert screen-space coordinates to a position on the arcball sphere
glm::vec3 getVector(double x, double y) {
    // �~���X�N���[���̒��ӂɓ��ڂ��Ă���Ɖ���
    // Assume a circle contacts internally with longer edges
    const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
    glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

    // z���W�̌v�Z
    // Calculate Z coordinate
    const double xySquared = pt.x * pt.x + pt.y * pt.y;
    if (xySquared <= 1.0) {
        // �P�ʉ~�̓����Ȃ�z���W���v�Z
        // Calculate Z coordinate if a point is inside a unit circle
        pt.z = std::sqrt(1.0 - xySquared);
    }
    else {
        // �O���Ȃ狅�̊O�g��ɂ���ƍl����
        // Suppose a point is on the circle line if the click position is outside the unit circle
        pt = glm::normalize(pt);
    }

    return pt;
};

void printMat(glm::mat4 modelMat) {
    printf("============================================================\n");
    for (int i = 0; i < 4; i++) {
        printf("%f, %f, %f, %f\n", (float)modelMat[i][0], (float)modelMat[i][1], (float)modelMat[i][2], (float)modelMat[i][3]);
    }
}

// ��]�����̍X�V
// Update rotation matrix
void updateRotate() {
    // �}�E�X�N���b�N�ʒu���A�[�N�{�[������̍��W�ɕϊ�
    // Convert click positions to those on the arcball sphere
    const glm::vec3 u = getVector(oldPos.x, oldPos.y);
    const glm::vec3 v = getVector(newPos.x, newPos.y);

    // �J�������W�ɂ������]�� (=���E���W�ɂ������]��)
    // Amount of rotation in camera space (= that in world space)
    const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));

    // �J������Ԃɂ������]��
    // Rotation axis in camera space
    const glm::vec3 rotAxis = glm::cross(u, v);

    // �J�������W�̏��𐢊E���W�ɕϊ�����s��
    // Transformation matrix from camera space to world space
    glm::mat4 modelMat = acTransMat * acRotMat * acScaleMat;
    const glm::mat4 c2wMat = glm::inverse(modelMat) + glm::inverse(viewMat);
    // ���f�����O���W�ɂ������]��
    // Rotation axis in world space
    const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));

    // ��]�s��̍X�V
    // Update rotation matrix
    acRotMat = glm::translate(glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate((float)(4.0 * angle), rotAxisWorldSpace) * glm::translate(glm::vec3(-1.0f, 0.0f, 0.0f)) * acRotMat;
    printMat(modelMat);
}

//// ��]�����̍X�V
//// Update rotation matrix
//void updateRotate() {
//    // �}�E�X�N���b�N�ʒu���A�[�N�{�[������̍��W�ɕϊ�
//    // Convert click positions to those on the arcball sphere
//    const glm::vec3 u = getVector(oldPos.x, oldPos.y);
//    const glm::vec3 v = getVector(newPos.x, newPos.y);
//
//    // �J�������W�ɂ������]�� (=���E���W�ɂ������]��)
//    // Amount of rotation in camera space (= that in world space)
//    const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));
//
//    // �J������Ԃɂ������]��
//    // Rotation axis in camera space
//    const glm::vec3 rotAxis = glm::cross(u, v);
//
//    // �J�������W�̏��𐢊E���W�ɕϊ�����s��
//    // Transformation matrix from camera space to world space
//    glm::mat4 modelMat = acTransMat * acRotMat * acScaleMat;
//    const glm::mat4 c2wMat = glm::inverse(modelMat) * glm::inverse(viewMat);
//
//    // ���E���W�ɂ������]��
//    // Rotation axis in world space
//    const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));
//
//    // ��]�s��̍X�V
//    // Update rotation matrix
//    acRotMat = glm::rotate((float)(4.0 * angle), rotAxisWorldSpace) * acRotMat;
//}

// ���s�ړ��ʐ����̍X�V
// Update translation matrix
void updateTranslate() {
    // NOTE:
    // ���̊֐��ł͕��̂����E���W�̌��_�t�߂ɂ���Ƃ��ĕ��s�ړ��ʂ��v�Z����
    // This function assumes the object locates near to the world-space origin and computes the amount of translation

    // ���E���W�̌��_�̃X�N���[�����W�����߂�
    // Calculate screen-space coordinates of the world-space origin
    glm::vec4 originScreenSpace = (projMat * viewMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    originScreenSpace /= originScreenSpace.w;

    // �X�N���[�����W�n�ɂ�����}�E�X�ړ��̎��_�ƏI�_�̌v�Z. �����̈ʒu�̓X�N���[�����W�n��Z���W�Ɉˑ����邱�Ƃɒ��ӂ���
    // Calculate the start and end points of mouse motion, which depend Z coordinate in screen space
    glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
    glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

    // �X�N���[�����W�̏��𐢊E���W���W�ɕϊ�����s�� (= MVP�s��̋t�s��)
    // Transformation from screen space to world space (= inverse of MVP matrix)
    glm::mat4 invMvpMat = glm::inverse(projMat * viewMat);

    // �X�N���[����Ԃ̍��W�𐢊E���W�ɕϊ�
    // Transform screen-space positions to world-space positions
    glm::vec4 newPosObjSpace = invMvpMat * newPosScreenSpace;
    glm::vec4 oldPosObjSpace = invMvpMat * oldPosScreenSpace;
    newPosObjSpace /= newPosObjSpace.w;
    oldPosObjSpace /= oldPosObjSpace.w;

    // ���E���W�n�ňړ��ʂ����߂�
    // Calculate the amount of translation in world space
    const glm::vec3 transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

    // �s��ɕϊ�
    // Calculate translation matrix
    acTransMat = glm::translate(transWorldSpace) * acTransMat;
}

// ���̂̊g��k�������X�V
// Update object scale
void updateScale() {
    acScaleMat = glm::scale(glm::vec3(acScale, acScale, acScale));
}

// �ϊ��s��̍X�V. �}�E�X����̓��e�ɉ����čX�V�Ώۂ�؂�ւ���
// Update transformation matrices, depending on type of mouse interaction
void updateTransform() {
    switch (arcballMode) {
    case ARCBALL_MODE_ROTATE:
        updateRotate();
        break;

    case ARCBALL_MODE_TRANSLATE:
        updateTranslate();
        break;

    case ARCBALL_MODE_SCALE:
        acScale += (float)(oldPos.y - newPos.y) / WIN_HEIGHT;
        updateScale();
        break;
    }
}

// �}�E�X�̓�������������R�[���o�b�N�֐�
// Callback for mouse move events
void motionEvent(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        // �}�E�X�̌��݈ʒu���X�V
        // Update current mouse position
        newPos = glm::ivec2(xpos, ypos);

        // �}�E�X�����܂蓮���Ă��Ȃ����͏��������Ȃ�
        // Update transform only when mouse moves sufficiently
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

// �}�E�X�z�C�[������������R�[���o�b�N�֐�
// Callback for mouse wheel event
void wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
    acScale += yoffset / 10.0;
    updateScale();
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

    // �}�E�X�̃C�x���g����������֐���o�^
    // Register a callback function for mouse click events
    glfwSetMouseButtonCallback(window, mouseEvent);

    // �}�E�X�̓�������������֐���o�^
    // Register a callback function for mouse move events
    glfwSetCursorPosCallback(window, motionEvent);

    // �}�E�X�z�C�[������������֐���o�^
    // Register a callback function for mouse wheel
    glfwSetScrollCallback(window, wheelEvent);

    // ���[�U�w��̏�����
    // User-specified initialization
    initializeGL();

    // ���C�����[�v
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // �`�� / Draw
        paintGL();

        // �`��p�o�b�t�@�̐؂�ւ�
        // Swap drawing target buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �㏈�� / Postprocess
    glfwDestroyWindow(window);
    glfwTerminate();
}