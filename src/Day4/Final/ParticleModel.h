#pragma once
#ifndef _PARTICLE_MODEL_
#define _PARTICLE_MODEL_

#ifndef _COMMON_H_
#include "common.h"
#endif

#ifndef _MODEL_H_
#include "Model.h"
#endif

#include "Physics.h"

#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/regex.hpp>
#include <cstdlib>
#include <string>


class ParticleModel : public Model {
private:
	static const int NUM_DIVISIONS_THETA = 100;
	static const int NUM_DIVISIONS_PHI = 100;
	const float COEFFI_VELOCITY = 10.0f;
	const float RHO = 100.0f;
	const float TIME_SPAN = 0.001f;

	Physics* _physics = nullptr;

	int _nParticles;
	std::vector<glm::vec3> _coords;
	std::vector<glm::vec3> _velocity;
	std::vector<float> _mass;
	std::vector<float> _radiuses;
	std::vector<int> _nIndeces;
	std::vector<GLuint> _vaoIDs;

	glm::mat4 _vpMat;
	glm::mat4 _acTransMat;
	glm::mat4 _acRotMat;
	glm::mat4 _acScaleMat;

	GLuint* _programId;

	void _readFile(std::string);
	void _cartecian2Polar(glm::vec3&, float, float, float);

public:
	ParticleModel(GLuint*, std::string);
	void initVAO();
	void paintGL();
	void setMatrices(glm::mat4, glm::mat4, glm::mat4, glm::mat4);
};

struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& normal_)
		: position(position_)
		, normal(normal_) {
	}

	glm::vec3 position;
	glm::vec3 normal;
};



ParticleModel::ParticleModel(GLuint* programId, std::string filePath) {
	_programId = programId;
	_readFile(filePath);

	_physics = new Physics(&_coords, &_velocity, &_mass, &_radiuses);
	_physics->setBox(20.0f, glm::vec3(0));
	_physics->setTimeSpan(TIME_SPAN);
}

void ParticleModel::_readFile(std::string filePath) {
	std::cout << "filePath= " + filePath << std::endl;
	std::ifstream file;
	file.open(filePath, std::ios::in);
	std::string line;
	std::vector<std::string> tokens;

	std::getline(file, line);
	boost::trim(line);
	_nParticles = std::stoi(line);

	for (int i = 0; i < _nParticles; i++) {
		std::getline(file, line);
		boost::trim(line);
		boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);

		/*float radius = std::stof(tokens[3]);
		_radiuses.push_back(radius);
		glm::vec3 coord;
		coord.x = std::stof(tokens[4]);
		coord.y = std::stof(tokens[5]);
		coord.z = std::stof(tokens[6]);
		_coords.push_back(coord);

		glm::vec3 velocity;
		velocity.x = ((float)rand() / RAND_MAX - 0.5f) * COEFFI_VELOCITY;
		velocity.y = ((float)rand() / RAND_MAX - 0.5f) * COEFFI_VELOCITY;
		velocity.z = ((float)rand() / RAND_MAX - 0.5f) * COEFFI_VELOCITY;
		_velocity.push_back(velocity);

		float mass = 4.0f / 3.0f * M_PI * radius * radius * radius * RHO;
		_mass.push_back(mass);*/

		float radius = std::stof(tokens[0]);
		_radiuses.push_back(radius);

		glm::vec3 coord;
		coord.x = std::stof(tokens[1]);
		coord.y = std::stof(tokens[2]);
		coord.z = std::stof(tokens[3]);
		_coords.push_back(coord);

		glm::vec3 velocity;
		velocity.x = std::stof(tokens[4]);
		velocity.y = std::stof(tokens[5]);
		velocity.z = std::stof(tokens[6]);
		_velocity.push_back(velocity);

		float mass = std::stof(tokens[7]);
		_mass.push_back(mass);
	}
}

void ParticleModel::_cartecian2Polar(glm::vec3& vec, float radius, float theta, float phi) {
	vec[0] = radius * std::sin(theta) * std::cos(phi);
	vec[1] = radius * std::sin(theta) * std::sin(phi);
	vec[2] = radius * std::cos(theta);
}

void ParticleModel::initVAO() {
	for (int iParticle = 0; iParticle < _nParticles; iParticle++) {
		GLuint vaoID, vertexBufferID, indexBufferID;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		int idx = 0;

		float deltaTheta = M_PI / (float)NUM_DIVISIONS_THETA;
		float deltaPhi = 2.0f * M_PI / (float)NUM_DIVISIONS_PHI;

		glm::vec3 pos1, pos2, pos3, pos4, normal;
		for (int i = 0; i < NUM_DIVISIONS_THETA; i++) {
			float theta1 = (float)i * deltaTheta;
			float theta2 = (float)(i + 1) * deltaTheta;
			for (int j = 0; j < NUM_DIVISIONS_PHI; j++) {
				float phi1 = (float)j * deltaPhi;
				float phi2 = (float)(j + 1) * deltaPhi;

				_cartecian2Polar(pos1, _radiuses[i], theta1, phi1);
				_cartecian2Polar(pos2, _radiuses[i], theta1, phi2);
				_cartecian2Polar(pos3, _radiuses[i], theta2, phi1);
				_cartecian2Polar(pos4, _radiuses[i], theta2, phi2);

				_cartecian2Polar(normal, 1.0, theta1 + deltaTheta / 2.0f, phi1 + deltaPhi / 2.0f);

				Vertex v1(pos1, normal);
				vertices.push_back(v1);
				indices.push_back(idx++);

				Vertex v2(pos2, normal);
				vertices.push_back(v2);
				indices.push_back(idx++);

				Vertex v3(pos3, normal);
				vertices.push_back(v3);
				indices.push_back(idx++);

				Vertex v21(pos2, normal);
				vertices.push_back(v21);
				indices.push_back(idx++);

				Vertex v31(pos3, normal);
				vertices.push_back(v31);
				indices.push_back(idx++);

				Vertex v4(pos4, normal);
				vertices.push_back(v4);
				indices.push_back(idx++);
			}
		}

		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
			indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		_vaoIDs.push_back(vaoID);
		_nIndeces.push_back(indices.size());
	}
}

void ParticleModel::paintGL() {
	_physics->update();

	glUseProgram(*_programId);
	glm::mat4 modelMat = _acTransMat * _acRotMat * _acScaleMat;
	for (int i = 0; i < _vaoIDs.size(); i++) {
		glm::mat4 mvpMat = _vpMat * glm::translate(_coords[i]) * modelMat;

		GLuint mvpMatLocId = glGetUniformLocation(*_programId, "u_mvpMat");
		glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

		glBindVertexArray(_vaoIDs[i]);
		glDrawElements(GL_TRIANGLES, _nIndeces[i], GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void ParticleModel::setMatrices(glm::mat4 vpMat, glm::mat4 acTransMat, glm::mat4 acRotMat, glm::mat4 acScaleMat)
{
	_vpMat = vpMat;
	_acTransMat = acTransMat;
	_acRotMat = acRotMat;
	_acScaleMat = acScaleMat;
}
#endif