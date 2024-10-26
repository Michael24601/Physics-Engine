
#ifdef DONE_REFACTOR

#include "simulations.h"

using namespace pe;


void pe::runMirrors() {

    GlfwWindowWrapper window(800, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 100.0f, 500.0f),
        glm::vec3(0.0f, 100.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.01,
        0.01
    );

    // Shaders
    std::vector<CookTorranceReflectionShader*> refShaders;
    std::vector<DiffuseLightingShader*> cubeShaders;

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorPurple(0.7, 0.1, 0.8, 1);
    glm::vec4 colorBlack(0.2, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
    glm::vec4 colorGrey(0.35, 0.35, 0.35, 1);
    glm::vec4 colorTrans(0, 0, 0, 1.0);


    glm::vec3 lightPos[]{ glm::vec3(0, 500, 0), glm::vec3(-500, 0, -500) };
    glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0) };

    std::vector<RectangularPrism*> prisms{
        new RectangularPrism(1, 200, 100, 20, Vector3D(100, 103, 0), new RigidBody),
        new RectangularPrism(1, 200, 100, 20, Vector3D(-100, 103, 0), new RigidBody)
    };
    std::vector<RectangularPrism*> blocks{
        new RectangularPrism(5000, 10, 5000, 0, Vector3D(0, -5, 0), new RigidBody),
        new RectangularPrism(5, 206, 106, 0, Vector3D(103, 103, 0), new RigidBody),
        new RectangularPrism(5, 206, 106, 0, Vector3D(-103, 103, 0), new RigidBody),
        new RectangularPrism(20, 20, 20, 20, Vector3D(0, 0, 0), new RigidBody)
    };

    prisms[0]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 1, 0), 0.0);
    prisms[1]->body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 1, 0), -0.0);

    prisms[0]->body->calculateDerivedData();
    prisms[1]->body->calculateDerivedData();

    for (int i = 0; i < 2; i++) {
        CookTorranceReflectionShader* shader = new CookTorranceReflectionShader;
        FaceData data = getFaceData(*prisms[i]);
        std::vector<std::vector<glm::vec3>> d{
            data.vertices, data.normals
        };
        shader->sendVaribleData(d, GL_STATIC_DRAW);
        shader->setTrianglesNumber(data.vertices.size());
        shader->setBaseColor(colorGreen);
        shader->setLightPosition(lightPos, 1);
        shader->setLightColors(lightColor, 1);
        shader->setReflectionStrength(1);
        shader->setFresnel(0.05);
        shader->setRoughness(0.5);
        shader->setModelMatrix(
            convertToGLM(prisms[i]->getTransformMatrix())
        );

        refShaders.push_back(shader);
    }

    for (int i = 0; i < 4; i++) {
        DiffuseLightingShader* shader = new DiffuseLightingShader;
        FaceData data = getFaceData(*blocks[i]);
        std::vector<std::vector<glm::vec3>> d{
            data.vertices, data.normals
        };
        shader->sendVaribleData(d, GL_STATIC_DRAW);
        shader->setTrianglesNumber(data.vertices.size());
        shader->setObjectColor(colorBlack);
        shader->setLightPosition(lightPos, 1);
        cubeShaders.push_back(shader);
    }
    cubeShaders[0]->setObjectColor(colorWhite);
    cubeShaders[3]->setObjectColor(colorRed);

    EnvironmentMapper mapper0(512, 512, 1);
    EnvironmentMapper mapper1(512, 512, 2);

    float deltaT = 0.07;

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

        for (int i = 0; i < 4; i++) {
            blocks[i]->body->calculateDerivedData();
            cubeShaders[i]->setModelMatrix(
                convertToGLM(blocks[i]->getTransformMatrix())
            );
        }

        glm::vec2 pos = window.getCursorPosition();
        blocks[3]->body->position = Vector3D(
            -pos.y,
            80,
            -pos.x
        );

        // Clears the environment map
        refShaders[0]->setEnvironmentMap(0, 1);
        refShaders[1]->setEnvironmentMap(0, 2);

        std::vector<Shader*> shaders0{ refShaders[1], cubeShaders[0], 
            cubeShaders[1], cubeShaders[2], cubeShaders[3] };
        std::vector<Shader*> shaders1{ refShaders[0], cubeShaders[0],
            cubeShaders[1], cubeShaders[2], cubeShaders[3] };

        // Captures environment recursively to create infinite mirror effect
        for (int i = 0; i < 10; i++) {
            mapper0.captureEnvironment(
                convertToGLM(prisms[0]->body->position), shaders0
            );
            mapper1.captureEnvironment(
                convertToGLM(prisms[1]->body->position), shaders1
            );

            //saveCubemapFaces(mapper0.getTexture(), 512, 512,
            //  "C:\\Users\\msaba\\Desktop\\cm\\" + std::to_string(i));

            refShaders[0]->setEnvironmentMap(mapper0.getTexture(), 1);
            refShaders[1]->setEnvironmentMap(mapper1.getTexture(), 2);
        }

        refShaders[0]->setProjectionMatrix(camera.getProjectionMatrix());
        refShaders[1]->setProjectionMatrix(camera.getProjectionMatrix());
        refShaders[0]->setViewMatrix(camera.getViewMatrix());
        refShaders[1]->setViewMatrix(camera.getViewMatrix());

        for (int i = 0; i < 4; i++) {
            cubeShaders[i]->setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i]->setViewMatrix(camera.getViewMatrix());
        }

        // Clears default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < 4; i++) {
                cubeShaders[i]->drawFaces();
            }
            refShaders[0]->drawFaces();
            refShaders[1]->drawFaces();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}

#endif