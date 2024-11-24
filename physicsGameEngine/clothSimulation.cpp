
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "cookTorranceShader.h"
#include "solidColorShader.h"
#include "polyhedra.h"
#include "particleGravity.h"
#include "diffuseLightingShader.h"
#include "cloth.h"
#include "faceBufferGenerator.h"

using namespace pe;


void pe::runClothSimulation() {

    GlfwWindowWrapper window(800, 800, 6, "window", false);

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

    glm::vec3 lightPos[]{ 
        glm::vec3(500, 0, 500) 
    };
    glm::vec4 lightColor[]{
        glm::vec4(1.0, 1.0, 1.0, 1.0),
    };

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 500.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.002,
        0.01
    );

    int size = 25;
    real structuralStiffness = 6;
    real shearStiffness = 3;
    real bendingStiffness = 3;
    real mass = 0.5;
    real damping = 0.9;
    real dampingCoefficient = 0.003;

    int laplacianIterations = 1;
    real laplacianFactor = 0.05;

    Cloth cloth(
        std::make_pair(size, size),
        std::make_pair(400, 400),
        std::make_pair(Vector3D(0, -1, 0), Vector3D(1, 0, 0)),
        Vector3D(-200, 200, 0),
        mass, damping, dampingCoefficient,
        structuralStiffness, shearStiffness, bendingStiffness
    );

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        cloth.body.particles[i].setAwake(false);
    }

    VertexBuffer buffer = createFaceVertexBuffer(
        &cloth.mesh, GL_DYNAMIC_DRAW,
        NORMALS::VERTEX_NORMALS, UV::INCLUDE
    );

    ParticleGravity g(Vector3D(0, -10, 0));

    // Shaders
    DiffuseLightingShader shader;
    shader.setLightPosition(lightPos, 1);
    shader.setLightColors(lightColor, 1);
    shader.setObjectColor(colorRed);

    bool isButtonPressed = false;

    float deltaT = 0.2;

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
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            isButtonPressed = true;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            isButtonPressed = false;
        }


        glm::vec2 worldPos = window.getCursorPosition();

        if (isButtonPressed) {
            glm::vec2 worldPos = window.getCursorPosition();
            Vector3D move(
                worldPos.x - cloth.body.particles[size * size / 2].position.x,
                worldPos.y - cloth.body.particles[size * size / 2].position.y,
                worldPos.x - cloth.body.particles[size * size / 2].position.z
            );

            move *= 2;
            ParticleGravity f(move);
            for (int i = size/2; i < size; i++) {
                f.updateForce(&cloth.body.particles[size + i * size - 1], deltaT);
            }
        }

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            cloth.body.applyForce(g, substep);
            cloth.body.applySpringForces(substep);
            cloth.body.verletIntegrate(substep);

            cloth.applyLaplacianSmoothing(laplacianIterations, laplacianFactor);
        }

        cloth.update();
        buffer.setData(generateFaceData(
            &cloth.mesh, NORMALS::VERTEX_NORMALS, UV::INCLUDE
        ));

        shader.setViewMatrix(camera.getViewMatrix());
        shader.setProjectionMatrix(camera.getProjectionMatrix());
        shader.setModelMatrix(glm::mat4(1.0));

        // Clears default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.render(buffer);

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}