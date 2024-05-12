
#include "simulations.h"

using namespace pe;


void pe::runReflection() {

    GlfwWindowWrapper window(800, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 500.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.01,
        0.01
    );

    // Shaders
    CookTorranceShader cookShader;
    SkyboxShader skyboxShader;
    CookTorranceSkyboxReflectionShader refShader2;

    GLuint skybox = loadCubemap(std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\right.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\left.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\top.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\bottom.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\front.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\back.jpg"
    });

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
    glm::vec4 colorGrey(0.35, 0.35, 0.35, 1);
    glm::vec4 colorTrans(0, 0, 0, 1.0);


    glm::vec3 lightPos[]{ glm::vec3(0, 500, 0), glm::vec3(-500, 0, -500) };
    glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0) };

    RectangularPrism c(100, 100, 100, 20, Vector3D(0, 0, 400), new RigidBody);

    real radius = 150;
    std::string filename = "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\moai.obj";
    Polyhedron c2 = returnPrimitive(filename, 1, Vector3D::ZERO, new RigidBody(), 2);
    c2.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), PI / 2);
    c2.body->calculateDerivedData();

    skyboxShader.setSkybox(skybox);
    skyboxShader.setModelScaleAndTranslate(2000, glm::vec3(0));

    FaceData data = getFaceData(c2);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals
    };
    refShader2.sendVaribleData(d, GL_STATIC_DRAW);
    refShader2.setTrianglesNumber(data.vertices.size());
    refShader2.setLightPosition(lightPos, 1);
    refShader2.setLightColors(lightColor, 1);
    refShader2.setBaseColor(colorRed);
    refShader2.setRoughness(0.05);
    refShader2.setFresnel(0.5);
    refShader2.setSkybox(skybox);
    refShader2.setLightInfluence(0.0);
    refShader2.setReflectionStrength(1.0);

    data = getFaceData(c);
    d = {
        data.vertices, data.normals
    };
    cookShader.sendVaribleData(d, GL_STATIC_DRAW);
    cookShader.setTrianglesNumber(data.vertices.size());
    cookShader.setLightPosition(lightPos, 1);
    cookShader.setLightColors(lightColor, 1);
    cookShader.setObjectColor(colorRed);
    cookShader.setRoughness(0.05);
    cookShader.setFresnel(0.5);

    EnvironmentMapper mapper(512, 512);

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

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();


        glm::vec2 pos = window.getCursorPosition();
        c.body->position = {
            c.body->position.x,
            pos.y,
            pos.x
        };


        cookShader.setModelMatrix(convertToGLM(c.body->transformMatrix));
        cookShader.setViewMatrix(camera.getViewMatrix());
        cookShader.setProjectionMatrix(camera.getProjectionMatrix());

        refShader2.setModelMatrix(convertToGLM(c2.body->transformMatrix));
        refShader2.setViewMatrix(camera.getViewMatrix());
        refShader2.setProjectionMatrix(camera.getProjectionMatrix());


        std::vector<Shader*> shaders{ &cookShader };

        mapper.captureEnvironment(
            convertToGLM(c2.body->position),
            shaders
        );

        refShader2.setEnvironmentMap(mapper.getTexture());

        cookShader.setViewMatrix(camera.getViewMatrix());
        cookShader.setProjectionMatrix(camera.getProjectionMatrix());

        skyboxShader.setViewMatrix(camera.getViewMatrix());
        skyboxShader.setProjectionMatrix(camera.getProjectionMatrix());

        // Unbind framebuffer to render to default framebuffer (window)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clear default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cookShader.drawFaces();
            skyboxShader.drawFaces();
            refShader2.drawFaces();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}