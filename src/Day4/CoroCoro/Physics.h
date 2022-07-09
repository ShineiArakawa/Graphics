#pragma once
#ifndef _COMMON_H_
#include "common.h"
#endif

#ifndef _PHYSICS_
#define _PHYSICS_

#include <vector>
#include <array>

class Physics {
private:
	const float GRAV_ACC = 1000.0f;
	const float DELTA_TIME = 0.01f;

	GLuint _vaoID, _nIndices;
	glm::vec3 _coords, _coords1, _coords2;
	float _mass, _radius;
	std::array<glm::vec2, 3> _range;
	int _state;

	void _cartecian2Polar(glm::vec3& vec, float radius, float theta, float phi);
	void _update(glm::mat4);
	void _judge();

public:
	static const int STATE_PLAYING = -100;
	static const int STATE_GAME_OVER = -200;
	static const int STATE_CLEAR = -300;

	Physics();
	~Physics();
	void paintGL(GLuint, glm::mat4, glm::mat4, glm::mat4);
	void initSphere(glm::vec3 offset);
	
	glm::vec3 getCoord();
	int getState();
	void setRange(std::array<glm::vec2, 3>);
};

void Physics::setRange(std::array<glm::vec2, 3> range) {
	_range = range;
}

Physics::Physics() {
	_state = STATE_PLAYING;
}

glm::vec3 Physics::getCoord() {
	return _coords;
}

int Physics::getState() {
	return _state;
}

void Physics::_cartecian2Polar(glm::vec3& vec, float radius, float theta, float phi) {
	vec[0] = radius * std::sin(theta) * std::cos(phi);
	vec[1] = radius * std::sin(theta) * std::sin(phi);
	vec[2] = radius * std::cos(theta);
}

void Physics::initSphere(glm::vec3 offset) {
	GLuint indexBufferId, vertexBufferId;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;

	float deltaTheta = M_PI / (float)NUM_DIVISIONS_THETA;
	float deltaPhi = 2.0f * M_PI / (float)NUM_DIVISIONS_PHI;

	glm::vec3 pos1, pos2, pos3, pos4, normal, color;
	glm::vec2 uv(0);
	color.x = 1.0;
	color.y = 1.0;
	color.z = 1.0;
	float id = -1.0f;
	for (int i = 0; i < NUM_DIVISIONS_THETA; i++) {
		float theta1 = (float)i * deltaTheta;
		float theta2 = (float)(i + 1) * deltaTheta;
		for (int j = 0; j < NUM_DIVISIONS_PHI; j++) {
			float phi1 = (float)j * deltaPhi;
			float phi2 = (float)(j + 1) * deltaPhi;

			_cartecian2Polar(pos1, RADIUS, theta1, phi1);
			_cartecian2Polar(pos2, RADIUS, theta1, phi2);
			_cartecian2Polar(pos3, RADIUS, theta2, phi1);
			_cartecian2Polar(pos4, RADIUS, theta2, phi2);

			_cartecian2Polar(normal, 1.0, theta1 + deltaTheta / 2.0f, phi1 + deltaPhi / 2.0f);

			Vertex v1(pos1, color, normal, uv, id);
			vertices.push_back(v1);
			indices.push_back(idx++);

			Vertex v2(pos2, color, normal, uv, id);
			vertices.push_back(v2);
			indices.push_back(idx++);

			Vertex v3(pos3, color, normal, uv, id);
			vertices.push_back(v3);
			indices.push_back(idx++);

			Vertex v21(pos2, color, normal, uv, id);
			vertices.push_back(v21);
			indices.push_back(idx++);

			Vertex v31(pos3, color, normal, uv, id);
			vertices.push_back(v31);
			indices.push_back(idx++);

			Vertex v4(pos4, color, normal, uv, id);
			vertices.push_back(v4);
			indices.push_back(idx++);
		}
	}

	_nIndices = indices.size();

	glGenVertexArrays(1, &_vaoID);
	glBindVertexArray(_vaoID);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

	_coords = offset;
	_coords1 = offset;
	_coords2 = offset;
	_radius = RADIUS;
	_mass = 1.0f;

}

