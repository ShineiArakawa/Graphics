#pragma once

#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS        // ラジアン単位の角度を使うことを強制する
#define GLM_ENABLE_EXPERIMENTAL  // glm/gtx/**.hppを使うのに必要
#include <glm/glm.hpp>
// glm::vec型をポインタに変換 / Convert glm::vec types to pointer
#include <glm/gtc/type_ptr.hpp>
// GLMの行列変換のためのユーティリティ関数 GLM's utility functions for matrix transformation
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
