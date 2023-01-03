#pragma once
#include <string>

struct CubeMap {
	unsigned int id;
	std::string path;

	CubeMap() {
		id = 0;
		path = "defualt_cubemap";
	}

	CubeMap(unsigned int id, std::string path) {
		this->id = id;
		this->path = path;
	}
};