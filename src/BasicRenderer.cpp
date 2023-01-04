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
    Model blunderbuss = Model("Models/Blunderbuss/Blunderbuss.fbx", true);

    std::vector<glm::vec3> lightPositions = {
        glm::vec3(0, 0, 10), glm::vec3(0, 20, 0), glm::vec3(0, 0, -10)
    };
    std::vector<glm::vec3> lightColors = {
        glm::vec3(100), glm::vec3(100), glm::vec3(100)
    };

    LightGroup lightGroup = LightGroup(lightPositions, lightColors);

    Camera camera = Camera();

    HDR houseHDR = HDR("HDRs/house.png", equirectangularToCubeMapShader, convoluteHDRShader, preFilterShader, BRDFIntegrationShader);
    SkyBox skyBox = SkyBox(houseHDR.cubeMap);

    camera.Translate(glm::vec3(0, 0, 8));
    sphere.RotateGlobal(90, glm::vec3(1, 0, 0));
    blunderbuss.Scale(glm::vec3(0.05));
    blunderbuss.RotateGlobal(90, glm::vec3(0, 1, 0));


    float lastTime = 0;
    
    sphere.SetShader(PBRSimpleShader);
    sphere.SetLightGroup(lightGroup);
    sphere.SetIrradianceMap(houseHDR.irradianceMap);
    sphere.SetPreFilterMap(houseHDR.preFilterMap);
    sphere.SetBRDFIntegrationMap(houseHDR.BRDFIntegrationMap);
    sphere.SetHDRBrightness(2);

    blunderbuss.SetShader(PBRShader);
    blunderbuss.SetLightGroup(lightGroup);
    blunderbuss.SetIrradianceMap(houseHDR.irradianceMap);
    blunderbuss.SetPreFilterMap(houseHDR.preFilterMap);
    blunderbuss.SetBRDFIntegrationMap(houseHDR.BRDFIntegrationMap);
    blunderbuss.SetHDRBrightness(2);

    unsigned int scene = 0;
    bool mousePressed = false;


    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (!mousePressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            scene = (scene + 1) % 2;
            mousePressed = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) mousePressed = false;
        
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.ProcessInput(window, deltaTime);
        
        skyBox.Draw(skyBoxShader, cube, camera);

        blunderbuss.RotateGlobal(45 * deltaTime, glm::vec3(0, 1, 0));

        if (scene == 0) {
            sphere.SetCamera(camera);

            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    sphere.SetPosition(glm::vec3(x * 2.5, y * 2.5, 0));

                    float r = (glm::sin(currentTime) + 1) / 2;
                    float g = (glm::cos(currentTime) + 1) / 2;
                    float b = (y + 2.0) / 4;

                    float roughness = (x + 2.0) / 4;
                    float metallic = (y + 2.0) / 4;

                    sphere.Draw(glm::vec3(r, g, b), roughness, metallic, 0);
                }
            }
        }
        else if (scene == 1) {
            blunderbuss.SetCamera(camera);
            blunderbuss.Draw();
        }
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

