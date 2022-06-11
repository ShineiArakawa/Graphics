#pragma once

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

class TransformVariables {
	glm::mat4 _viewMat, _projMat, _acRotMat, _acTransMat, _acScaleMat;
	float _acScale;
	int* _windowWidth;
	int* _windowHeight;

public:
	TransformVariables(int*, int*);
	~TransformVariables();
	void _init();
};
