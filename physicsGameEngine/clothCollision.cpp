
#ifdef DONE_REFACTOR

#include "simulations.h"

using namespace pe;


void pe::runClothCollision() {

    GlfwWindowWrapper window(800, 800, 6, "window", false);

    // Shaders
    DiffuseLightingShader shader;
    DiffuseLightingShader clothShader;

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

    glm::vec3 lightPos[]{ 
        glm::vec3(500, 0, 500), 
        glm::vec3(-500, 0, -500) 
    };
    glm::vec4 lightColor[]{
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0) 
    };

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 500.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.02,
        0.1
    );

    int size = 30;
    real strength = 0.5;
    real mass = 0.3;
    real damping = 0.5;
    real dk = 0.2;
    int laplacianIterations = 1;
    real laplacianFactor = 0.1;

    Cloth mesh(
        Vector3D(-200, 200, 0),
        Vector3D(200, -200, 0),
        size, size,
        mass, damping, strength, dk
    );

    SolidSphere c(100, 100, 20, 20, Vector3D(0, 0, 400), new RigidBody);

    FaceData data = getFaceData(c);
    std::vector<std::vector<glm::vec3>> d{
        data.vertices, data.normals
    };
    shader.sendVaribleData(d, GL_STATIC_DRAW);
    shader.setTrianglesNumber(data.vertices.size());
    shader.setObjectColor(colorBlue);
    shader.setLightPosition(lightPos, 1);
    shader.setProjectionMatrix(camera.getProjectionMatrix());

    clothShader.setObjectColor(colorRed);
    clothShader.setLightPosition(lightPos, 1);
    clothShader.setModelMatrix(identity);
    clothShader.setProjectionMatrix(camera.getProjectionMatrix());

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        mesh.particles[i]->setAwake(false);
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    bool isButtonPressed[]{ false, false };

    float deltaT = 0.05;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 120;
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
            isButtonPressed[0] = true;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            isButtonPressed[0] = false;
        }


        glm::vec2 worldPos = window.getCursorPosition();

        if (isButtonPressed[0] || isButtonPressed[1]) {

            if (isButtonPressed[0]) {
                glm::vec2 worldPos = window.getCursorPosition();
                Vector3D move(
                    worldPos.x - mesh.particles[size * size / 2]->position.x,
                    worldPos.y - mesh.particles[size * size / 2]->position.y,
                    worldPos.x - mesh.particles[size * size / 2]->position.z
                );

                move *= mass;
                ParticleGravity f(move);

                for (int i = 0; i < size; i++) {
                    f.updateForce(mesh.particles[size + i * size - 1], deltaT);
                }
            }
            else if (isButtonPressed[1]) {
                glm::vec2 worldPos = window.getCursorPosition();
                Vector3D move(
                    worldPos.x - mesh.particles[size * size / 2]->position.x,
                    worldPos.y - mesh.particles[size * size / 2]->position.y,
                    -(worldPos.x + mesh.particles[size * size / 2]->position.z)
                );

                move *= mass;
                ParticleGravity f(move);

                for (int i = 0; i < size; i++) {
                    f.updateForce(mesh.particles[i * size], deltaT);
                }
            }
        }

        c.body->position = {
            c.body->position.x,
            worldPos.y,
            worldPos.x
        };

        int numSteps = 20;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            c.body->calculateDerivedData();

            for (auto& particle : mesh.particles) {
                g.updateForce(particle, deltaT);
            }

            for (auto& force : mesh.forces) {
                force.force1.updateForce(force.force2.otherParticle, deltaT);
                force.force2.updateForce(force.force1.otherParticle, deltaT);
            }

            ParticleContactResolver r;
            std::vector<ParticleContact> contacts;
            for (Particle* particle : mesh.particles) {
                generateContactParticleAndSphere(particle, c, contacts, 0.0);
            }
            r.setIterations(contacts.size() * 1);
            if (contacts.size()) {
                r.resolveContacts(contacts.data(), contacts.size(), substep);
            }

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i]->isAwake) {
                    mesh.particles[i]->verletIntegrate(deltaT);
                }
            }

            mesh.laplacianSmoothing(laplacianIterations, laplacianFactor);
            mesh.applyConstraints();

            c.body->integrate(substep);
            mesh.update();
        }

        FaceData data = getFaceData(mesh);
        std::vector<std::vector<glm::vec3>> d{
            data.vertices, data.normals
        };
        clothShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
        clothShader.setTrianglesNumber(data.vertices.size());
        clothShader.setViewMatrix(camera.getViewMatrix());

        shader.setViewMatrix(camera.getViewMatrix());
        shader.setModelMatrix(convertToGLM(c.getTransformMatrix()));


        // Clears default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            clothShader.drawFaces();
            shader.drawFaces();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}

#endif