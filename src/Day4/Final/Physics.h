#pragma once

#ifndef _COMMON_H_
#include "common.h"
#endif

#include "Environ.h"
#include "Box.h"

#include <vector>
#include <array>

class Physics {
private:
    const float COEFFICIENT_OF_RESTITUTION = 1.0f;

    Environ* _environment = nullptr;
    float _timeSpan = 0.0f;
    std::vector<glm::vec3>* _coords = nullptr;
    std::vector<glm::vec3>* _velocity = nullptr;
    std::vector<float>* _mass = nullptr;
    std::vector<float>* _radiuses = nullptr;

    void _reboundWithOtherParticle(int i, int j);
    void _reboundWithEnviron();
    std::vector<std::array<int, 2>> _calcCombination(int);
    void _forward();
    bool _isHitOtherParticle(int, int);

public:
    Physics(std::vector<glm::vec3>*, std::vector<glm::vec3>*, std::vector<float>*, std::vector<float>*);
    ~Physics();
    void update();
    void setBox(float, glm::vec3);
    void setTimeSpan(float);
};

Physics::Physics(
	std::vector<glm::vec3>* coords,
	std::vector<glm::vec3>* velocity,
	std::vector<float>* mass,
	std::vector<float>* radiuses
) {
	_coords = coords;
	_velocity = velocity;
	_mass = mass;
	_radiuses = radiuses;
}

void Physics::setBox(
	float boxSize,
	glm::vec3 center
) {
	_environment = new Box(
		boxSize,
		center,
		_coords,
		_velocity,
		_mass,
		_radiuses
	);
}

void Physics::setTimeSpan(float timeSpan) {
	_timeSpan = timeSpan;
}



Physics::~Physics()
{
	if (_coords != nullptr) {
		free(_coords);
	}
	if (_velocity != nullptr) {
		free(_velocity);
	}
	if (_mass != nullptr) {
		free(_mass);
	}
	if (_radiuses != nullptr) {
		free(_radiuses);
	}
}

void Physics::_forward() {
	for (int i = 0; i < (*_coords).size(); i++) {
		(*_coords)[i] += (*_velocity)[i] * _timeSpan;
	}
}

bool Physics::_isHitOtherParticle(int i, int j) {
	bool isHit = false;
	float distance = glm::length((*_coords)[i] - (*_coords)[j]);
	if (distance <= (*_radiuses)[i] + (*_radiuses)[j]) {
		isHit = true;
	}
	return isHit;
}

void Physics::update()
{
	int nParticles = _coords->size();
	_forward();

	std::vector<std::array<int, 2>> lsCombi = _calcCombination(nParticles);
	for (int iCombi = 0; iCombi < lsCombi.size(); iCombi++) {
		int i = lsCombi[iCombi][0];
		int j = lsCombi[iCombi][1];
		bool isHit = _isHitOtherParticle(i, j);
		if (isHit) {
			_reboundWithOtherParticle(i, j);
		}
	}

	_reboundWithEnviron();
}

void Physics::_reboundWithOtherParticle(int i, int j)
{
	glm::vec3 iCoord = (*_coords)[i];
	glm::vec3 jCoord = (*_coords)[j];
	glm::vec3 iVelocity = (*_velocity)[i];
	glm::vec3 jVelocity = (*_velocity)[j];
	float iRadius = (*_radiuses)[i];
	float jRadius = (*_radiuses)[j];
	float iMass = (*_mass)[i];
	float jMass = (*_mass)[i];
	glm::vec3 unitActionLineVector = glm::normalize(jCoord - iCoord);
	float v1PreParallel = glm::dot(unitActionLineVector, iVelocity);
	float v2PreParallel = glm::dot(-unitActionLineVector, jVelocity); // <= ‹^–â

	// float lenPre = glm::length(iVelocity + jVelocity);

	const glm::vec3 v1AfterParallel = (v1PreParallel
		+ jMass * (1 + COEFFICIENT_OF_RESTITUTION) * (v2PreParallel - v1PreParallel) / (iMass + jMass))
		* unitActionLineVector;
	const glm::vec3 v2AfterParallel = (v1PreParallel
		- iMass * (1 + COEFFICIENT_OF_RESTITUTION) * (v2PreParallel - v1PreParallel) / (iMass + jMass))
		* unitActionLineVector;

	const glm::vec3 iVelocityAfter = iVelocity - unitActionLineVector * v1PreParallel + v1AfterParallel;
	const glm::vec3 jVelocityAfter = jVelocity + unitActionLineVector * v2PreParallel + v2AfterParallel;

	// float lenAfter = glm::length(iVelocityAfter + jVelocityAfter);

	(*_velocity)[i] = iVelocityAfter;
	(*_velocity)[j] = jVelocityAfter;
}

void Physics::_reboundWithEnviron()
{
	_environment->reboundWithEnviron();
}

std::vector<std::array<int, 2>> Physics::_calcCombination(int n)
{
	std::vector<std::array<int, 2>> combination;
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			std::array<int, 2> combi;
			combi[0] = i;
			combi[1] = j;
			combination.push_back(combi);
		}
	}
	return combination;
}


