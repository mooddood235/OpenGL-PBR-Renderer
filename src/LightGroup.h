#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <stdio.h>
#include "Shader.h"
#include <string>
class LightGroup{
public:
	LightGroup() {}
	LightGroup(std::vector<glm::vec3> positions, std::vector<glm::vec3> colors) {
		if (positions.size() != colors.size()) {
			printf("The number of light positions <%u> != number of light colors <%u>", positions.size(), colors.size());
			exit(-1);
		}
		this->positions = positions;
		this->colors = colors;
	}
	void SetLightUniforms(Shader shader) {
		for (unsigned int i = 0; i < positions.size(); i++) {
			shader.SetVec3("lightPositions[" + std::to_string(i) + "]", positions[i]);
			shader.SetVec3("lightColors[" + std::to_string(i) + "]", colors[i]);
		}
	}
private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;
};