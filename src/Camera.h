#pragma once
#include "GameObject.h"
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "WindowSettings.h"

class Camera : public GameObject{
public:
	glm::mat4 GetProjectionMatrix() {
		return projectionMatrix;
	}
	void SetCameraSpeed(float cameraSpeed) {
		this->cameraSpeed = cameraSpeed;
	}
	void ProcessInput(GLFWwindow* window, float deltaTime) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Translate(-GetZAxis() * cameraSpeed * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Translate(GetZAxis() * cameraSpeed * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Translate(-GetXAxis() * cameraSpeed * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Translate(GetXAxis() * cameraSpeed * deltaTime);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (!firstMouse) {
			lastX = mouseX;
			lastY = mouseY;
			firstMouse = true;
		}

		double xOffset = mouseX - lastX;
		double yOffset = mouseY - lastY;

		RotateGlobal(-xOffset * mouseSensitivity, glm::vec3(0, 1, 0));
		RotateLocal(-yOffset * mouseSensitivity, glm::vec3(1, 0, 0));

		lastX = mouseX;
		lastY = mouseY;
	}
private:
	double lastX = 0;
	double lastY = 0;

	bool firstMouse = false;

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), WINDOWWIDTH / WINDOWHEIGHT, 0.1f, 100.0f);;

	float cameraSpeed = 15;
	float mouseSensitivity = 0.05f;
};