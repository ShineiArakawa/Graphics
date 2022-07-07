#ifndef _COMMON_H_
#include "common.h"
#endif
#include "Model.h"
#include "ParticleModel.h"

GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

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

int arcballMode = ARCBALL_MODE_NONE;
glm::mat4 viewMat, projMat;
glm::mat4 acRotMat, acTransMat, acScaleMat;
float acScale = 1.0f;
Models* _models;

void initVAO() {
    _models = new Models();
    ParticleModel* model = new ParticleModel(&programId, PARTICLE_FILE_PATH);
    
    _models->setModel(model);

    _models->initVAO();
}


void initShaders() {
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

void initializeGL() {
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    initVAO();

    initShaders();

    projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    viewMat = glm::lookAt(
        glm::vec3(20.0f, 20.0f, 20.0f),   // ���_�̈ʒu / Eye position
        glm::vec3(0.0f, 0.0f, 0.0f),   // ���Ă���� / Looking position
        glm::vec3(0.0f, 1.0f, 0.0f)    // ���E�̏���� / Upward vector
    );

    acRotMat = glm::mat4(1.0);
    acTransMat = glm::mat4(1.0);
    acScaleMat = glm::mat4(1.0);
}

void paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 vpMat = projMat * viewMat;

    _models->setMatrices(0, vpMat, acTransMat, acRotMat, acScaleMat);
    _models->paintGL(0);
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
    const glm::mat4 c2wMat = glm::inverse(viewMat);

    // ���E���W�ɂ������]��
    // Rotation axis in world space
    const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));

    // ��]�s��̍X�V
    // Update rotation matrix
    acRotMat = glm::rotate((float)(4.0 * angle), rotAxisWorldSpace) * acRotMat;
}

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