

#include "simulations.h"

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

    // Shaders
    DiffuseTextureShader poleShader;
    DiffuseTextureShader lightShader;

    GLuint texture1 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\leb.png"
    );
    GLuint texture3 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\wood.jpg"
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

    glm::vec3 lightPos[]{
        glm::vec3(-500 ,0 , 500),
        glm::vec3(500, 0, 500)
    };
    glm::vec4 lightColor[]{
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0)
    };

    int size = 18;
    real strength = 0.4;
    real mass = 0.45;
    real damping = 0.5;

    const real height = 400;
    const Vector3D topLeft(-200, height / 2.0, 0);

    Cloth mesh(
        topLeft,
        Vector3D(300, -height / 2.0, 0),
        size, size,
        mass, damping, strength
    );

    Cylinder cylinder(10, 800, 10, 20, Vector3D(-205, -200, 0), new RigidBody());

    lightShader.setLightPosition(lightPos, 1);
    poleShader.setLightPosition(lightPos, 1);

    FaceData data = getFaceData(cylinder);
    std::vector<std::vector<glm::vec3>> d{
           data.vertices, data.normals, data.uvCoordinates
    };
    poleShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
    poleShader.setTrianglesNumber(data.vertices.size());


    // The first row of particles is suspended
    for (int i = 0; i < size * size; i++) {
        if (i % size == 0) {
            mesh.particles[i]->setAwake(false);
        }
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    bool isPressed{ false };

    float deltaT = 0.13;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 30;
    float frameRate = 1.0 / framesPerSecond;

    real windMultiplier = 0.7;

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
            isPressed = true;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            isPressed = false;
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_X) == GLFW_PRESS) {
            windMultiplier *= 1.0002;
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_Z) == GLFW_PRESS) {
            windMultiplier *= 0.9998;
        }

        int numSteps = 2;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            if (isPressed) {

                glm::vec2 mouse = window.getCursorPosition();

                Vector3D pos = Vector3D(
                    mouse.x,
                    mouse.y,
                    cylinder.body->position.z
                );

                cylinder.body->position = pos;

                for (int i = 0; i < size; i++) {
                    mesh.particles[i * size]->position = pos +
                        Vector3D(0, ((size - i) * height / size), 0);
                }
            }

            cylinder.body->calculateDerivedData();

            for (Particle* particle : mesh.particles) {
                g.updateForce(particle, substep);
            }

            for (int i = 0; i < mesh.particles.size(); i++) {

                if (i < size) {
                    mesh.particles[i]->addForce(Vector3D(2, 7, 0) * windMultiplier);
                    continue;
                }
                else if (i >= (size - 1) * size) {
                    mesh.particles[i]->addForce(Vector3D(1, 4, 0) * windMultiplier);
                    continue;
                }

                int enter = generateRandomNumber(0, 5);
                if (enter <= 3) {
                    real x = generateRandomNumber(1.0f, 4.0f);
                    real y = generateRandomNumber(2.0f, 8.0f);
                    real z = generateRandomNumber(0.0f, 2.0f);
                    mesh.particles[i]->addForce(Vector3D(x, y, z) * windMultiplier);
                }
            }

            for (auto& force : mesh.forces) {
                force.force1.updateForce(force.force2.otherParticle, substep);
                force.force2.updateForce(force.force1.otherParticle, substep);
            }

            mesh.applyConstraints();

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i]->isAwake) {
                    mesh.particles[i]->verletIntegrate(substep);
                }
            }

            mesh.update();
        }

        FaceData data = getFaceData(mesh);
        std::vector<std::vector<glm::vec3>> d{
            data.vertices, data.normals, data.uvCoordinates
        };
        lightShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
        lightShader.setTrianglesNumber(data.vertices.size());
        lightShader.setModelMatrix(identity);
        lightShader.setViewMatrix(camera.getViewMatrix());
        lightShader.setProjectionMatrix(camera.getProjectionMatrix());

        poleShader.setModelMatrix(convertToGLM(cylinder.getTransformMatrix()));
        poleShader.setViewMatrix(camera.getViewMatrix());
        poleShader.setProjectionMatrix(camera.getProjectionMatrix());

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // We set the texture here cause they use the same unit
            lightShader.setObjectTexture(texture1);
            lightShader.drawFaces();
            poleShader.setObjectTexture(texture3);
            poleShader.drawFaces();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}