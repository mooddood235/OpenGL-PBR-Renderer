#pragma once
#include "Texture.h"
#include <string>
#include <stb_image/stb_image.h>
#include <GLAD/glad.h>
#include <iostream>
#include "CubeMap.h"

struct HDR{
	Texture texture;
	CubeMap cubeMap;

	HDR(std::string HDRPath) {
		LoadHDRTexture(HDRPath);
		GenerateCubeMap();
	}
private:
	void LoadHDRTexture(std::string path) {
		int width, height, numChannels;
		float* data = stbi_loadf(path.c_str(), &width, &height, &numChannels, 0);

		if (!data) {
			std::cout << "Could not load texture at path <" << path << ">.";
			stbi_image_free(data);
			exit(-1);
		}

		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		texture = Texture(id, aiTextureType_UNKNOWN, path);
	}
	void GenerateCubeMap() {

	}
};