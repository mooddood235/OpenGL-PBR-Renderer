#include <iostream>
#include <GLAD/glad.h>
#include <glfw/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Model.h"
#include "Camera.h"
#include "WindowSettings.h"

GLFWwindow* InitGLFW();
void InitGLAD();


int main()
{
    GLFWwindow* window = InitGLFW();
    InitGLAD();
    
    glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    Shader shader = Shader("src/Shaders/Default.vert", "src/Shaders/Default.frag");
    Model backpack = Model("Models/Backpack/backpack.obj");
    Camera camera = Camera();

    camera.Translate(glm::vec3(0, 0, 8));

    float lastTime = 0;
    
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
 
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.ProcessInput(window, deltaTime);

        //backpack.RotateGlobal(45 * deltaTime, glm::vec3(0, 1, 0));

        shader.SetMat4("modelMatrix", backpack.GetModelMatrix());
        shader.SetMat4("viewMatrix", glm::inverse(camera.GetModelMatrix()));
        shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());

        shader.SetVec3("lightPos", glm::vec3(-1.0f, 5.0, 3.0f) * 20.0f);

        glm::vec3 viewPos = glm::vec3(camera.GetModelMatrix()[3]);
        shader.SetVec3("viewPos", viewPos);

        shader.Use();
        std::vector<Mesh> backpackMeshes = backpack.GetMeshes();
        for (unsigned int i = 0; i < backpackMeshes.size(); i++) {
            backpackMeshes[i].BindTexturesAndSetTextureUniforms(shader);
            backpackMeshes[i].Draw();
        }
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

    GLFWwindow* window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "BasicRenderer", NULL, NULL);

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}
void InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

