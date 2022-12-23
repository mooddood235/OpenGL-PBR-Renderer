#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameObject {
public:
	GameObject() {
		modelMatrix = glm::mat4(1);
	}

	void Translate(glm::vec3 translation) {
		modelMatrix = glm::translate(modelMatrix, translation);
	}
	void Scale(glm::vec3 scale) {
		modelMatrix = glm::scale(modelMatrix, scale);
	}
	void Rotate(glm::vec3 rotation) {
		modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	}
	
private:
	glm::mat4 modelMatrix;
};