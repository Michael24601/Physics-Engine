
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "polyhedra.h"
#include "environmentMapper.h"
#include "joint.h"
#include "collisionResolver.h"
#include "rigidBodyGravity.h"

using namespace pe;


void pe::runRagdoll() {

    GlfwWindowWrapper window(800, 800, 6, "window", false);

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
    glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
    glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
    glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
    glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
    glm::vec4 colorGrey(0.7, 0.7, 0.7, 1.0);

    glm::vec3 lightPos[]{
        glm::vec3(0.0f, 300.0f, 400.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    GLuint texture = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\wood.jpg"
    );

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0, 0, 500),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.0003,
        0.0003
    );

    // Shaders
    DiffuseLightingShader diffuseShader;
    SolidColorShader shader;
    diffuseShader.setLightColors(lightColors, 1);
    diffuseShader.setLightPosition(lightPos, 1);
    diffuseShader.setProjectionMatrix(camera.getProjectionMatrix());
    shader.setProjectionMatrix(camera.getProjectionMatrix());

    real torsoHeight = 65, torsoWidth = 85, coreHeight = 55, coreWidth = 75;
    real joinRadius = 15;
    real armLength = 50, legLength = 50;
    std::vector<PolyhedronObject*> prisms{
        // Head 0
        new SphereObject(30, 20, 20, Vector3D(100, 100, 0),  Quaternion::IDENTITY, 50),
        // Torso 1
        new CuboidObject(torsoWidth, torsoHeight, 60, Vector3D(0, 0, 0), Quaternion::IDENTITY, 100),
        // Core 2
        new CuboidObject(coreWidth, coreHeight, 60, Vector3D(0, 0, 0), Quaternion::IDENTITY, 80),
        // L Arm 3
        new CuboidObject(20, armLength, 20, Vector3D(-100, 0, 0), Quaternion::IDENTITY, 20),
        // R Arm 4
        new CuboidObject(20, armLength, 20, Vector3D(100, 0, 0), Quaternion::IDENTITY, 20),
        // L Elbow 5
        new SphereObject(joinRadius, 15, 15, Vector3D(-100, -100, 0), Quaternion::IDENTITY, 10),
        // R Elbow 6
        new SphereObject(joinRadius, 15, 15, Vector3D(100, -100, 0), Quaternion::IDENTITY, 10),
        // L Hand 7
        new CuboidObject(20, armLength, 20, Vector3D(-100, -100, 0),  Quaternion::IDENTITY, 20),
        // R Hand 8
        new CuboidObject(20, armLength, 20, Vector3D(100, -100, 0), Quaternion::IDENTITY, 20),
        // L Thigh 9
        new CuboidObject(30, legLength, 30, Vector3D(-100, -200, 0), Quaternion::IDENTITY, 20),
        // R Thigh 10
        new CuboidObject(30,legLength, 30, Vector3D(100, -200, 0), Quaternion::IDENTITY, 20),
        // L Knee 11
        new SphereObject(joinRadius, 15, 15, Vector3D(-100, -200, 0), Quaternion::IDENTITY, 10),
        // R Knee 12
        new SphereObject(joinRadius, 15, 15, Vector3D(100, -200, 0), Quaternion::IDENTITY, 10),
        // L Thigh 9
        new CuboidObject(30, legLength, 30, Vector3D(-100, -300, 0), Quaternion::IDENTITY, 20),
        // R Thigh 10
        new CuboidObject(30,legLength, 30, Vector3D(100, -300, 0), Quaternion::IDENTITY, 20),
    };

    for (PolyhedronObject* prism : prisms) {
        prism->body.angularDamping = 0.3;
        prism->body.linearDamping = 0.9;
    };

    std::vector<std::vector<Vector3D>> connections{
        {prisms[0]->mesh->getVertex(1), Vector3D(0, torsoHeight / 2 + 5, 0)},
        {Vector3D(0, -torsoHeight / 2, 0), Vector3D(0, coreHeight / 2 + 5, 0)},
        // Arm torso
        {Vector3D(0, armLength / 2, 0), Vector3D(-torsoWidth / 2 - 20, torsoHeight / 2 + 5, 0)},
        {Vector3D(0, armLength / 2, 0), Vector3D(torsoWidth / 2 + 20, torsoHeight / 2 + 5, 0)},
        // Arm elbow
        {Vector3D(0, joinRadius, 0), Vector3D(0, -armLength / 2, 0)},
        {Vector3D(0, joinRadius, 0), Vector3D(0, -armLength / 2, 0)},
        // Elbow hand
        {Vector3D(0, -joinRadius, 0), Vector3D(0, armLength / 2, 0)},
        {Vector3D(0, -joinRadius, 0), Vector3D(0, armLength / 2, 0)},
        // Leg Core
        {Vector3D(0, legLength / 2, 0), Vector3D(-coreWidth / 2 + 10, -coreHeight / 2, 0)},
        {Vector3D(0, legLength / 2, 0), Vector3D(coreWidth / 2 - 10, -coreHeight / 2, 0)},
        // Knee Thigh
        {Vector3D(0, joinRadius, 0), Vector3D(0, -legLength / 2, 0)},
        {Vector3D(0, joinRadius, 0), Vector3D(0, -legLength / 2, 0)},
        // Knee Feet
        {Vector3D(0, -joinRadius, 0), Vector3D(0, legLength / 2, 0)},
        {Vector3D(0, -joinRadius, 0), Vector3D(0, legLength / 2, 0)},
    };

    std::vector<Joint*> joints{
        // Head Torso
        new Joint(
            &prisms[0]->body, &prisms[1]->body,
            connections[0][0], connections[1][1], 5
        ),
        // Torso Core
        new Joint(
            &prisms[1]->body, &prisms[2]->body,
            connections[1][0], connections[1][1], 5
        ),
        // L Arm Torso 
        new Joint(
            &prisms[3]->body, &prisms[1]->body,
            connections[2][0], connections[2][1], 5
        ),
        // R Arm Torso 
        new Joint(
            &prisms[4]->body, &prisms[1]->body,
            connections[3][0], connections[3][1], 5
        ),
        // L Elbow Arm 
        new Joint(
            &prisms[5]->body, &prisms[3]->body,
            connections[4][0], connections[4][1], 5
        ),
        // R Elbow Arm
        new Joint(
            &prisms[6]->body, &prisms[4]->body,
            connections[5][0], connections[5][1], 5
        ),
        // L Elbow Hand
         new Joint(
            &prisms[5]->body, &prisms[7]->body,
            connections[6][0], connections[6][1], 5
        ),
        // R Elbow Hand 
        new Joint(
            &prisms[6]->body, &prisms[8]->body,
            connections[7][0], connections[7][1], 5
        ),
        // L Elbow Hand
        new Joint(
           &prisms[9]->body, &prisms[2]->body,
           connections[8][0], connections[8][1], 5
        ),
        // R Elbow Hand 
        new Joint(
            &prisms[10]->body, &prisms[2]->body,
            connections[9][0], connections[9][1], 5
        ),
        // L Knee Thigh
        new Joint(
            &prisms[11]->body, &prisms[9]->body,
            connections[10][0], connections[10][1], 5
        ),
        // R Knee Thigh
        new Joint(
            &prisms[12]->body, &prisms[10]->body,
            connections[11][0], connections[11][1], 5
        ),
        // L Knee Foot
        new Joint(
            &prisms[11]->body, &prisms[13]->body,
            connections[12][0], connections[11][1], 5
        ),
        // R Knee Foot
        new Joint(
            &prisms[12]->body, &prisms[14]->body,
            connections[13][0], connections[13][1], 5
        ),
    };

    for (int i = 0; i < prisms.size(); i++) {
        prisms[i]->faceRenderer.setTexture(texture);
        prisms[i]->faceRenderer.setShader(&diffuseShader);
    }

    // For the lines (dynamic as the lines deform).
    // We have two vertices per joint and every joint has a line except for 2
    VertexBuffer lineBuffer(
        (joints.size() - 2) * 2, 
        std::vector<unsigned int>{3}, 
        2, GL_DYNAMIC_DRAW
    );
    RenderComponent lineRenderer;
    lineRenderer.setColor(colorWhite);
    lineRenderer.setVertexBuffer(&lineBuffer);
    lineRenderer.setShader(&shader);

    RigidBodyGravity g(Vector3D(0, -10, 0));

    float deltaT = 0.0015;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    glfwSetFramebufferSizeCallback(
        window.getWindow(),
        window.framebuffer_size_callback
    );

    Vector3D pos = prisms[0]->body.position;

    while (!glfwWindowShouldClose(window.getWindow())) {

        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        glfwPollEvents();
        window.processInput();
        camera.processInput(frameRate);
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec2 worldPos = window.getCursorPosition();
            Vector3D vec(
                worldPos.x - prisms[0]->body.position.x,
                worldPos.y - prisms[0]->body.position.y,
                worldPos.x - prisms[0]->body.position.z
            );
            pos += vec * 0.5;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_L) == GLFW_PRESS) {
            glm::vec2 worldPos = window.getCursorPosition();
            Vector3D vec(
                worldPos.x - prisms[7]->body.position.x,
                worldPos.y - prisms[7]->body.position.y,
                worldPos.x - prisms[7]->body.position.z
            );
            prisms[7]->body.position += vec * 0.25;
        }
        else if (glfwGetKey(window.getWindow(), GLFW_KEY_R) == GLFW_PRESS) {
            glm::vec2 worldPos = window.getCursorPosition();
            Vector3D vec(
                worldPos.x - prisms[8]->body.position.x,
                worldPos.y - prisms[8]->body.position.y,
                worldPos.x - prisms[8]->body.position.z
            );
            prisms[8]->body.position += vec * 0.25;
        }

        prisms[0]->body.position = pos;

        int numSteps = 10;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (PolyhedronObject* prism : prisms) {
                g.updateForce(&prism->body, substep);
            };

            std::vector<Contact> contacts;
            for (Joint* joint : joints) {
                joint->addContact(contacts);
            }
            CollisionResolver resolver(10, 1, 0.01, 0.01);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);


            for (PolyhedronObject* prism : prisms) {
                prism->body.integrate(substep);
                prism->update();
            };
        }

        diffuseShader.setViewMatrix(camera.getViewMatrix());
        shader.setViewMatrix(camera.getViewMatrix());

        // Lines
        std::vector<float> vertices;
        for (int i = 0; i < joints.size(); i++) {
            if (i != 2 && i != 3) {
                Vector3D v1 = joints[i]->body[0]->transformMatrix.transform(joints[i]->position[0]);
                Vector3D v2 = joints[i]->body[1]->transformMatrix.transform(joints[i]->position[1]);
                vertices.push_back(v1.x);
                vertices.push_back(v1.y);
                vertices.push_back(v1.z);
                vertices.push_back(v2.x);
                vertices.push_back(v2.y);
                vertices.push_back(v2.z);
            }
        }
        lineBuffer.setData(vertices);

        for (int i = 0; i < prisms.size(); i++) {
            prisms[i]->updateModelMatrix();
        }

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < prisms.size(); i++) {
                prisms[i]->faceRenderer.render();
            }
            lineRenderer.render();

            glfwSwapBuffers(window.getWindow());
            deltaTime = 0.0f;
        }
    }

}