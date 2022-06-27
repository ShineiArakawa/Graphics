/*
* This is header only library.
*/
#pragma once
#ifndef _COMMON_H_
#include "common.h"
#endif

class Model;

class Models {
private:
	std::vector<Model*> _models;

public:
	Models();
	void initVAO();
	void paintGL(int);
	void setMatrices(int, glm::mat4, glm::mat4, glm::mat4, glm::mat4);
	void setModel(Model*);
};

class Model {
private:


public:
	virtual void initVAO() = 0;
	virtual void paintGL() = 0;
	virtual void setMatrices(glm::mat4, glm::mat4, glm::mat4, glm::mat4) = 0;
};

Models::Models() {
}

void Models::initVAO() {
	for (int i = 0; i < _models.size(); i++) {
		_models[i]->initVAO();
	}
}

void Models::paintGL(int index) {
	_models[index]->paintGL();
}


void Models::setMatrices(int index, glm::mat4 vpMat, glm::mat4 acTransMat, glm::mat4 acRotMat, glm::mat4 acScaleMat) {
	_models[index]->setMatrices(vpMat, acTransMat, acRotMat, acScaleMat);
}

void Models::setModel(Model* model) {
	_models.push_back(model);
}


