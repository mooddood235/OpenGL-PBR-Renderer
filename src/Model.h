#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

class Model : GameObject{
public:
	Model(const char* modelPath) : GameObject() {
		meshes = std::vector<Mesh>();
		
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			exit(-1);
		}
		AddMeshes(scene->mRootNode, scene);
	}
	void Draw() {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			meshes[i].Draw();
		}
	}
private:
	std::vector<Mesh> meshes;

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

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			MeshVertex meshVertex = {
				glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
				glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
			};
			vertices.push_back(meshVertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		return Mesh(vertices, indices);
	}
};