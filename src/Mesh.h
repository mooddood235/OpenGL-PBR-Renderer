#pragma once
#include <vector>
#include "MeshVertex.h"
#include "Texture.h"
#include <GLAD/glad.h>
#include "GameObject.h"
#include "Shader.h"
#include <string>
#include "Camera.h"
#include <assimp/types.h>

class Mesh : public GameObject{
public:
	Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : GameObject() {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		
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
	
	void Draw() {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	void BindTexturesAndSetTextureUniforms(Shader shader) {
		unsigned int diffuseNum = 0;
		unsigned int specularNum = 0;

		for (unsigned int i = 0; i < textures.size(); i++) {
			std::string uniformName;

			if (textures[i].type == aiTextureType_DIFFUSE) {
				uniformName = "texture_diffuse" + std::to_string(diffuseNum);
				diffuseNum++;
			}
			else if (textures[i].type == aiTextureType_SPECULAR) {
				uniformName = "texture_specular" + std::to_string(specularNum);
				specularNum++;
			}
			else continue;
			
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			shader.SetInt(uniformName, i);
		}
		glActiveTexture(GL_TEXTURE0);
	}
private:
	unsigned int vao, vbo, ebo;

	std::vector<Texture> textures;
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

};

