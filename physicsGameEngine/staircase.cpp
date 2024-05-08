

#include "simulations.h"

using namespace pe;


void pe::runStaircase() {

    GlfwWindowWrapper window(1200, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 2000.0f, 1000.0f),
        glm::vec3(0.0f, 1800.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.0001,
        0.00025
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
        glm::vec3(200.0f, 1800.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    // Shaders
    std::vector<CookTorranceShader> cubeShaders(36);
    CookTorranceShader sphereShader;
    ShadowMappingShader groundShader;
    SolidColorShader lineShader;

    const real mass = 0;

    // Sqrt(1^2 + 0.5^2)
    const real s = 1.118;

    std::vector<RectangularPrism*> prisms{
        new RectangularPrism(200, 100, 200 * s, mass, Vector3D(-400, 1556, 420), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(-400, 1500, 200), new RigidBody),
        new RectangularPrism(200, 100, 200 * s, mass, Vector3D(-400, 1456, 20), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(-400, 1400, -200), new RigidBody),

        new RectangularPrism(200 * s, 100, 200, mass, Vector3D(-420, 1356, -400), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(-200, 1300, -400), new RigidBody),
        new RectangularPrism(200 * s, 100, 200, mass, Vector3D(-20, 1256, -400), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(200, 1200, -400), new RigidBody),

        new RectangularPrism(200, 100, 200 * s, mass, Vector3D(400, 1156, -420), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(400, 1100, -200), new RigidBody),
        new RectangularPrism(200, 100, 200 * s, mass, Vector3D(400, 1056, -20), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(400, 1000, 200), new RigidBody),

        new RectangularPrism(200 * s, 100, 200, mass, Vector3D(420, 956, 400), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(200, 900, 400), new RigidBody),
        new RectangularPrism(200 * s, 100, 200, mass, Vector3D(20, 856, 400), new RigidBody),
        new RectangularPrism(200, 100, 200, mass, Vector3D(-200, 800, 400), new RigidBody),
    };

    for (int i = 0; i < 16; i++) {
        prisms.push_back(
            new RectangularPrism(
                prisms[i]->width,
                prisms[i]->height,
                prisms[i]->depth,
                mass,
                prisms[i]->getCentre() - Vector3D(0, 800, 0),
                new RigidBody
            )
        );
    }

    prisms.push_back(new RectangularPrism(
        100, 1600, 1200, mass, Vector3D(-550, 800, 0), new RigidBody
    ));
    prisms.push_back(new RectangularPrism(
        100, 1600, 1200, mass, Vector3D(550, 800, 0), new RigidBody
    ));
    prisms.push_back(new RectangularPrism(
        1000, 1600, 100, mass, Vector3D(0, 800, 550), new RigidBody
    ));
    prisms.push_back(new RectangularPrism(
        1000, 1600, 100, mass, Vector3D(0, 800, -550), new RigidBody
    ));

    for (int i = 0; i < prisms.size(); i++) {

        prisms[i]->body->inverseMass = 0;

        FaceData data = getFaceData(*prisms[i]);
        std::vector<std::vector<glm::vec3>> vertices{
            data.vertices, data.normals
        };

        cubeShaders[i].sendVaribleData(vertices, GL_STATIC_DRAW);
        cubeShaders[i].setTrianglesNumber(data.vertices.size());
        cubeShaders[i].setLightPosition(lightPos);
        cubeShaders[i].setLightColors(lightColors);
        cubeShaders[i].setFresnel(0.05);
        cubeShaders[i].setRoughness(0.5);
        cubeShaders[i].setObjectColor(colorWhite);
        cubeShaders[i].setActiveLightsCount(1);
    }

    for (int i = 0; i < 2; i++) {
        prisms[0 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, PI / 7);
        prisms[2 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, PI / 7);
        prisms[4 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, PI / 7);
        prisms[6 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, PI / 7);
        prisms[8 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, -PI / 7);
        prisms[10 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, -PI / 7);
        prisms[12 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, -PI / 7);
        prisms[14 + i * 16]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, -PI / 7);
    }

    // Sphere

    SolidSphere sphere(50, 5, 30, 30, Vector3D(-400, 1700, 400), new RigidBody);
    sphere.body->canSleep = false;
    sphere.body->angularDamping = 0.8;
    sphere.body->linearDamping = 0.95;

    FaceData data = getFaceData(sphere);
    std::vector<std::vector<glm::vec3>>vertices = {
        data.vertices, data.normals
    };
    sphereShader.sendVaribleData(vertices, GL_STATIC_DRAW);
    sphereShader.setTrianglesNumber(data.vertices.size());
    sphereShader.setLightPosition(lightPos);
    sphereShader.setLightColors(lightColors);
    sphereShader.setFresnel(0.1);
    sphereShader.setRoughness(0.05);
    sphereShader.setObjectColor(colorRed);
    sphereShader.setActiveLightsCount(1);

    lineShader.setObjectColor(colorWhite);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    float deltaT = 0.001;

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
        camera.processInput(frameRate);
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            isPressed = true;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            isPressed = false;
        }

        if (isPressed) {
            sphere.body->addForce(Vector3D(0, 0, -100));
        }

        int numSteps = 10;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (RectangularPrism* prism : prisms) {
                prism->body->calculateDerivedData();
            }
            sphere.body->calculateDerivedData();

            g.updateForce(sphere.body, substep);

            std::vector<Contact> contacts;

            for (int i = 0; i < prisms.size(); i++) {
                generateContactBoxAndSphere(*(prisms[i]), sphere, contacts, 0.25, 0.0);
            }

            CollisionResolver resolver(10, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            sphere.body->integrate(substep);
        }


        for (int i = 0; i < prisms.size(); i++) {
            cubeShaders[i].setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i].setViewMatrix(camera.getViewMatrix());
            cubeShaders[i].setModelMatrix(convertToGLM(prisms[i]->getTransformMatrix()));
        }
        sphereShader.setProjectionMatrix(camera.getProjectionMatrix());
        sphereShader.setViewMatrix(camera.getViewMatrix());
        sphereShader.setModelMatrix(convertToGLM(sphere.getTransformMatrix()));


        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < prisms.size(); i++) {
                cubeShaders[i].drawFaces();
            }
            sphereShader.drawFaces();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}