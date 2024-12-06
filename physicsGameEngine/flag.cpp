
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "cookTorranceReflectionShader.h"
#include "diffuseLightingShader.h"
#include "polyhedra.h"
#include "environmentMapper.h"
#include "cloth.h"
#include "particleGravity.h"
#include "skyboxRenderer.h"

using namespace pe;


void pe::runFlag() {

    GlfwWindowWrapper window(800, 800, 6, "window", false);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0, 0, 500),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.0005,
        0.001
    );

    // Lighting
    glm::vec3 lightPos[]{
        glm::vec3(-500 ,0 , 500),
        glm::vec3(500, 0, 500)
    };
    glm::vec4 lightColor[]{
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0)
    };


    // Shaders
    DiffuseLightingShader shader;
    shader.setLightColors(lightColor, 1);
    shader.setLightPosition(lightPos, 1);
    shader.setProjectionMatrix(camera.getProjectionMatrix());

    GLuint textureFlag = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\leb.png"
    );
    GLuint textureWood = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\wood.jpg"
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

    const real height = 400;

    int size = 20;
    real structuralStiffness = 5;
    real shearStiffness = 2;
    real bendingStiffness = 2;
    real mass = 0.5;
    real damping = 0.9;
    real dampingCoefficient = 0.005;

    int laplacianIterations = 1;
    real laplacianFactor = 0.05;

    Cloth cloth(
        std::make_pair(size, size),
        std::make_pair(400, 600),
        std::make_pair(Vector3D(0, -1, 0), Vector3D(1, 0, 0)),
        Vector3D(-300, 200, 0),
        mass, damping, dampingCoefficient,
        structuralStiffness, shearStiffness, bendingStiffness
    );
    cloth.faceRenderer.setTexture(textureFlag);
    cloth.faceRenderer.setShader(&shader);

    CylinderObject cylinder(
        10, 800, 10, Vector3D(-305, -200, 0), Quaternion::IDENTITY, 0
    );
    cylinder.faceRenderer.setTexture(textureWood);
    cylinder.faceRenderer.setShader(&shader);

    // The first row of particles is suspended
    for (int i = 0; i < size * size; i++) {
        if (i % size == 0) {
            cloth.body.particles[i].setAwake(false);
        }
    }

    VertexBuffer buffer = createFaceVertexBuffer(
        &cloth.mesh, GL_DYNAMIC_DRAW,
        NORMALS::VERTEX_NORMALS, UV::INCLUDE
    );
    cloth.faceRenderer.setVertexBuffer(&buffer);

    ParticleGravity g(Vector3D(0, -10, 0));

    float deltaT = 0.2;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 30;
    float frameRate = 1.0 / framesPerSecond;

    real windMultiplier = 1;

    glfwSetFramebufferSizeCallback(
        window.getWindow(),
        window.framebuffer_size_callback
    );
    while (!glfwWindowShouldClose(window.getWindow())) {

        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        glfwPollEvents();
        window.processInput();
        camera.processInput(frameRate);

        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec2 mouse = window.getCursorPosition();
            Vector3D pos = Vector3D(
                mouse.x,
                mouse.y,
                cylinder.body.position.z
            );
            cylinder.body.position = pos;
            for (int i = 0; i < size; i++) {
                cloth.body.particles[i * size].position =
                    pos + Vector3D(0, ((size - i) * height / size), 0);
            }
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_B) == GLFW_PRESS) {
            glm::vec2 mouse = window.getCursorPosition();
            Vector3D pos = Vector3D(
                mouse.x,
                cylinder.body.position.y,
                -mouse.y
            );
            for (int i = 0; i < size; i++) {
                cloth.body.particles[(i+1) * size - 1].position = 
                    pos + Vector3D(0, ((size - i) * height / size), 0);
            }
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_X) == GLFW_PRESS) {
            windMultiplier *= 1.0002;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_Z) == GLFW_PRESS) {
            windMultiplier *= 0.9998;
        }

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            cloth.body.applyForce(g, substep);
            cloth.body.applySpringForces(substep);

            cloth.applyWindForce(Vector3D(3, 13, 4) * windMultiplier, substep);

            cloth.body.verletIntegrate(substep);
            cloth.applyLaplacianSmoothing(laplacianIterations, laplacianFactor);
        }

        cylinder.update();
        cylinder.updateModelMatrix();
        cloth.update();

        buffer.setData(generateFaceData(
            &cloth.mesh, NORMALS::VERTEX_NORMALS, UV::INCLUDE
        ));

        shader.setViewMatrix(camera.getViewMatrix());

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // We set the texture here cause they use the same unit
            cloth.faceRenderer.render();
            cylinder.faceRenderer.render();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}
