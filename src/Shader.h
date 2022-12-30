#pragma once
#include <iostream>
#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
		int vertexShaderID = CompileShader(vertexShaderPath, GL_VERTEX_SHADER);
		int fragmentShaderID = CompileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShaderID);
		glAttachShader(shaderProgram, fragmentShaderID);
		glLinkProgram(shaderProgram);

		int  success;
		char infoLog[512];

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			printf("Shader program failed to link. %s", infoLog);
			exit(-1);
		}
	}
	void Use() {
		glUseProgram(shaderProgram);
	}
	void SetMat4(std::string name, glm::mat4 value) {
		int location = glGetUniformLocation(shaderProgram, name.c_str());
		glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void SetMat3(std::string name, glm::mat3 value) {
		int location = glGetUniformLocation(shaderProgram, name.c_str());
		glProgramUniformMatrix3fv(shaderProgram, location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void SetVec3(std::string name, glm::vec3 value) {
		int location = glGetUniformLocation(shaderProgram, name.c_str());
		glProgramUniform3fv(shaderProgram, location, 1, glm::value_ptr(value));
	}
	void SetInt(std::string name, int value) {
		int location = glGetUniformLocation(shaderProgram, name.c_str());
		glProgramUniform1i(shaderProgram, location, value);
	}

	static void Unuse() {
		glUseProgram(0);
	}

private:
	unsigned int shaderProgram;

	int CompileShader(const char* shaderPath, int shaderType) {
		if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER) {
			std::cout << "Shader type is invalid." << std::endl;
			exit(-1);
		}

		FILE* shader = fopen(shaderPath, "r");

		if (!shader) {
			printf("Shader path: <%s> is invalid.", shaderPath);
			exit(-1);
		}

		fseek(shader, 0, SEEK_END);
		unsigned int shaderSize = ftell(shader);
		rewind(shader);

		char* shaderString = (char*)malloc(sizeof(char) * (shaderSize + 1));
		memset(shaderString, '\0', shaderSize);

		fread(shaderString, sizeof(char), shaderSize, shader);

		fclose(shader);

		unsigned int shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &shaderString, NULL);
		glCompileShader(shaderID);

		free(shaderString);

		int  success;
		char infoLog[512];
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
			printf("Shader compilation for <%s> failed. %s", shaderPath, infoLog);
			exit(-1);
		}

		return shaderID;
	}
};