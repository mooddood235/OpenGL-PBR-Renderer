#pragma once
#include <glm/glm.hpp>

struct MeshVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 biTangent;
	glm::vec2 uv;

	MeshVertex() {
		position = glm::vec3(0);
		normal = glm::vec3(0);
		tangent = glm::vec3(0);
		biTangent = glm::vec3(0);
		uv = glm::vec2(0);
	}

	MeshVertex(glm::vec3 position, glm::vec3 normal, glm::vec3 tangent, glm::vec3 biTangent, glm::vec2 uv) {
		this->position = position;
		this->normal = normal;
		this->tangent = tangent;
		this->biTangent = biTangent;
		this->uv = uv;
	}
};