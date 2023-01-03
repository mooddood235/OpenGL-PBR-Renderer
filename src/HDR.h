#pragma once
#include "Texture.h"
#include <string>
#include <stb_image/stb_image.h>
#include <GLAD/glad.h>
#include <iostream>
#include "CubeMap.h"
#include "Shader.h"


struct HDR{
	static const int cubeMapDims = 512;

	Texture equirectangularTexture;
	CubeMap cubeMap;
	CubeMap irradianceMap;
	CubeMap preFilterMap;
	Texture BRDFIntegrationMap;

	HDR(std::string HDRPath, Shader equirectangularToCubemapShader, Shader convoluteHDRShader, Shader preFilterShader, Shader BRDFIntegrationShader) {
		LoadHDRTexture(HDRPath);
		GenerateCubeMap(equirectangularToCubemapShader);
		GenerateIrradianceMap(convoluteHDRShader);
		GeneratePreFilterMap(preFilterShader);
		GenerateBRDFIntegrationMap(BRDFIntegrationShader);
	}
private:
	void LoadHDRTexture(std::string path) {
		stbi_set_flip_vertically_on_load(true);

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
		stbi_set_flip_vertically_on_load(false);

		equirectangularTexture = Texture(id, aiTextureType_UNKNOWN, path);
	}
	void GenerateCubeMap(Shader equirectangularToCubemapShader) {
		unsigned int fbo, rbo;
		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapDims, cubeMapDims);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		for (unsigned int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, cubeMapDims, cubeMapDims, 0, GL_RGB, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 viewMatrices[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		equirectangularToCubemapShader.SetInt("equirectangularMap", 0);
		equirectangularToCubemapShader.SetMat4("projectionMatrix", projectionMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, equirectangularTexture.id);

		glViewport(0, 0, cubeMapDims, cubeMapDims); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		Model cube = Model("Models/Cube/Cube.fbx");
		equirectangularToCubemapShader.Use();

		for (unsigned int i = 0; i < 6; i++)
		{
			equirectangularToCubemapShader.SetMat4("viewMatrix", viewMatrices[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.JustDraw();
		}
		Shader::Unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);

		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);
		cubeMap = CubeMap(id, equirectangularTexture.path);
	}
	void GenerateIrradianceMap(Shader convoluteHDRShader) {
		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
				GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned int fbo;
		unsigned int rbo;

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 viewMatrices[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		convoluteHDRShader.SetInt("environmentMap", 0);
		convoluteHDRShader.SetMat4("projectionMatrix", projectionMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id);

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		Model cube = Model("Models/Cube/Cube.fbx");
		convoluteHDRShader.Use();

		for (unsigned int i = 0; i < 6; ++i)
		{
			convoluteHDRShader.SetMat4("viewMatrix", viewMatrices[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.JustDraw();		
		}
		Shader::Unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);

		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);

		irradianceMap = CubeMap(id, "NULL");
	}
	void GeneratePreFilterMap(Shader preFilterShader) {
		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 viewMatrices[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		unsigned int fbo;
		unsigned int rbo;

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		preFilterShader.SetInt("environmentMap", 0);
		preFilterShader.SetMat4("projectionMatrix", projectionMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id);

		preFilterShader.Use();

		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			Model cube = Model("Models/Cube/Cube.fbx");

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			preFilterShader.SetFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				preFilterShader.SetMat4("viewMatrix", viewMatrices[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				cube.JustDraw();
			}
		}
		Shader::Unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffers(1, &rbo);
		glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);

		preFilterMap = CubeMap(id, "NULL");
	}
	void GenerateBRDFIntegrationMap(Shader BRDFIntegrationShader) {
		unsigned int brdfLUTTexture;
		glGenTextures(1, &brdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned int fbo, rbo;

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		BRDFIntegrationShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Model("Models/Quad/Quad.fbx").JustDraw();
		Shader::Unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffers(1, &rbo);
		glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);

		BRDFIntegrationMap = Texture(brdfLUTTexture, aiTextureType_UNKNOWN, "NULL");
	}
};