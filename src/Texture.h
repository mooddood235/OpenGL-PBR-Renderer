#pragma once
#include <string>


enum TextureType{
	DIFFUSE,
	SPECULAR
};

struct Texture {
	unsigned int id;
	unsigned int type;
	std::string path;
};