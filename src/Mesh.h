#pragma once
#include <vector>
#include "MeshVertex.h"
#include "Texture.h"
#include <GLAD/glad.h>
#include "GameObject.h"
#include "Shader.h"
#include <string>
#include "Camera.h"
#include <assimp/scene.h>
#include <iostream>

class Mesh : public GameObject{
public:
	Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : GameObject() {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		SortTextures();
		GenerateGeometry();
	}

	void Draw() {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void SortTextures() {
		for (unsigned int i = 0; i < textures.size(); i++) {
			if (textures[i].type == aiTextureType_DIFFUSE) diffuseTextures.push_back(i);
			else if (textures[i].type == aiTextureType_SPECULAR) specularTextures.push_back(i);
			else if (textures[i].type == aiTextureType_NORMALS) normalTextures.push_back(i);
			else if (textures[i].type == aiTextureType_METALNESS) metallicTextures.push_back(i);
			else if (textures[i].type == aiTextureType_SHININESS) roughnessTextures.push_back(i);
			else if (textures[i].type == aiTextureType_AMBIENT_OCCLUSION) aoTextures.push_back(i);
		}
	}

	void BindTexturesAndSetTextureUniforms(Shader shader) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		unsigned int textureID = 1;

		auto helper = [&](std::vector<unsigned int> textureArray, std::string textureName) {
			if (textureArray.size() == 0) shader.SetInt(textureName, 0);
			else {
				glActiveTexture(GL_TEXTURE0 + textureID);
				glBindTexture(GL_TEXTURE_2D, textures[textureArray[0]].id);
				shader.SetInt(textureName, textureID);
				textureID++;
			}
		};
		helper(diffuseTextures, "albedoTexture");
		helper(specularTextures, "specularTextures");
		helper(normalTextures, "normalTexture");
		helper(metallicTextures, "metallicTexture");
		helper(roughnessTextures, "roughnessTexture");
		helper(aoTextures, "aoTexture");
	}
private:
	unsigned int vao, vbo, ebo;

	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<Texture> textures;
	std::vector<unsigned int> diffuseTextures;
	std::vector<unsigned int> specularTextures;
	std::vector<unsigned int> normalTextures;
	std::vector<unsigned int> metallicTextures;
	std::vector<unsigned int> roughnessTextures;
	std::vector<unsigned int> aoTextures;

	void GenerateGeometry() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, tangent));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, biTangent));
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, uv));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

