#pragma once
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------

    Shader() {
        shaderProgram = -1;
    }
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex);
        glAttachShader(shaderProgram, fragment);
        glLinkProgram(shaderProgram);
        checkCompileErrors(shaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
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
	void SetFloat(std::string name, float value) {
		int location = glGetUniformLocation(shaderProgram, name.c_str());
		glProgramUniform1f(shaderProgram, location, value);
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
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};



//#pragma once
//#include <iostream>
//#include <GLAD/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//class Shader {
//public:
//	Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
//		int vertexShaderID = CompileShader(vertexShaderPath, GL_VERTEX_SHADER);
//		int fragmentShaderID = CompileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
//
//		shaderProgram = glCreateProgram();
//		glAttachShader(shaderProgram, vertexShaderID);
//		glAttachShader(shaderProgram, fragmentShaderID);
//		glLinkProgram(shaderProgram);
//
//		int  success;
//		char infoLog[512];
//
//		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//
//		if (!success) {
//			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//			printf("Shader program failed to link. %s", infoLog);
//			exit(-1);
//		}
//	}
//	void Use() {
//		glUseProgram(shaderProgram);
//	}
//	void SetMat4(std::string name, glm::mat4 value) {
//		int location = glGetUniformLocation(shaderProgram, name.c_str());
//		glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, glm::value_ptr(value));
//	}
//	void SetMat3(std::string name, glm::mat3 value) {
//		int location = glGetUniformLocation(shaderProgram, name.c_str());
//		glProgramUniformMatrix3fv(shaderProgram, location, 1, GL_FALSE, glm::value_ptr(value));
//	}
//	void SetVec3(std::string name, glm::vec3 value) {
//		int location = glGetUniformLocation(shaderProgram, name.c_str());
//		glProgramUniform3fv(shaderProgram, location, 1, glm::value_ptr(value));
//	}
//	void SetFloat(std::string name, float value) {
//		int location = glGetUniformLocation(shaderProgram, name.c_str());
//		glProgramUniform1f(shaderProgram, location, value);
//	}
//	void SetInt(std::string name, int value) {
//		int location = glGetUniformLocation(shaderProgram, name.c_str());
//		glProgramUniform1i(shaderProgram, location, value);
//	}
//
//	static void Unuse() {
//		glUseProgram(0);
//	}
//
//private:
//	unsigned int shaderProgram;
//
//	int CompileShader(const char* shaderPath, int shaderType) {
//		if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER) {
//			std::cout << "Shader type is invalid." << std::endl;
//			exit(-1);
//		}
//
//		FILE* shader = fopen(shaderPath, "r");
//
//		if (!shader) {
//			printf("Shader path: <%s> is invalid.", shaderPath);
//			exit(-1);
//		}
//
//		fseek(shader, 0, SEEK_END);
//		unsigned int shaderSize = ftell(shader);
//		rewind(shader);
//
//		char* shaderString = (char*)malloc(sizeof(char) * (shaderSize + 1));
//		memset(shaderString, '\0', shaderSize);
//
//		fread(shaderString, sizeof(char), shaderSize, shader);
//
//		fclose(shader);
//
//		unsigned int shaderID = glCreateShader(shaderType);
//		glShaderSource(shaderID, 1, &shaderString, NULL);
//		glCompileShader(shaderID);
//
//		free(shaderString);
//
//		int  success;
//		char infoLog[512];
//		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
//
//		if (!success)
//		{
//			glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
//			printf("Shader compilation for <%s> failed. %s", shaderPath, infoLog);
//			exit(-1);
//		}
//
//		return shaderID;
//	}
//};