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

		GenerateGeometry();
	}

	void Draw() {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void BindTexturesAndSetTextureUniforms(Shader shader) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		unsigned int diffuseIndex = 0;
		unsigned int specularIndex = 0;

		std::string array_name;
		unsigned int index;

		for (unsigned int i = 0; i < textures.size(); i++) {
			if (textures[i].type == aiTextureType_DIFFUSE) {
				array_name = "texture_diffuse";
				index = diffuseIndex;
				diffuseIndex++;
			}
			else if (textures[i].type == aiTextureType_SPECULAR) {
				array_name = "texture_specular";
				index = specularIndex;
				specularIndex++;
			}
			else continue;

			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			shader.SetInt(array_name + "[" + std::to_string(index) + "]", i + 1);
		}
		if (diffuseIndex == 0) shader.SetInt("texture_diffuse[0]", 0);
		if (specularIndex == 0) shader.SetInt("texture_specular[0]", 0);

	}
private:
	unsigned int vao, vbo, ebo;

	std::vector<Texture> textures;

	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

	void GenerateGeometry() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, uv));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

