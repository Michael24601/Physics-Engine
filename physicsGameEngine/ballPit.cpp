
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "cookTorranceShader.h"
#include "solidColorShader.h"
#include "polyhedra.h"
#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "fineCollisionDetection.h"
#include "collisionResolver.h"
#include "boundingVolumeHierarchy.h"
#include "faceBufferGenerator.h"

using namespace pe;

void pe::runBallPit() {

    std::cout << "Use arrow keys to move the camera\n";
    std::cout << "Press the A key to add a ball\n";

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


    // Shaders
    CookTorranceShader shader;
    shader.setLightPosition(lightPos, 1);
    shader.setLightColors(lightColors, 1);
    shader.setFresnel(0.5);
    shader.setRoughness(0.05);
    shader.setProjectionMatrix(camera.getProjectionMatrix());


    const int MAX_SPHERES = 50;
    int numSpheres = 0;

    // Objects
    std::vector<CuboidObject*> walls{
        // Walls
        new CuboidObject(
            100, 400, 1200, Vector3D(-550, 250, 0), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            100, 400, 1200, Vector3D(550, 250, 0), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            1200, 400, 100, Vector3D(0, 250, 550), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            1200, 400, 100, Vector3D(0, 250, -550), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            400, 400, 400, Vector3D(400, 250, 400), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            400, 400, 400, Vector3D(400, 250, -400), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            400, 400, 400, Vector3D(-400, 250, 400), Quaternion::IDENTITY, 0
        ),
        new CuboidObject(
            400, 400, 400, Vector3D(-400, 250, -400), Quaternion::IDENTITY, 0
        ),
        // Planks
        new CuboidObject(
            400 * 1.45, 10, 400, Vector3D(325, 210, 0),
            Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, -PI / 3.4), 0
        ),
        new CuboidObject(
            400, 10, 400 * 1.45, Vector3D(0, 210, -325),
            Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, -PI / 3.4), 0
        ),
        new CuboidObject(
            400, 10, 400 * 1.45, Vector3D(0, 210, 325),
            Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, PI / 3.4), 0
        ),
        new CuboidObject(
            400 * 1.45, 10, 400, Vector3D(-325, 210, -0),
            Quaternion::rotatedByAxisAngle(Vector3D::RIGHT, PI / 3.4), 0
        ),
        // Ground
        new CuboidObject(
            1200, 50, 1200, Vector3D(0, 25, 0), Quaternion::IDENTITY, 0
        )
    };

    for (CuboidObject* o : walls) {
        o->body.inverseMass = 0;
        o->body.isAwake = false;
        o->faceRenderer.setColor(colorWhite);
        o->faceRenderer.setShader(&shader);
    }

    // To be filled in real time
    std::vector<SphereObject*> spheres;

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

            // Generates a sphere on each keypress
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
                SphereObject* s = new SphereObject(
                    70, 20, 20, Vector3D(n, 1000, m), Quaternion::IDENTITY, 20
                );
                s->body.angularDamping = 0.3;
                s->body.linearDamping = 0.95;
                s->faceRenderer.setColor(colorBlue);
                s->faceRenderer.setShader(&shader);
                spheres.push_back(s);
            }

            isPressed = false;
        }


        int numSteps = 1;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (SphereObject* s : spheres) {
                g.updateForce(&s->body, substep);
            }

            BoundingVolumeHierarchy BVH;
            for (SphereObject* o : spheres) {
                BVH.insert(
                    o,
                    o->boundingVolumeTransform.getTranslation(),
                    o->boundingVolume->getBVHSphereRadius()
                );
            }
            for (CuboidObject* o : walls) {
                BVH.insert(
                    o,
                    o->boundingVolumeTransform.getTranslation(),
                    o->boundingVolume->getBVHSphereRadius()
                );
            }

            PotentialContact con[1000];
            int size = BVH.getPotentialContacts(con, 1000);

            // Fine collision detection/resolution

            std::vector<Contact> contacts;
            for (int i = 0; i < size; i++) {
                // We only do the expensive fine collision detection phase
                // if at least one body is awake (moving), otherwise it serves
                // no purpose and wastes time.
                if (con[i].object[0]->body.isAwake || con[i].object[1]->body.isAwake) {
                    generateContacts(
                        *con[i].object[0], *con[i].object[1], contacts, 0.6, 0.0
                    );
                }
            }

            CollisionResolver resolver(1, 0);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            for (SphereObject* s : spheres) {
                s->body.integrate(substep);
            }
            for (SphereObject* s : spheres) {
                s->update();
            }
        }

        shader.setViewMatrix(camera.getViewMatrix());

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < walls.size(); i++) {
                walls[i]->updateModelMatrix();
                walls[i]->faceRenderer.render();
            }

            for (SphereObject* o : spheres) {
                o->updateModelMatrix();
                o->faceRenderer.render();
            }

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }
}
