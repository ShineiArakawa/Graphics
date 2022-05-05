#pragma once
#include <string>
#include <array>
#include <vector>

using namespace std;

class MeshDrawer {
private:
	int _nNodes;
	int _nElements;
	vector<array<float, 3>> _coords;
	vector<array<int, 3>> _connectivity;
	float _color[3];

	void _readPchFile(std::string);
	std::string _removeRegExp(std::string);
	std::vector<std::string> _split(const std::string&, char);

public:
	MeshDrawer(std::string);
	~MeshDrawer();
	void draw();
	void setColor(float R, float G, float B);
};
