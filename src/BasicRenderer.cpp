#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include "Shader.h"
#include "Triangle.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* InitGLFW();
void InitGLAD();


int main()
{
    GLFWwindow* window = InitGLFW();
    InitGLAD();
    
    glViewport(0, 0, 800, 600);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    Shader shader = Shader("src/Shaders/Default.vert", "src/Shaders/Default.frag");
    Triangle triangle = Triangle();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.SetMat4("modelMatrix", modelMatrix);
    shader.SetMat4("viewMatrix", viewMatrix);
    shader.SetMat4("projectionMatrix", projectionMatrix);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        triangle.Bind();
        shader.Use();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        Triangle::UnBind();
        Shader::Unuse();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* InitGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "BasicRenderer", NULL, NULL);

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    return window;
}
void InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