void Physics::paintGL(GLuint programId, glm::mat4 modelMat, glm::mat4 vpMat, glm::mat4 rotateMat) {
	_update(rotateMat);

	glm::mat4 mvpMat = vpMat* modelMat * glm::translate(_coords);

	GLuint uid;
	uid = glGetUniformLocation(programId, "u_mvpMat");
	glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

	uid = glGetUniformLocation(programId, "u_toUseTexture");
	glUniform1f(uid, 0.0);

	uid = glGetUniformLocation(programId, "u_position");
	glUniform3fv(uid, 1, glm::value_ptr(_coords));

	glBindVertexArray(_vaoID);
	glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Physics::_update(glm::mat4 rotateMat) {
	_coords1 = _coords;
	_coords2 = _coords1;

	glm::vec3 vec1(1.0, 0.0, 0.0);
	glm::vec3 vec2(0.0, 0.0, 1.0);
	glm::vec3 vec1w = glm::vec3(rotateMat * glm::vec4(vec1, 0.0f));
	glm::vec3 vec2w = glm::vec3(rotateMat * glm::vec4(vec2, 0.0f));
	float theta = std::atan2(vec1w.y, glm::length(glm::vec2(vec1w.x, vec1w.z)));
	float phi = std::atan2(vec2w.y, glm::length(glm::vec2(vec2w.x, vec2w.z)));

	float forceX, forceY, forceZ;
	if (_state == STATE_PLAYING) {
		forceX = - std::sin(theta) * _mass * GRAV_ACC;
		forceY = 0.0f;
		forceZ = - std::sin(phi) * _mass * GRAV_ACC;
	}
	else {
		forceX = 0.0f;
		forceY = - _mass * GRAV_ACC;
		forceZ = 0.0f;
	}

	_coords = -_coords2 + glm::vec3(2.0f) * _coords1 + glm::vec3(forceX, forceY, forceZ) * DELTA_TIME * DELTA_TIME / _mass;
	
	//std::cout << "theta= " + std::to_string(theta) + ", phi= " + std::to_string(phi) << std::endl;

	_judge();
}

void Physics::_judge() {
	glm::vec3 position = _coords;

	if (position.x + _radius < _range[0][0] || position.x - _radius > _range[0][1] || position.z + _radius< _range[2][0] || position.z - _radius > _range[2][1]) {
		_state = STATE_GAME_OVER;
	}

	std::vector<glm::vec3> holePos = {
		glm::vec3(2.0, 0.0, 0.0),
		glm::vec3(-2.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 2.0),
		glm::vec3(0.0, 0.0, -2.0),

		glm::vec3(3.0, 0.0, 3.0),
		glm::vec3(3.0, 0.0, -3.0),
		glm::vec3(-3.0, 0.0, 3.0),
		glm::vec3(-3.0, 0.0, -3.0),

		glm::vec3(5.0, 0.0, 0.0),
		glm::vec3(-5.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 5.0),
		glm::vec3(0.0, 0.0, -5.0),
		
		//glm::vec3(8.0, 0.0, 0.0),
		glm::vec3(-8.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 8.0),
		glm::vec3(0.0, 0.0, -8.0),
		
		glm::vec3(7.0, 0.0, 7.0),
		glm::vec3(7.0, 0.0, -7.0),
		glm::vec3(-7.0, 0.0, 7.0),
		glm::vec3(-7.0, 0.0, -7.0),
	};
	std::vector<float> holeRadius = {
		0.4f,
		0.4f,
		0.4f,
		0.4f,

		0.4f,
		0.4f,
		0.4f,
		0.4f,

		0.4f,
		0.4f,
		0.4f,
		0.4f,
		
		//1.0f,
		1.6f,
		1.6f,
		1.6f,
		
		2.6f,
		2.6f,
		2.6f,
		2.6f,
	};

	std::cout << "x= " + std::to_string(position.x) + ", .y= " + std::to_string(position.y) + ", z= " + std::to_string(position.z) << std::endl;
	for (int i = 0; i < holePos.size(); i++) {

		float distance = glm::length(glm::vec2(holePos[i].x - position.x, holePos[i].z - position.z));

		if (distance < holeRadius[i]) {
			_state = STATE_GAME_OVER;

			if (i == 12) {
				_state = STATE_CLEAR;
			}
			break;
		}
	}
}

#endif