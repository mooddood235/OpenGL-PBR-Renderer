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
#include "HDR.h"
#include "SkyBox.h"
#include "LightGroup.h"

GLFWwindow* InitGLFW();
void InitGLAD();


int main()
{
    GLFWwindow* window = InitGLFW();
    InitGLAD();
    
    glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    Shader PBRShader = Shader("src/Shaders/PBR.vert", "src/Shaders/PBR.frag");
    Shader PBRSimpleShader = Shader("src/Shaders/PBR.vert", "src/Shaders/PBRSimple.frag");
    Shader equirectangularToCubeMapShader = Shader("src/Shaders/equirectangularToCubemap.vert", "src/Shaders/equirectangularToCubemap.frag");
    Shader convoluteHDRShader = Shader("src/Shaders/equirectangularToCubemap.vert", "src/Shaders/ConvoluteHDR.frag");
    Shader preFilterShader = Shader("src/Shaders/equirectangularToCubemap.vert", "src/Shaders/PreFilter.frag");
    Shader BRDFIntegrationShader = Shader("src/Shaders/NDC.vert", "src/Shaders/BRDFIntegration.frag");

    Shader skyBoxShader = Shader("src/Shaders/SkyBox.vert", "src/Shaders/SkyBox.frag");

    Model cube = Model("Models/Cube/Cube.fbx");
    Model sphere = Model("Models/Sphere/Sphere.fbx");

    LightGroup lightGroup = LightGroup(std::vector<glm::vec3>{glm::vec3(5),
        glm::vec3(0, 5, 0)}, std::vector<glm::vec3>{glm::vec3(100), glm::vec3(100)});

    Camera camera = Camera();

    HDR houseHDR = HDR("HDRs/house.png", equirectangularToCubeMapShader, convoluteHDRShader, preFilterShader, BRDFIntegrationShader);
    SkyBox skyBox = SkyBox(houseHDR.cubeMap);

    camera.Translate(glm::vec3(0, 0, 8));
    sphere.RotateGlobal(90, glm::vec3(1, 0, 0));
    cube.Translate(glm::vec3(2, 0, 0));
    float lastTime = 0;
    
    sphere.SetShader(PBRShader);
    sphere.SetLightGroup(lightGroup);
    sphere.SetIrradianceMap(houseHDR.irradianceMap);
    sphere.SetPreFilterMap(houseHDR.preFilterMap);
    sphere.SetBRDFIntegrationMap(houseHDR.BRDFIntegrationMap);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.ProcessInput(window, deltaTime);
        
        skyBox.Draw(skyBoxShader, cube, camera);

        sphere.SetCamera(camera);
        sphere.Draw();
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

