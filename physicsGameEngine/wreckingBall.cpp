
#include "simulations.h"

using namespace pe;

void pe::runWreckingBall() {

    GlfwWindowWrapper window(1200, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 200.0f, 500.0f),
        glm::vec3(0.0f, 200.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.0002,
        0.001
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
    glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
    glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
    glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
    glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
    glm::vec4 colorMagenta(0.9, 0.3, 0.6, 1.0);
    glm::vec4 colorOrange(0.9, 0.6, 0.2, 1.0);
    glm::vec4 colorDarkBlue(0.1, 0.1, 0.4, 1.0);
    glm::vec4 colorGrey(0.4, 0.4, 0.4, 1.0);
    glm::vec4 colorBlack(0.05, 0.05, 0.05, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(200.0f, 2000.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    // Shaders
    std::vector<CookTorranceShader> cubeShaders(27);
    CookTorranceShader sphereShader;
    DiffuseLightingShader groundShader;
    SolidColorShader lineShader;

    real mass = 1.5;

    std::vector<RectangularPrism*> prisms;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {

                int index = i * 9 + j * 3 + k;

                real x = i * 200 - 400;
                real y = j * 200 - 200;
                real z = k * 200 - 400;

                RectangularPrism* prism = new RectangularPrism(
                    200, 200, 200, mass, Vector3D(x, y, z), new RigidBody
                );
                prism->body->angularDamping = 0.8;
                prism->body->linearDamping = 0.95;
                prism->body->canSleep = true;

                prisms.push_back(prism);

                FaceData data = getFaceData(*prism);
                std::vector<std::vector<glm::vec3>> vertices{
                    data.vertices, data.normals
                };

                cubeShaders[index].sendVaribleData(vertices, GL_STATIC_DRAW);
                cubeShaders[index].setTrianglesNumber(data.vertices.size());
                cubeShaders[index].setLightPosition(lightPos, 1);
                cubeShaders[index].setLightColors(lightColors, 1);
                cubeShaders[index].setFresnel(0.05);
                cubeShaders[index].setRoughness(0.5);
                cubeShaders[index].setObjectColor(colorPurple);
            }
        }
    }

    // Ground

    RectangularPrism ground(5000, 100, 5000, 0, Vector3D(0, -350, -0), new RigidBody);
    ground.body->inverseMass = 0;

    FaceData data = getFaceData(ground);
    std::vector<std::vector<glm::vec3>> vertices{
        data.vertices, data.normals
    };
    groundShader.sendVaribleData(vertices, GL_STATIC_DRAW);
    groundShader.setTrianglesNumber(data.vertices.size());
    groundShader.setLightPosition(lightPos, 1);
    groundShader.setObjectColor(colorWhite);

    // Sphere

    SolidSphere sphere(200, 2, 20, 20, Vector3D(800, 400, 0), new RigidBody);
    sphere.body->canSleep = false;
    sphere.body->angularDamping = 0.8;
    sphere.body->linearDamping = 0.95;

    data = getFaceData(sphere);
    vertices = {
        data.vertices, data.normals
    };
    sphereShader.sendVaribleData(vertices, GL_STATIC_DRAW);
    sphereShader.setTrianglesNumber(data.vertices.size());
    sphereShader.setLightPosition(lightPos, 1);
    sphereShader.setLightColors(lightColors, 1);
    sphereShader.setFresnel(0.4);
    sphereShader.setRoughness(0.05);
    sphereShader.setObjectColor(colorRed);

    lineShader.setObjectColor(colorWhite);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    RigidBody b;
    b.position = Vector3D(800, 700, 0);
    RigidBodySpringForce f(sphere.localVertices[0], &b, Vector3D(), 0.09, 300);

    float deltaT = 0.0038;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    bool isPressed = false;

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
        camera.processInput(deltaT);
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            isPressed = true;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            isPressed = false;
        }

        if (isPressed) {
            glm::vec2 worldPos = window.getCursorPosition();
            sphere.body->position.x = worldPos.x * 4;
            sphere.body->position.y = worldPos.y * 4;
            sphere.body->setAwake(true);
        }

        int numSteps = 2;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (RectangularPrism* prism : prisms) {
                prism->body->calculateDerivedData();
            }
            ground.body->calculateDerivedData();
            sphere.body->calculateDerivedData();
            b.calculateDerivedData();

            for (RectangularPrism* prism : prisms) {
                g.updateForce(prism->body, substep);
            }
            g.updateForce(sphere.body, substep);
            f.updateForce(sphere.body, substep);

            std::vector<Contact> contacts;

            for (int i = 0; i < prisms.size(); i++) {
                generateContactBoxAndBox(*(prisms[i]), ground, contacts, 0.25, 0.0);
                generateContactBoxAndSphere(*(prisms[i]), sphere, contacts, 0.25, 0.0);
                for (int j = 0; j < prisms.size(); j++) {
                    if (i != j && (
                        (prisms[i]->body->position - prisms[j]->body->position)
                        .magnitudeSquared() < 350*350
                    )) {
                        generateContactBoxAndBox(*(prisms[i]), *(prisms[j]), contacts, 0.25, 0.0);
                    }
                }
            }
            generateContactBoxAndSphere(ground, sphere, contacts, 0.25, 0.0);

            CollisionResolver resolver(10, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            for (RectangularPrism* prism : prisms) {
                prism->body->integrate(substep);
            }
            ground.body->integrate(substep);
            sphere.body->integrate(substep);
        }


        for (int i = 0; i < prisms.size(); i++) {
            cubeShaders[i].setModelMatrix(convertToGLM(prisms[i]->getTransformMatrix()));
        }
        groundShader.setModelMatrix(convertToGLM(ground.getTransformMatrix()));
        sphereShader.setModelMatrix(convertToGLM(sphere.getTransformMatrix()));


        for (int i = 0; i < prisms.size(); i++) {
            cubeShaders[i].setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i].setViewMatrix(camera.getViewMatrix());
        }

        groundShader.setProjectionMatrix(camera.getProjectionMatrix());
        groundShader.setViewMatrix(camera.getViewMatrix());

        sphereShader.setProjectionMatrix(camera.getProjectionMatrix());
        sphereShader.setViewMatrix(camera.getViewMatrix());

        std::vector<std::vector<glm::vec3>> spring{ {
            convertToGLM(b.position),
            convertToGLM(sphere.getTransformMatrix().transform(sphere.localVertices[0]))
        } };
        lineShader.sendVaribleData(spring, GL_DYNAMIC_DRAW);
        lineShader.setEdgeNumber(2);
        lineShader.setModelMatrix(identity);
        lineShader.setProjectionMatrix(camera.getProjectionMatrix());
        lineShader.setViewMatrix(camera.getViewMatrix());

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < prisms.size(); i++) {
                cubeShaders[i].drawFaces();
            }
            groundShader.drawFaces();
            sphereShader.drawFaces();
            lineShader.drawEdges();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}