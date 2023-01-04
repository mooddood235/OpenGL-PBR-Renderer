#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameObject {
public:
	void Translate(glm::vec3 translation) {
		translationMatrix = glm::translate(translationMatrix, translation);
	}
	void SetPosition(glm::vec3 position) {
		translationMatrix = glm::translate(glm::mat4(1), position);
	}
	void Scale(glm::vec3 scale) {
		scaleMatrix = glm::scale(scaleMatrix, scale);
	}
	void RotateLocal(float angleInDegrees, glm::vec3 axis){
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angleInDegrees), axis);
	}
	void RotateGlobal(float angleInDegrees, glm::vec3 axis) {
		glm::vec3 localAxis = glm::mat3(glm::inverse(rotationMatrix)) * axis;
		RotateLocal(angleInDegrees, localAxis);
	}
	glm::vec3 GetXAxis() {
		return glm::mat3(rotationMatrix) * glm::vec3(1, 0, 0);
	}
	glm::vec3 GetYAxis() {
		return glm::mat3(rotationMatrix) * glm::vec3(0, 1, 0);
	}
	glm::vec3 GetZAxis() {
		return glm::mat3(rotationMatrix) * glm::vec3(0, 0, 1);
	}
 	glm::mat4 GetModelMatrix() {
		return translationMatrix * rotationMatrix * scaleMatrix;
	}
	
	
protected:
	glm::mat4 translationMatrix = glm::mat4(1);
	glm::mat4 scaleMatrix = glm::mat4(1);
	glm::mat4 rotationMatrix = glm::mat4(1);
};