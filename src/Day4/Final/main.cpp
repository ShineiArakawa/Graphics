#ifndef _COMMON_H_
#include "common.h"
#endif
#include "Model.h"
#include "ParticleModel.h"

GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

GLuint programId;

// マウスドラッグ中かどうか
// Flag to check mouse is dragged or not
bool isDragging = false;

// マウスのクリック位置
// Mouse click position
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
        glm::vec3(20.0f, 20.0f, 20.0f),   // 視点の位置 / Eye position
        glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先 / Looking position
        glm::vec3(0.0f, 1.0f, 0.0f)    // 視界の上方向 / Upward vector
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

    // 東映変換行列の更新
    // Update projection matrix
    projMat = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
}

// マウスのクリックを処理するコールバック関数
// Callback for mouse click events
void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
    // クリックしたボタンで処理を切り替える
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

    // クリックされた位置を取得
    // Acquire a click position
    double px, py;
    glfwGetCursorPos(window, &px, &py);

    // マウスドラッグの状態を更新
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

// スクリーン上の位置をアークボール球上の位置に変換する関数
// Convert screen-space coordinates to a position on the arcball sphere
glm::vec3 getVector(double x, double y) {
    // 円がスクリーンの長辺に内接していると仮定
    // Assume a circle contacts internally with longer edges
    const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
    glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

    // z座標の計算
    // Calculate Z coordinate
    const double xySquared = pt.x * pt.x + pt.y * pt.y;
    if (xySquared <= 1.0) {
        // 単位円の内側ならz座標を計算
        // Calculate Z coordinate if a point is inside a unit circle
        pt.z = std::sqrt(1.0 - xySquared);
    }
    else {
        // 外側なら球の外枠上にあると考える
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

// 回転成分の更新
// Update rotation matrix
void updateRotate() {
    // マウスクリック位置をアークボール球上の座標に変換
    // Convert click positions to those on the arcball sphere
    const glm::vec3 u = getVector(oldPos.x, oldPos.y);
    const glm::vec3 v = getVector(newPos.x, newPos.y);

    // カメラ座標における回転量 (=世界座標における回転量)
    // Amount of rotation in camera space (= that in world space)
    const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));

    // カメラ空間における回転軸
    // Rotation axis in camera space
    const glm::vec3 rotAxis = glm::cross(u, v);

    // カメラ座標の情報を世界座標に変換する行列
    // Transformation matrix from camera space to world space
    const glm::mat4 c2wMat = glm::inverse(viewMat);

    // 世界座標における回転軸
    // Rotation axis in world space
    const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));

    // 回転行列の更新
    // Update rotation matrix
    acRotMat = glm::rotate((float)(4.0 * angle), rotAxisWorldSpace) * acRotMat;
}

// 平行移動量成分の更新
// Update translation matrix
void updateTranslate() {
    // NOTE:
    // この関数では物体が世界座標の原点付近にあるとして平行移動量を計算する
    // This function assumes the object locates near to the world-space origin and computes the amount of translation

    // 世界座標の原点のスクリーン座標を求める
    // Calculate screen-space coordinates of the world-space origin
    glm::vec4 originScreenSpace = (projMat * viewMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    originScreenSpace /= originScreenSpace.w;

    // スクリーン座標系におけるマウス移動の視点と終点の計算. これらの位置はスクリーン座標系のZ座標に依存することに注意する
    // Calculate the start and end points of mouse motion, which depend Z coordinate in screen space
    glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
    glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

    // スクリーン座標の情報を世界座標座標に変換する行列 (= MVP行列の逆行列)
    // Transformation from screen space to world space (= inverse of MVP matrix)
    glm::mat4 invMvpMat = glm::inverse(projMat * viewMat);

    // スクリーン空間の座標を世界座標に変換
    // Transform screen-space positions to world-space positions
    glm::vec4 newPosObjSpace = invMvpMat * newPosScreenSpace;
    glm::vec4 oldPosObjSpace = invMvpMat * oldPosScreenSpace;
    newPosObjSpace /= newPosObjSpace.w;
    oldPosObjSpace /= oldPosObjSpace.w;

    // 世界座標系で移動量を求める
    // Calculate the amount of translation in world space
    const glm::vec3 transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

    // 行列に変換
    // Calculate translation matrix
    acTransMat = glm::translate(transWorldSpace) * acTransMat;
}

// 物体の拡大縮小率を更新
// Update object scale
void updateScale() {
    acScaleMat = glm::scale(glm::vec3(acScale, acScale, acScale));
}

// 変換行列の更新. マウス操作の内容に応じて更新対象を切り替える
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

// マウスの動きを処理するコールバック関数
// Callback for mouse move events
void motionEvent(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        // マウスの現在位置を更新
        // Update current mouse position
        newPos = glm::ivec2(xpos, ypos);

        // マウスがあまり動いていない時は処理をしない
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

// マウスホイールを処理するコールバック関数
// Callback for mouse wheel event
void wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
    acScale += yoffset / 10.0;
    updateScale();
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