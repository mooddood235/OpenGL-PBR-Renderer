#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "Texture.h"
#include "Camera.h"
#include <stb_image/stb_image.h>

class Model : public GameObject{
public:
	Model(std::string modelPath) : GameObject() {
		directoryPath = modelPath.substr(0, modelPath.find_last_of('/'));

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			exit(-1);
		}

		AddMeshes(scene->mRootNode, scene);
	}
	std::vector<Mesh> GetMeshes() {
		return meshes;
	}
private:
	std::string directoryPath;
	std::vector<Mesh> meshes = std::vector<Mesh>();
	std::vector<Texture> loaded_textures;

	void AddMeshes(aiNode *node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ConvertMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			AddMeshes(node->mChildren[i], scene);
		}
	}
	Mesh ConvertMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			MeshVertex meshVertex = {
				glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
				glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
			};
			if (mesh->mTextureCoords[0])
				meshVertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else meshVertex.uv = glm::vec2(0);
			
			vertices.push_back(meshVertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Texture> diffuseTextures = LoadTextures(material, aiTextureType_DIFFUSE);
			std::vector<Texture> specularTextures = LoadTextures(material, aiTextureType_SPECULAR);

			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
		}

		return Mesh(vertices, indices, textures);
	}
	std::vector<Texture> LoadTextures(aiMaterial* material, aiTextureType type) {
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
			aiString textureFileName;
			material->GetTexture(type, i, &textureFileName);

			std::string texturePath = directoryPath + "/" + textureFileName.C_Str();
			
			Texture texture;

			bool already_loaded = false;

			for (unsigned int j = 0; j < loaded_textures.size(); j++) {
				if (loaded_textures[j].path == texturePath) texture = loaded_textures[j];
				already_loaded = true;
			}
			if (!already_loaded) {
				texture.id = LoadTexture(texturePath);
				if (type == aiTextureType_DIFFUSE) texture.type = DIFFUSE;
				else texture.type = SPECULAR;
				texture.path = texturePath;

				loaded_textures.push_back(texture);
			}
		
			textures.push_back(texture);
		}
		return textures;
	}
	unsigned int LoadTexture(std::string path) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, numChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

		if (!data) {
			std::cout << "Texture at path <" << path << "> failed to load." << std::endl;
			stbi_image_free(data);
			exit(-1);
		}
		GLenum format;

		if (numChannels == 1) format = GL_RED;
		else if (numChannels == 3) format = GL_RGB;
		else if (numChannels == 4) format = GL_RGBA;
		else {
			std::cout << "Texture at path <" << path << "> has unsupported channel format <" << numChannels << ">." << std::endl;
			stbi_image_free(data);
			exit(-1);
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}
};