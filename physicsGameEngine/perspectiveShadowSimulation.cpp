
#include "simulations.h"
#include "glfwWindowWrapper.h"
#include "rotatingCamera.h"
#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "cubemapShadowMappingShader.h"
#include "polyhedra.h"
#include "importedMesh.h"
#include "renderComponent.h"
#include "pointDepthMapper.h"
#include "pointProjection.h"

using namespace pe;

void pe::runPerspectiveShadowSimulation() {

    GlfwWindowWrapper window(800, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 100.0f, 500.0f),
        glm::vec3(0.0f, 100.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.001,
        0.001
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorPurple(0.7, 0.1, 0.8, 1);
    glm::vec4 colorBlack(0.2, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
    glm::vec4 colorGrey(0.35, 0.35, 0.35, 1);
    glm::vec4 colorTrans(0, 0, 0, 1.0);

    glm::vec3 lightPos[]{ glm::vec3(500, 300, 0) };
    glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0) };

    // Shaders
    CubemapShadowMappingShader shader;
    shader.setLightPosition(lightPos[0]);
    shader.setProjectionMatrix(camera.getProjectionMatrix());
    shader.setPCF(true);
    shader.setShadowStrength(0.8);
    shader.setFarPlane(2000);

    DiffuseLightingShader diffuseShader;
    diffuseShader.setLightPosition(lightPos, 1);
    diffuseShader.setLightColors(lightColor, 1);
    diffuseShader.setProjectionMatrix(camera.getProjectionMatrix());

    std::string filename = "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\angel.obj";
    Mesh* mesh = extractMesh(filename);
    VertexBuffer buffer = createFaceVertexBuffer(
        mesh, GL_STATIC_DRAW, NORMALS::FACE_NORMALS, UV::INCLUDE
    );
    buffer.setData(generateFaceData(
        mesh, NORMALS::FACE_NORMALS, UV::INCLUDE
    ));
    RenderComponent renderer;
    renderer.setColor(colorGrey);
    renderer.setShader(&diffuseShader);
    renderer.setVertexBuffer(&buffer);

    CuboidObject ground(1000, 100, 1000, Vector3D(0, -200, 0), Quaternion::IDENTITY, 0);
    ground.body.setInfiniteMass();
    ground.faceRenderer.setColor(colorWhite);
    ground.updateModelMatrix();
    ground.faceRenderer.setShader(&shader);

    // Shadow mapping

    PointProjection projection(lightPos[0], 90.0, 0.1, 2000);
    PointDepthMapper mapper(1024, 1024);

    // Light movement speed
    float speed = 0.1;

    float deltaT = 0.07;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
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
            lightPos[0].x -= speed;
            diffuseShader.setLightPosition(lightPos, 1);
            shader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            lightPos[0].x += speed;
            diffuseShader.setLightPosition(lightPos, 1);
            shader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);
        }

        // Moves block with mouse
        glm::vec2 pos = window.getCursorPosition();
        Vector3D position(pos.x, 80, -pos.y);
        Matrix3x4 transform(Matrix3x3::IDENTITY, position);
        renderer.setModel(convertToGLM(transform));

        std::vector<RenderComponent*> objects{ &renderer };
        mapper.captureDepth(
            lightPos[0],
            projection.getViewMatrices(),
            projection.getProjectionMatrix(),
            projection.getFarPlane(),
            objects
        );

        // saveCubemapDepth(mapper.getTexture(), 1024, 1024, "C:\\Users\\msaba\\Desktop\\ff");

        shader.setViewMatrix(camera.getViewMatrix());
        shader.setCubemapShadowMap(mapper.getTexture());
        diffuseShader.setViewMatrix(camera.getViewMatrix());

        // Clears default framebuffer
        if (deltaTime >= frameRate) {

            // Unbinds framebuffer to render to default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderer.render();
            ground.faceRenderer.render();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

    delete mesh;
}