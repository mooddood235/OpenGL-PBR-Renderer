#pragma once
#include <GLAD/glad.h>

class Triangle {
public:
	Triangle() {
		glGenVertexArrays(1, &vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void Bind() {
		glBindVertexArray(vao);
	}
	static void UnBind() {
		glBindVertexArray(0);
	}
private:
	unsigned int vao;
};
