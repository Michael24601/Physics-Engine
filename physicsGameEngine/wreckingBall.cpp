
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
#include "boundingVolumeRenderer.h"

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

    // Light

    glm::vec3 lightPos[]{
        glm::vec3(200.0f, 2000.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };

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


    // Shaders

    CookTorranceShader cookTorranceShader;
    SolidColorShader lineShader;

    cookTorranceShader.setLightPosition(lightPos, 1);
    cookTorranceShader.setLightColors(lightColors, 1);
    cookTorranceShader.setFresnel(0.5);
    cookTorranceShader.setRoughness(0.05);
    lineShader.setProjectionMatrix(camera.getProjectionMatrix());
    cookTorranceShader.setProjectionMatrix(camera.getProjectionMatrix());

    // Cubes

    real mass = 1.5;
    std::vector<CuboidObject*> prisms;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {

                int index = i * 9 + j * 3 + k;

                real x = i * 200 - 400;
                real y = j * 200 - 200;
                real z = k * 200 - 400;

                CuboidObject* prism = new CuboidObject(
                    200, 200, 200, Vector3D(x, y, z), Quaternion::IDENTITY, mass
                );
                prism->body.angularDamping = 0.8;
                prism->body.linearDamping = 0.95;
                prism->faceRenderer.setColor(colorPurple);
                prism->faceRenderer.setShader(&cookTorranceShader);

                prisms.push_back(prism);
            }
        }
    }

    // Ground

    CuboidObject ground(5000, 100, 5000, Vector3D(0, -350, -0), Quaternion::IDENTITY, 0);
    ground.body.inverseMass = 0;
    ground.faceRenderer.setColor(colorWhite);
    ground.faceRenderer.setShader(&cookTorranceShader);

    // Sphere

    SphereObject sphere(200, 20, 20, Vector3D(800, 400, 0), Quaternion::IDENTITY, 5);
    sphere.body.angularDamping = 0.8;
    sphere.body.linearDamping = 0.95;
    sphere.faceRenderer.setColor(colorRed);
    sphere.faceRenderer.setShader(&cookTorranceShader);


    // For the line (dynamic as the line deforms)
    VertexBuffer lineBuffer(2, std::vector<unsigned int>{3}, 2, GL_DYNAMIC_DRAW);
    RenderComponent lineRenderer;
    lineRenderer.setColor(colorWhite);
    lineRenderer.setVertexBuffer(&lineBuffer);
    lineRenderer.setShader(&lineShader);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));
    RigidBody b;
    b.position = Vector3D(800, 700, 0);
    RigidBodySpringForce f(sphere.mesh->getVertex(0), &b, Vector3D(), 0.3, 300);

    // Renderer for bounding boxes
    BoundingVolumeRenderer renderer(20, colorWhite);

    // Framerate

    float deltaT = 0.002;
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
            sphere.body.position.x = worldPos.x * 4;
            sphere.body.position.y = worldPos.y * 4;
        }

        int numSteps = 2;
        real substep = deltaT / numSteps;

        
        while (numSteps--) {

            // Applying the forces

            for (CuboidObject* prism : prisms) {
                g.updateForce(&(prism->body), substep);
            }
            g.updateForce(&(sphere.body), substep);
            f.updateForce(&(sphere.body), substep);

            // Coarse collision detection

            BoundingVolumeHierarchy BVH;
            for (int i = 0; i < prisms.size(); i++) {
                BVH.insert(
                    prisms[i], 
                    prisms[i]->boundingVolumeTransform.getTranslation(),
                    prisms[i]->boundingVolume->getBVHSphereRadius()
                );
            }
            BVH.insert(
                &ground, 
                ground.boundingVolumeTransform.getTranslation(),
                ground.boundingVolume->getBVHSphereRadius()
            );
            BVH.insert(
                &sphere, 
                sphere.boundingVolumeTransform.getTranslation(),
                sphere.boundingVolume->getBVHSphereRadius()
            );

            PotentialContact con[1000];
            int size = BVH.getPotentialContacts(con, 1000);

            // Fine collision detection/resolution
            
            std::vector<Contact> contacts;
            for (int i = 0; i < size; i++) {
                generateContacts(
                    *con[i].object[0], *con[i].object[1], 
                    contacts, 0.25, 0.0
                );
            }

            CollisionResolver resolver(1, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            // Integrating the bodies

            for (CuboidObject* prism : prisms) {
                prism->body.integrate(substep);
            }
            ground.body.integrate(substep);
            sphere.body.integrate(substep);

            // Updating the objects

            for (CuboidObject* prism : prisms) {
                prism->update();
            }
            ground.update();
            sphere.update();
            b.calculateDerivedData();
        }

        Vector3D p = sphere.body.transformMatrix.transform(sphere.mesh->getVertex(0));
        std::vector<float> springData {
            p.x, p.y, p.z, b.position.x, b.position.y, b.position.z
        };
        lineBuffer.setData(springData);

        for (int i = 0; i < prisms.size(); i++) {
            prisms[i]->updateModelMatrix();
        }
        sphere.updateModelMatrix();
        ground.updateModelMatrix();

        lineShader.setViewMatrix(camera.getViewMatrix());
        cookTorranceShader.setViewMatrix(camera.getViewMatrix());

        renderer.setCameraMatrices(camera);

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Rendering 
    
            for (int i = 0; i < prisms.size(); i++) {
                prisms[i]->faceRenderer.render();
            //    renderer.renderBoundingVolume(prisms[i]->boundingVolume, prisms[i]->boundingVolumeTransform);
            }
            ground.faceRenderer.render();
            sphere.faceRenderer.render();
            lineRenderer.render();

            // renderer.renderBoundingVolume(sphere.boundingVolume, sphere.boundingVolumeTransform);

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }
}