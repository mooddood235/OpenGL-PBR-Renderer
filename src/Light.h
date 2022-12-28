#pragma once
#include "GameObject.h"
#include <glm/glm.hpp>

class Light : public GameObject{
public:
	glm::vec3 color;
	float brightness;

	Light(glm::vec3 color, float brightness) {
		this->color = color;
		this->brightness = brightness;
	}
};