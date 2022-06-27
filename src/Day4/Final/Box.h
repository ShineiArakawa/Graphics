#pragma once

#include "Environ.h"

class Box : public Environ
{
private:
    const int LABEL_FACE_INVALID = -1;
    const int LABEL_FACE_XY_MIN = 0;
    const int LABEL_FACE_XY_MAX = 1;
    const int LABEL_FACE_YZ_MIN = 2;
    const int LABEL_FACE_YZ_MAX = 3;
    const int LABEL_FACE_ZX_MIN = 4;
    const int LABEL_FACE_ZX_MAX = 5;
    const std::array<int, 6> LIST_LABEL_FACE = {
        LABEL_FACE_XY_MIN, LABEL_FACE_XY_MAX, LABEL_FACE_YZ_MIN,
        LABEL_FACE_YZ_MAX, LABEL_FACE_ZX_MIN, LABEL_FACE_ZX_MAX
    };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_XY_MIN = { 0.0f, 0.0f, 1.0f };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_XY_MAX = { 0.0f, 0.0f, -1.0f };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_YZ_MIN = { 1.0f, 0.0f, 0.0f };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_YZ_MAX = { -1.0f, 0.0f, 0.0f };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_ZX_MIN = { 0.0f, 1.0f, 0.0f };
    const glm::vec3 UNIT_PERPENDICULAR_VECTOR_ZX_MAX = { 0.0f, -1.0f, 0.0f };
    const std::array<glm::vec3, 6> LIST_UNIT_PERPENDICULAR_VECTOR = {
        UNIT_PERPENDICULAR_VECTOR_XY_MIN,
        UNIT_PERPENDICULAR_VECTOR_XY_MAX,
        UNIT_PERPENDICULAR_VECTOR_YZ_MIN,
        UNIT_PERPENDICULAR_VECTOR_YZ_MAX,
        UNIT_PERPENDICULAR_VECTOR_ZX_MIN,
        UNIT_PERPENDICULAR_VECTOR_ZX_MAX
    };

    float _boxSize;
    glm::vec2 _boundsX;
    glm::vec2 _boundsY;
    glm::vec2 _boundsZ;

    std::vector<glm::vec3>* _coords;
    std::vector<glm::vec3>* _velocity;
    std::vector<float>* _mass;
    std::vector<float>* _radiuses;

    glm::vec3 _calcSymmetricVector(glm::vec3, glm::vec3);

public:
    Box(float, glm::vec3, std::vector<glm::vec3>*, std::vector<glm::vec3>*, std::vector<float>*, std::vector<float>*);
    ~Box();
	
    void reboundWithEnviron();
};

Box::Box(
	float boxSize,
	glm::vec3 centerCoord,
	std::vector<glm::vec3>* coords,
	std::vector<glm::vec3>* velocity,
	std::vector<float>* mass,
	std::vector<float>* radiuses)
{
	_boxSize = boxSize;
	_boundsX[0] = -boxSize / 2.0f + centerCoord[0];
	_boundsX[1] = boxSize / 2.0f + centerCoord[0];
	_boundsY[0] = -boxSize / 2.0f + centerCoord[1];
	_boundsY[1] = boxSize / 2.0f + centerCoord[1];
	_boundsZ[0] = -boxSize / 2.0f + centerCoord[2];
	_boundsZ[1] = boxSize / 2.0f + centerCoord[2];

	_coords = coords;
	_velocity = velocity;
	_mass = mass;
	_radiuses = radiuses;
}

Box::~Box()
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

void Box::reboundWithEnviron()
{
	int nParticles = _coords->size();
	glm::vec3 minCoord;
	glm::vec3 maxCoord;
	glm::vec3 coord;
	std::vector<int> faceID;
	std::array<int, 6> distanceToWall;

	for (int i = 0; i < nParticles; i++) {
		faceID.clear();
		coord = (*_coords)[i];
		float radius = (*_radiuses)[i];

		minCoord = coord - radius;
		maxCoord = coord + radius;

		if (minCoord[0] < _boundsX[0]) {
			faceID.push_back(LABEL_FACE_YZ_MIN);
		}
		if (minCoord[1] < _boundsY[0]) {
			faceID.push_back(LABEL_FACE_ZX_MIN);
		}
		if (minCoord[2] < _boundsZ[0]) {
			faceID.push_back(LABEL_FACE_XY_MIN);
		}
		if (maxCoord[0] > _boundsX[1]) {
			faceID.push_back(LABEL_FACE_YZ_MAX);
		}
		if (maxCoord[1] > _boundsY[1]) {
			faceID.push_back(LABEL_FACE_ZX_MAX);
		}
		if (maxCoord[2] > _boundsZ[1]) {
			faceID.push_back(LABEL_FACE_XY_MAX);
		}

		if ((int)faceID.size() == 0) {
			continue;
		}
		else {
			distanceToWall[0] = _boundsZ[0] - minCoord[2];
			distanceToWall[1] = maxCoord[2] - _boundsZ[1];
			distanceToWall[2] = _boundsX[0] - minCoord[0];
			distanceToWall[3] = maxCoord[0] - _boundsX[1];
			distanceToWall[4] = _boundsY[0] - minCoord[1];
			distanceToWall[5] = maxCoord[1] - _boundsY[1];
			int argMax = 0;
			for (int j = 1; j < 6; j++) {
				if (distanceToWall[argMax] < distanceToWall[j]) {
					argMax = j;
				}
			}

			const int mostDeepHit = LIST_LABEL_FACE[argMax];
			const glm::vec3 perpendicularVector = LIST_UNIT_PERPENDICULAR_VECTOR[mostDeepHit];
			const glm::vec3 preVelocity = (*_velocity)[i];

			const glm::vec3  afterVelocity = _calcSymmetricVector(preVelocity, perpendicularVector);

			(*_velocity)[i] = -afterVelocity;
		}
	}
}

glm::vec3 Box::_calcSymmetricVector(glm::vec3 vector, glm::vec3 pole)
{
	glm::vec3 unitPole = glm::normalize(pole);

	glm::vec3 symmetricVector = -vector + 2.0f * glm::dot(vector, unitPole) * unitPole;

	return symmetricVector;
}