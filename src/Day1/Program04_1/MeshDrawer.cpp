#include "MeshDrawer.h"
#include <fstream>
#include <iostream>
#include <regex>
 #include <sstream>
 #include <istream>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

MeshDrawer::MeshDrawer(std::string filePath) {
	_readPchFile(filePath);

	_color[0] = 0.0f;
	_color[1] = 1.0f;
	_color[2] = 1.0f;
}

MeshDrawer::~MeshDrawer() {
	
}

void MeshDrawer::_readPchFile(std::string filePath) {
	std::ifstream fs;
	fs.open(filePath, std::ios::in);
	std::string line;
	std::vector<std::string> tokens;
	char delimiter = ' ';

	std::getline(fs, line);
	line = _removeRegExp(line);
	_nNodes = std::stoi(line);

	for (int i = 0; i < _nNodes; i++) {
		std::getline(fs, line);
		line = _removeRegExp(line);
		tokens = _split(line, delimiter);

		array<float, 3> tmpArray;
		tmpArray[0] = std::stof(tokens[0]);
		tmpArray[1] = std::stof(tokens[1]);
		tmpArray[2] = std::stof(tokens[2]);
		_coords.push_back(tmpArray);
	}
	float means[3];
	for (int i = 0; i < 3; i++) {
		float sum = 0.0;
		for (int j = 0; j < _nNodes; j++) {
			sum += _coords[j][i];
		}
		float mean = (float)(sum / _nNodes);
		for (int j = 0; j < _nNodes; j++) {
			_coords[j][i] = (_coords[j][i] - mean) / 1.5;
		}
		means[i] = mean;
	}
	printf("(meanX, meanY, meanZ)= (%.03f, %.03f, %.03f)", means[0], means[1], means[2]);

	std::getline(fs, line);
	line = _removeRegExp(line);
	_nElements = std::stoi(line);

	for (int i = 0; i < _nElements; i++) {
		std::getline(fs, line);
		line = _removeRegExp(line);
		tokens = _split(line, delimiter);

		array<int, 3> tmpArray;
		tmpArray[0] = std::stoi(tokens[0]);
		tmpArray[1] = std::stoi(tokens[1]);
		tmpArray[2] = std::stoi(tokens[2]);
		_connectivity.push_back(tmpArray);
	}

	fs.close();
}

void MeshDrawer::draw() {
	glBegin(GL_TRIANGLES);
	glColor3fv(_color);
	
	float* coord;

	for (int iElement = 0; iElement < _nElements; iElement++) {
		for (int i = 0; i < 3; i++) {
			int nodeIndex = _connectivity[iElement][i];
			coord = &_coords[nodeIndex][0];
			glVertex3fv(coord);
		}
	}
	glEnd();
}

std::string MeshDrawer::_removeRegExp(std::string str) {
	str = std::regex_replace(str, std::regex("\r\n|\n|\r"), "");
	return str;
}

std::vector<std::string> MeshDrawer::_split(const std::string& s, char delim) {
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		if (!item.empty()) {
			tokens.push_back(item);
		}
	}
	return tokens;
}

void MeshDrawer::setColor(float R, float G, float B) {
	_color[0] = R;
	_color[1] = G;
	_color[2] = B;
}