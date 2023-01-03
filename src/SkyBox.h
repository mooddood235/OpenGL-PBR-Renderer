#pragma once
#include "Shader.h"
#include "CubeMap.h"
#include "Mesh.h"
#include "Model.h"

class SkyBox {
public:
	SkyBox(CubeMap environmentMap){
		this->environmentMap = environmentMap;
	}
	void Draw(Shader skyBoxShader, Model cube, Camera camera) {
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap.id);

		skyBoxShader.SetInt("environmentMap", 0);
		skyBoxShader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
		skyBoxShader.SetMat4("viewMatrix", glm::inverse(camera.GetModelMatrix()));
		skyBoxShader.Use();
		cube.JustDraw();
		Shader::Unuse();
		glDepthMask(GL_TRUE);
	}
private:
	CubeMap environmentMap;
};