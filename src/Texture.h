#pragma once
#include <string>
#include <assimp/scene.h>


struct Texture {
	unsigned int id;
	aiTextureType type;
	std::string path;
};