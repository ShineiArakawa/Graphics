#include "TransformVariables.h"

TransformVariables::TransformVariables(int* windowWidth, int* windowHeight) {
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;

	_init();
}

TransformVariables::~TransformVariables() {

}

void TransformVariables::_init() {
	_projMat = glm::perspective(glm::radians(45.0f), (float)*_windowWidth / (float)*_windowHeight, 0.1f, 1000.0f);

	_viewMat = glm::lookAt(
		glm::vec3(3.0f, 4.0f, 5.0f),   // ���_�̈ʒu / Eye position
		glm::vec3(0.0f, 0.0f, 0.0f),   // ���Ă���� / Looking position
		glm::vec3(0.0f, 1.0f, 0.0f)    // ���E�̏���� / Upward vector
	);

	_acScale = 1.0f;
	_acRotMat = glm::mat4(1.0);
	_acTransMat = glm::mat4(1.0);
	_acScaleMat = glm::mat4(1.0);
}