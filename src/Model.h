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
#include "CubeMap.h"
#include "LightGroup.h"

class Model : public GameObject{
public:
	Model(std::string modelPath, bool flipUVS = false) : GameObject() {
		directoryPath = modelPath.substr(0, modelPath.find_last_of('/'));

		unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
		if (flipUVS) flags |= aiProcess_FlipUVs;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelPath, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			exit(-1);
		}

		AddMeshes(scene->mRootNode, scene);
	}
	std::vector<Mesh> GetMeshes() {
		return meshes;
	}
	void SetShader(Shader shader) {
		this->shader = shader;
	}
	void SetCamera(Camera camera) {
		this->camera = camera;
	}
	void SetIrradianceMap(CubeMap irradianceMap) {
		this->irradianceMap = irradianceMap;
	}
	void SetPreFilterMap(CubeMap preFilterMap) {
		this->preFilterMap = preFilterMap;
	}
	void SetBRDFIntegrationMap(Texture BRDFIntegartionMap) {
		this->BRDFIntegrationMap = BRDFIntegartionMap;
	}
	void SetLightGroup(LightGroup lightGroup) {
		this->lightGroup = lightGroup;
	}
	void SetHDRBrightness(float brightness) {
		HDRBrightness = brightness;
	}
	void Draw() {
		shader.SetMat4("modelMatrix", GetModelMatrix());
		shader.SetMat3("normalMatrix", glm::transpose(glm::inverse(GetModelMatrix())));

		shader.SetMat4("viewMatrix", glm::inverse(camera.GetModelMatrix()));
		shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
		shader.SetVec3("viewPos", glm::vec3(camera.GetModelMatrix()[3]));

		shader.SetFloat("HDRBrightness", HDRBrightness);

		lightGroup.SetLightUniforms(shader);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap.id);
		shader.SetInt("irradianceMap", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, preFilterMap.id);
		shader.SetInt("prefilterMap", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, BRDFIntegrationMap.id);
		shader.SetInt("brdfLUT", 3);

		shader.Use();
		for (unsigned int i = 0; i < meshes.size(); i++) {
			meshes[i].BindTexturesAndSetTextureUniforms(shader, 4);
			meshes[i].Draw();
		}
		Shader::Unuse();
	}
	void Draw(glm::vec3 albedo, float roughness, float metallic, float ao) {
		shader.SetVec3("diffuse", albedo);
		shader.SetFloat("roughness", roughness);
		shader.SetFloat("metallic", metallic);
		shader.SetFloat("ao", ao);

		Draw();
	}
	void JustDraw() {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			meshes[i].Draw();
		}
	}
private:
	std::string directoryPath;
	std::vector<Mesh> meshes = std::vector<Mesh>();
	std::vector<Texture> loaded_textures;

	Shader shader;
	Camera camera;
	CubeMap irradianceMap;
	CubeMap preFilterMap;
	Texture BRDFIntegrationMap;
	LightGroup lightGroup;
	float HDRBrightness;

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
			glm::vec2 uv;
			if (mesh->mTextureCoords[0]) uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else uv = glm::vec2(0);

			MeshVertex meshVertex(
				glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
				glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
				glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z),
				glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z),
				uv
			);
			
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
			std::vector<Texture> normalTextures = LoadTextures(material, aiTextureType_NORMALS);
			std::vector<Texture> metallicTextures = LoadTextures(material, aiTextureType_METALNESS);
			std::vector<Texture> roughnessTextures = LoadTextures(material, aiTextureType_SHININESS);
			std::vector<Texture> aoTextures = LoadTextures(material, aiTextureType_AMBIENT_OCCLUSION);

			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
			textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());
			textures.insert(textures.end(), metallicTextures.begin(), metallicTextures.end());
			textures.insert(textures.end(), roughnessTextures.begin(), roughnessTextures.end());
			textures.insert(textures.end(), aoTextures.begin(), aoTextures.end());
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
				if (loaded_textures[j].path.compare(texturePath) == 0) {
					texture = loaded_textures[j];
					already_loaded = true;
					break;
				}
			}
			if (!already_loaded) {
				texture = Texture(LoadTexture(texturePath, type), type, texturePath);
				loaded_textures.push_back(texture);
			}
			textures.push_back(texture);
		}
		return textures;
	}
	unsigned int LoadTexture(std::string path, aiTextureType type) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, numChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

		if (!data) {
			std::cout << "Texture at path <" << path << "> failed to load." << std::endl;
			stbi_image_free(data);
			exit(-1);
		}
		GLenum inFormat;
		GLenum outFormat;

		if (numChannels == 1) {
			inFormat = GL_RED;
			outFormat = GL_RED;
		}
		else if (numChannels == 3) {
			if (type == aiTextureType_DIFFUSE) inFormat = GL_SRGB;
			else inFormat = GL_RGB;
			outFormat = GL_RGB;
		}
		else if (numChannels == 4) {
			if (type == aiTextureType_DIFFUSE) inFormat = GL_SRGB_ALPHA;
			else inFormat = GL_RGBA;
			outFormat = GL_RGBA;
		}
		else {
			std::cout << "Texture at path <" << path << "> has unsupported channel format <" << numChannels << ">." << std::endl;
			stbi_image_free(data);
			exit(-1);
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, outFormat, GL_UNSIGNED_BYTE, data);
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