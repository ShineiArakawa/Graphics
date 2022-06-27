/*
* This is header header only module.
*/
#pragma once

#ifndef _COMMON_H_
#include "common.h"
#endif

class TransformVariable {
private:
	glm::mat4 _viewMat, _projMat, _acRotMat, _acTransMat, _acScaleMat;
	glm::vec3 _centerCoord;
	float _acScale;
	int* _windowWidth;
	int* _windowHeight;
	glm::vec3 _getVector(double x, double y);
	void _init();

public:
	TransformVariable(int*, int*, glm::vec3);
	~TransformVariable();
	void updateRotate(glm::ivec2, glm::ivec2);
	void updateTranslate(glm::ivec2, glm::ivec2);
	void updateScale();
	glm::mat4 getMvpMat();
	void setProjMat(glm::mat4);
	void setViewMat(glm::mat4);
	void addAcScale(float);
};

class Parameters {
private:
	std::vector<TransformVariable> _values;

public:
	Parameters(std::vector<glm::vec3>, int*, int*);
	~Parameters();
	void updateRotate(int, glm::ivec2, glm::ivec2);
	void updateTranslate(int, glm::ivec2, glm::ivec2);
	void updateScale(int);
	glm::mat4 getMvpMat(int);
	void setProjMat(glm::mat4);
	void setViewMat(glm::mat4);
	void addAcScale(int, float);
};

TransformVariable::TransformVariable(int* windowWidth, int* windowHeight, glm::vec3 centerCoord) {
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_centerCoord = centerCoord;

	_init();
}

TransformVariable::~TransformVariable() {

}

void TransformVariable::_init() {
	_projMat = glm::perspective(glm::radians(45.0f), (float)*_windowWidth / (float)*_windowHeight, 0.1f, 1000.0f);

	_viewMat = glm::lookAt(
		glm::vec3(3.0f, 4.0f, 5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	_acScale = 1.0f;
	_acRotMat = glm::translate(_centerCoord) * glm::mat4(1.0) * glm::translate(-_centerCoord);
	_acTransMat = glm::translate(_centerCoord) * glm::mat4(1.0) * glm::translate(-_centerCoord);
	_acScaleMat = glm::translate(_centerCoord) * glm::mat4(1.0) * glm::translate(-_centerCoord);
}


glm::vec3 TransformVariable::_getVector(double x, double y) {
	const int shortSide = std::min(*_windowWidth, *_windowHeight);
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

void TransformVariable::updateRotate(glm::ivec2 oldPos, glm::ivec2 newPos) {
	const glm::vec3 u = _getVector((oldPos).x, (oldPos).y);
	const glm::vec3 v = _getVector((newPos).x, (newPos).y);

	const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));

	const glm::vec3 rotAxis = glm::cross(u, v);

	glm::mat4 modelMat = _acTransMat * _acRotMat * _acScaleMat;
	const glm::mat4 c2mMat = glm::inverse(modelMat * _viewMat);
	const glm::vec3 rotAxisModelSpace = glm::vec3(c2mMat * glm::vec4(rotAxis, 0.0f));

	_acRotMat = glm::translate(_centerCoord) * glm::rotate((float)(4.0 * angle), rotAxisModelSpace) * glm::translate(-_centerCoord) * _acRotMat;
}

void TransformVariable::updateTranslate(glm::ivec2 oldPos, glm::ivec2 newPos) {
	glm::vec4 originScreenSpace = (_projMat * _viewMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	originScreenSpace /= originScreenSpace.w;

	glm::vec4 newPosScreenSpace(2.0f * (newPos).x / *_windowWidth - 1.0f, -2.0f * (newPos).y / *_windowHeight + 1.0f, originScreenSpace.z, 1.0f);
	glm::vec4 oldPosScreenSpace(2.0f * (oldPos).x / *_windowWidth - 1.0f, -2.0f * (oldPos).y / *_windowHeight + 1.0f, originScreenSpace.z, 1.0f);

	glm::mat4 invVpMat = glm::inverse(_projMat * _viewMat);

	glm::vec4 newPosObjSpace = invVpMat * newPosScreenSpace;
	glm::vec4 oldPosObjSpace = invVpMat * oldPosScreenSpace;
	newPosObjSpace /= newPosObjSpace.w;
	oldPosObjSpace /= oldPosObjSpace.w;

	const glm::vec3 transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

	_acTransMat = glm::translate(transWorldSpace) * _acTransMat;
}

void TransformVariable::updateScale() {
	_acScaleMat = glm::translate(_centerCoord) * glm::scale(glm::vec3(_acScale, _acScale, _acScale)) * glm::translate(-_centerCoord);
}

glm::mat4 TransformVariable::getMvpMat()
{
	glm::mat4 modelMat = _acTransMat * _acRotMat * _acScaleMat;
	glm::mat4 mvpMat = _projMat * _viewMat * modelMat;
	return mvpMat;
}

void TransformVariable::setProjMat(glm::mat4 projMat) {
	_projMat = projMat;
}

void TransformVariable::setViewMat(glm::mat4 viewMat) {
	_viewMat = viewMat;
}

void TransformVariable::addAcScale(float addValue) {
	_acScale += addValue;
}

Parameters::Parameters(std::vector<glm::vec3> centerCoords, int* windowWidth, int* windowHeight)
{
	for (int i = 0; i < centerCoords.size(); i++) {
		TransformVariable transVal(windowWidth, windowHeight, centerCoords[i]);
		_values.push_back(transVal);
	}
}

Parameters::~Parameters()
{

}

void Parameters::updateRotate(int index, glm::ivec2 oldPos, glm::ivec2 newPos)
{
	if (index >= 0 && index < _values.size()) {
		_values[index].updateRotate(oldPos, newPos);
	}
}

void Parameters::updateTranslate(int index, glm::ivec2 oldPos, glm::ivec2 newPos)
{
	if (index >= 0 && index < _values.size()) {
		_values[index].updateTranslate(oldPos, newPos);
	}
}

void Parameters::updateScale(int index)
{
	if (index >= 0 && index < _values.size()) {
		_values[index].updateScale();
	}
}

glm::mat4 Parameters::getMvpMat(int index)
{
	if (index >= 0 && index < _values.size()) {
		return _values[index].getMvpMat();
	}

	return glm::mat4(0.0);
}

void Parameters::setProjMat(glm::mat4 projMat)
{
	for (int i = 0; i < _values.size(); i++) {
		_values[i].setProjMat(projMat);
	}
}

void Parameters::setViewMat(glm::mat4 viewMat)
{
	for (int i = 0; i < _values.size(); i++) {
		_values[i].setViewMat(viewMat);
	}
}


void Parameters::addAcScale(int index, float addValue)
{
	if (index >= 0 && index < _values.size()) {
		_values[index].addAcScale(addValue);
	}
}
