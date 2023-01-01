#pragma once
#include <string>
#include <assimp/scene.h>

struct Texture {
	unsigned int id;
	aiTextureType type;
	std::string path;

	Texture(unsigned int id, aiTextureType type, std::string path) {
		this->id = id;
		this->type = type;
		this->path = path;
	}
	Texture() {
		id = 0;
		type = aiTextureType_NONE;
		path = "texture_default";
	}
};
