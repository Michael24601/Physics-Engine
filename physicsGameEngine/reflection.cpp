
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "shadowMappingShader.h"
#include "polyhedra.h"
#include "depthMapper.h"
#include "importedMesh.h"
#include "renderComponent.h"
#include "cookTorranceReflectionShader.h"
#include "cookTorranceShader.h"
#include "skyboxRenderer.h"
#include "environmentMapper.h"

using namespace pe;

void pe::runReflection() {

    std::cout << "Use arrow keys to move the camera\n";
    std::cout << "Move the mouse to move the cube\n";

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
    CookTorranceReflectionShader refShader;

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

    CuboidObject c(100, 100, 100, Vector3D(0, 0, 400), Quaternion::IDENTITY, 1);
    c.faceRenderer.setColor(colorRed);
    c.faceRenderer.setShader(&cookShader);

    std::string filename = "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\moai.obj";
    Mesh* mesh = extractMesh(filename);
    VertexBuffer buffer = createFaceVertexBuffer(
        mesh, GL_STATIC_DRAW, NORMALS::FACE_NORMALS, UV::INCLUDE
    );
    buffer.setData(generateFaceData(
        mesh, NORMALS::VERTEX_NORMALS, UV::INCLUDE
    ));
    RenderComponent renderer;
    renderer.setColor(colorGrey);
    renderer.setShader(&refShader);
    renderer.setVertexBuffer(&buffer);
    Vector3D position(0, 0, 0);
    Quaternion rotation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), PI / 2.0);
    Matrix3x4 transform(Matrix3x3(rotation), position);
    renderer.setModel(convertToGLM(transform));

    // The skybox
    SkyboxRenderer skybox(
        std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\right.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\left.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\top.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\bottom.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\front.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\back.jpg"
        },
        2000
    );

    cookShader.setLightColors(lightColor, 1);
    cookShader.setLightPosition(lightPos, 1);
    cookShader.setFresnel(0.5);
    cookShader.setRoughness(0.05);

    refShader.setLightColors(lightColor, 1);
    refShader.setLightPosition(lightPos, 1);
    refShader.setFresnel(0.5);
    refShader.setRoughness(0.05);
    refShader.setReflectionStrength(1.0);

    EnvironmentMapper mapper(512, 512, 0.1f, 2500.0f);

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


        glm::vec2 pos = window.getCursorPosition();
        c.body.position = {
            c.body.position.x,
            pos.y,
            pos.x
        };

        c.body.calculateDerivedData();

        std::vector<RenderComponent*> objects{ &c.faceRenderer, &skybox.skyboxRenderer };

        mapper.captureEnvironment(
            glm::vec3(0.0),
            objects
        );

        // saveCubemapFaces(mapper.getTexture(), 512, 512, "C:\\Users\\msaba\\Desktop\\ff");

        renderer.setEnvironmentMap(mapper.getTexture());
        c.updateModelMatrix();

        cookShader.setViewMatrix(camera.getViewMatrix());
        cookShader.setProjectionMatrix(camera.getProjectionMatrix());
        refShader.setViewMatrix(camera.getViewMatrix());
        refShader.setProjectionMatrix(camera.getProjectionMatrix());

        skybox.setViewProjection(camera.getViewMatrix(), camera.getProjectionMatrix());

        // Unbind framebuffer to render to default framebuffer (window)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clear default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            c.faceRenderer.render();
            renderer.render();
            skybox.renderSkybox();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

}