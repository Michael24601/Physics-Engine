
#include "simulations.h"

using namespace pe;


void pe::runBallPit() {

    GlfwWindowWrapper window(1200, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 1000.0f, 1000.0f),
        glm::vec3(0.0f, 300.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.00003,
        0.0001
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

    const int MAX_SPHERES = 50;
    int numSpheres = 0;

    // Shaders
    CookTorranceShader sphereShader;
    std::vector<CookTorranceShader> cubeShaders(13);

    const real mass = 0;


    std::vector<SolidSphere*> spheres;
    std::vector<RectangularPrism*> walls;


    walls.push_back(new RectangularPrism(
        100, 400, 1200, mass, Vector3D(-550, 250, 0), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        100, 400, 1200, mass, Vector3D(550, 250, 0), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        1200, 400, 100, mass, Vector3D(0, 250, 550), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        1200, 400, 100, mass, Vector3D(0, 250, -550), new RigidBody
    ));

    walls.push_back(new RectangularPrism(
        400, 400, 400, mass, Vector3D(400, 250, 400), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400, 400, 400, mass, Vector3D(400, 250, -400), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400, 400, 400, mass, Vector3D(-400, 250, 400), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400, 400, 400, mass, Vector3D(-400, 250, -400), new RigidBody
    ));

    walls.push_back(new RectangularPrism(
        400 * 1.45, 10, 400, mass, Vector3D(325, 210, 0), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400, 10, 400 * 1.5, mass, Vector3D(0, 210, -325), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400, 10, 400 * 1.5, mass, Vector3D(0, 210, 325), new RigidBody
    ));
    walls.push_back(new RectangularPrism(
        400 * 1.5, 10, 400, mass, Vector3D(-325, 210, -0), new RigidBody
    ));

    walls.push_back(new RectangularPrism(
        1200, 50, 1200, mass, Vector3D(0, 25, 0), new RigidBody
    ));

    walls[8]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, -PI / 3.4);
    walls[9]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, -PI / 3.4);
    walls[10]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, PI / 3.4);
    walls[11]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, PI / 3.4);


    for (RectangularPrism* prism : walls) {
        prism->body->calculateDerivedData();
    }


    for (int i = 0; i < walls.size(); i++) {

        walls[i]->body->inverseMass = 0;
        walls[i]->body->canSleep = true;

        FaceData data = getFaceData(*walls[i]);
        std::vector<std::vector<glm::vec3>> vertices{
            data.vertices, data.normals
        };

        cubeShaders[i].sendVaribleData(vertices, GL_STATIC_DRAW);
        cubeShaders[i].setTrianglesNumber(data.vertices.size());
        cubeShaders[i].setLightPosition(lightPos, 1);
        cubeShaders[i].setLightColors(lightColors, 1);
        cubeShaders[i].setFresnel(0.05);
        cubeShaders[i].setRoughness(0.5);
        cubeShaders[i].setObjectColor(colorWhite);
    }

    // Sphere

    SolidSphere sphere(70, 5, 20, 20, Vector3D(0, 0, 0), new RigidBody);

    FaceData data = getFaceData(sphere);
    std::vector<std::vector<glm::vec3>>vertices = {
        data.vertices, data.normals
    };
    sphereShader.sendVaribleData(vertices, GL_STATIC_DRAW);
    sphereShader.setTrianglesNumber(data.vertices.size());
    sphereShader.setLightPosition(lightPos, 1);
    sphereShader.setLightColors(lightColors, 1);
    sphereShader.setFresnel(0.1);
    sphereShader.setRoughness(0.05);
    sphereShader.setObjectColor(colorBlue);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    float deltaT = 0.0008;

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

            if (isPressed) {

                int r = generateRandomNumber(0, 4);
                int l1 = 0, g1 = 0, l2 = 0, g2 = 0;
                switch (r) {
                case 0: l1 = -500; g1 = -200; l2 = -200; g2 = 200; break;
                case 1: l1 = -200; g1 = 200; l2 = -200; g2 = 200; break;
                case 2: l1 = 200; g1 = 500; l2 = -200; g2 = 200; break;
                case 3: l1 = -200; g1 = 200; l2 = -500; g2 = -200; break;
                case 4: l1 = -200; g1 = 200; l2 = 200; g2 = 500; break;
                }

                int n = generateRandomNumber(l1, g1);
                int m = generateRandomNumber(l2, g2);
                // We can generate only 1 vertex for
                // these spheres as they will be shaded later
                SolidSphere* v = new SolidSphere(
                    70, 0.1, 1, 1, Vector3D(n, 1000, m), new RigidBody
                );
                v->body->angularDamping = 0.3;
                v->body->linearDamping = 0.95;
                v->body->canSleep = true;
                spheres.push_back(v);
            }

            isPressed = false;
        }


        int numSteps = 1;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (SolidSphere* s : spheres) {
                s->body->calculateDerivedData();
            }

            for (SolidSphere* s : spheres) {
                g.updateForce(s->body, substep);
            }


            std::vector<Contact> contacts;
            for (int i = 0; i < walls.size(); i++) {
                for (SolidSphere* s : spheres) {
                    generateContactBoxAndSphere(*(walls[i]), *s, contacts, 1.0, 0.5);
                }
            }
            for (SolidSphere* s : spheres) {
                for (SolidSphere* s2 : spheres) {
                    if (
                        (s2->getCentre() - s->getCentre()).magnitudeSquared() <
                        (s2->radius + s->radius) * (s2->radius + s->radius)
                        ) {
                        generateContactSphereAndSphere(*s2, *s, contacts, 0.5, 0.0);
                    }
                }
            }


            CollisionResolver resolver(1, 0);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            for (SolidSphere* s : spheres) {
                s->body->integrate(substep);
            }
        }


        for (int i = 0; i < walls.size(); i++) {
            cubeShaders[i].setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i].setViewMatrix(camera.getViewMatrix());
            cubeShaders[i].setModelMatrix(convertToGLM(walls[i]->getTransformMatrix()));
        }

        sphereShader.setProjectionMatrix(camera.getProjectionMatrix());
        sphereShader.setViewMatrix(camera.getViewMatrix());


        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < walls.size(); i++) {
                cubeShaders[i].drawFaces();
            }
            for (SolidSphere* s : spheres) {
                sphereShader.setModelMatrix(convertToGLM(s->getTransformMatrix()));
                sphereShader.drawFaces();
            }


            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}