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

    Shader meshShader = Shader("src/Shaders/PBR.vert", "src/Shaders/PBR.frag");
    Model sphere = Model("Models/Sphere/Sphere.fbx");
    Camera camera = Camera();

    camera.Translate(glm::vec3(0, 0, 8));
    sphere.RotateGlobal(90, glm::vec3(1, 0, 0));

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

        meshShader.SetMat4("modelMatrix", sphere.GetModelMatrix());
        meshShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(sphere.GetModelMatrix())));
        meshShader.SetMat4("viewMatrix", glm::inverse(camera.GetModelMatrix()));
        meshShader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());

        meshShader.SetVec3("viewPos", glm::vec3(camera.GetModelMatrix()[3]));

        meshShader.SetVec3("lightPositions[0]", glm::vec3(2));
        meshShader.SetVec3("lightColors[0]", glm::vec3(1));
        meshShader.SetVec3("lightPositions[1]", glm::vec3(0, 2, 0));
        meshShader.SetVec3("lightColors[1]", glm::vec3(1));


        meshShader.Use();
        std::vector<Mesh> sphereMeshes = sphere.GetMeshes();
        for (unsigned int i = 0; i < sphereMeshes.size(); i++) {
            sphereMeshes[i].BindTexturesAndSetTextureUniforms(meshShader);
            sphereMeshes[i].Draw();
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

