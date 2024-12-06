
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "cookTorranceReflectionShader.h"
#include "diffuseLightingShader.h"
#include "polyhedra.h"
#include "environmentMapper.h"

using namespace pe;

void pe::runMirrors() {

    GlfwWindowWrapper window(800, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 100.0f, 500.0f),
        glm::vec3(0.0f, 100.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.01,
        0.01
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorPurple(0.7, 0.1, 0.8, 1);
    glm::vec4 colorBlack(0.2, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
    glm::vec4 colorGrey(0.35, 0.35, 0.35, 1);
    glm::vec4 colorTrans(0, 0, 0, 1.0);


    glm::vec3 lightPos[]{ glm::vec3(0, 500, 0), glm::vec3(-500, 0, -500) };
    glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0) };


    // Shaders
    CookTorranceReflectionShader refShader;
    refShader.setLightPosition(lightPos, 1);
    refShader.setLightColors(lightColor, 1);
    refShader.setFresnel(0.5);
    refShader.setRoughness(0.05);
    refShader.setLightInfluence(0.0);
    refShader.setReflectionStrength(1.0);

    DiffuseLightingShader cubeShader;
    cubeShader.setLightPosition(lightPos, 1);
    cubeShader.setLightColors(lightColor, 1);

    // Objects
    std::vector<CuboidObject*> prisms{
        // mirrors
        new CuboidObject(1, 200, 100, Vector3D(100, 103, 0), Quaternion::IDENTITY, 0),
        new CuboidObject(1, 200, 100, Vector3D(-100, 103, 0), Quaternion::IDENTITY, 0),
        // other blocks
        new CuboidObject(5000, 10, 5000, Vector3D(0, -5, 0), Quaternion::IDENTITY, 0),
        new CuboidObject(5, 206, 106, Vector3D(103, 103, 0),Quaternion::IDENTITY, 0),
        new CuboidObject(5, 206, 106, Vector3D(-103, 103, 0),Quaternion::IDENTITY, 0),
        new CuboidObject(20, 20, 20, Vector3D(0, 0, 0),Quaternion::IDENTITY, 0)
    };

    for (int i = 0; i < 6; i++) {
        if (i < 2) {
            prisms[i]->faceRenderer.setShader(&refShader);
            prisms[i]->faceRenderer.setColor(colorWhite);
        }
        else {
            prisms[i]->faceRenderer.setShader(&cubeShader);
            if(i == 2)
                prisms[i]->faceRenderer.setColor(colorWhite);
            else if (i < 5)
                prisms[i]->faceRenderer.setColor(colorBlack);
            else
                prisms[i]->faceRenderer.setColor(colorRed);
        }
    }

    // For the first and second mirrors
    EnvironmentMapper mapper0(512, 512, 0.1f, 500.0f, 105.0f, 1);
    EnvironmentMapper mapper1(512, 512, 0.1f, 500.0f, 105.0f, 2);

    float deltaT = 0.07;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    glfwSetFramebufferSizeCallback(
        window.getWindow(),
        window.framebuffer_size_callback
    );
    while (!glfwWindowShouldClose(window.getWindow())) {

        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        window.processInput();
        camera.processInput(deltaT);

        // Moves block with mouse
        glm::vec2 pos = window.getCursorPosition();
        prisms[5]->body.position = Vector3D(-pos.y, 80, -pos.x);

        // Updates the transform matrices
        for (int i = 0; i < 6; i++) {
            prisms[i]->update();
            prisms[i]->updateModelMatrix();
        }

        // Clears the environment map
        prisms[0]->faceRenderer.setEnvironmentMap(0);
        prisms[1]->faceRenderer.setEnvironmentMap(0);

        std::vector<RenderComponent*> renderComponents0;
        std::vector<RenderComponent*> renderComponents1;
        for (int i = 0; i < 6; i++) {
            if (i != 0) 
                renderComponents0.push_back(&prisms[i]->faceRenderer);
            if (i != 1)
                renderComponents1.push_back(&prisms[i]->faceRenderer);
        }

        // Captures environment recursively to create infinite mirror effect
        for (int i = 0; i < 10; i++) {
            mapper0.captureEnvironment(
                convertToGLM(prisms[0]->body.position), renderComponents0
            );
            /*
                We then capture the second environment, including the
                first mirror with its newly added reflection.
            */
            mapper1.captureEnvironment(
                convertToGLM(prisms[1]->body.position), renderComponents1
            );

            /*
                We then set the capture environment in the first and second
                mirrors. The next iteration, the set environment map
                will appear in the next environment map, creating the infinite
                mirror effect.
            */
            prisms[0]->faceRenderer.setEnvironmentMap(mapper0.getTexture());
            prisms[1]->faceRenderer.setEnvironmentMap(mapper1.getTexture());
        }

        /*
            The environment mapper resets the projection and view matrices so
            we need to reset them here.
        */
        refShader.setProjectionMatrix(camera.getProjectionMatrix());
        refShader.setViewMatrix(camera.getViewMatrix());
        cubeShader.setProjectionMatrix(camera.getProjectionMatrix());
        cubeShader.setViewMatrix(camera.getViewMatrix());

        // Clears default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < 6; i++) {
                prisms[i]->faceRenderer.render();
            }

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}
