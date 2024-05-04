
// Must be before any SFML or glfw or glm or glew files                        
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <gtx/string_cast.hpp>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>
#include <random>

#include "particle.h"
#include "particleSpringForce.h"
#include "particleBungeeForce.h"
#include "anchoredSpringForce.h"
#include "particleGravity.h"
#include "particleRod.h"
#include "particleCable.h"

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "rectangularPrism.h"
#include "solidSphere.h"
#include "pyramid.h"
#include "cylinder.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "cone.h"

#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "diffuseSpecularLightingShader.h"
#include "cookTorranceShader.h"
#include "shaderInterface.h"
#include "anisotropicShader.h"
#include "cookTorranceTextureShader.h"
#include "anisotropicTextureShader.h"

#include "sat.h"
#include "contact.h"
#include "contactGeneration.h"
#include "collisionResolver.h"

#include "BVHNode.h"
#include "boundingVolumeHierarchy.h"
#include "boundingSphere.h"

#include "cloth.h"

#include "fixedJoint.h"
#include "springJoint.h"
#include "joint.h"
#include "ballJoint.h"
#include "sphericalJoint.h"

#include "openglUtility.h"
#include "fineCollisionDetection.h"
#include "gjk.h"

#include "cookTorranceReflectionShader.h"
#include "customPrimitive.h"
#include "breakable.h"

#include "freeMovingCamera.h"
#include "rotatingCamera.h"

#include "particleCollisionDetection.h"
#include "particleContactResolver.h"

#include "blob.h"
#include "skyboxShader.h"
#include "cookTorranceReflectionShaderWithSkybox.h"
#include "shadowMappingShader.h"
#include "simpleShader.h"

#include "environmentMapper.h"
#include "depthMapper.h"
#include "glfwWindowWrapper.h"
#include "diffuseLightingTextureShader.h"

#include "firstPersonCamera.h"
#include "directionalProjection.h"
#include "pointProjection.h"

#include "textureShader.h"
#include "diffuseLightingFourTextureShader.h"
#include "shadowMappingTextureShader.h"


using namespace pe;
using namespace std;

#define SIM_15

#ifdef SIM_1

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit(); 
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    glFrontFace(GL_CCW);
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is
    // definitely best not to render both sides (enable culling) so it
    // appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    AnisotropicShader aniShader;
    CookTorranceTextureShader cookTexShader;
    AnisotropicTextureShader aniTexShader;

    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 600.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 0.0f, 0.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,
        cameraTarget, upVector);

    // Projection matrix, used for perspective
    // Field of View (FOV) in degrees
    real fov = 90.0f;
    // Aspect ratio
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
    // Near and far clipping planes
    real nearPlane = 0.1f;
    real farPlane = 10000.0f;

    // Create a perspective projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),
        aspectRatio, nearPlane, farPlane);

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(1200, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.03;

    real side = 100;
    RectangularPrism c1(side, side, side, 150, Vector3D(250, 0, -150), new RigidBody);

    real height = 150;
    real radius = 50;
    Cylinder c2(radius, height, 150, 20, Vector3D(200, 0, 200), new RigidBody);

    radius = 100;
    SolidSphere c3(radius, 150, 20, 20, Vector3D(-200, 0, -200), new RigidBody);

    height = 150;
    side = 100;
    Pyramid c4(side, height, 150, Vector3D(-200, 0, 200), new RigidBody);


    c1.body->angularDamping = 0.75;
    c1.body->linearDamping = 0.90;
    c2.body->angularDamping = 0.75;
    c2.body->linearDamping = 0.90;
    c3.body->angularDamping = 0.75;
    c3.body->linearDamping = 0.90;
    c4.body->angularDamping = 0.75;
    c4.body->linearDamping = 0.90;

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D origin;

    int particleNum = 2;

    RigidBody fixed1;
    fixed1.position = Vector3D(200, 200, -200);
    RigidBody fixed2;
    fixed2.position = Vector3D(200, 200, 200);
    RigidBody fixed3;
    fixed3.position = Vector3D(-200, 200, -200);
    RigidBody fixed4;
    fixed4.position = Vector3D(-200, 200, 200);

    RigidBodySpringForce force1(c1.localVertices[0], &fixed1, Vector3D(), 10, 100);
    RigidBodySpringForce force2(c2.localVertices[0], &fixed2, Vector3D(), 10, 100);
    RigidBodySpringForce force3(c3.localVertices[0], &fixed3, Vector3D(), 10, 100);
    RigidBodySpringForce force4(c4.localVertices[0], &fixed4, Vector3D(), 10, 100);

    ParticleGravity pg(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false,
        false,
        false,
        false
    };


    GLuint texture = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\dirty-metal.jpg");
    GLuint texture2 = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\world.jpg");
    GLuint texture3 = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\wood.jpg");
    GLuint texture4 = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\coke.jpg");


    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                isButtonPressed[0] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                isButtonPressed[1] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                isButtonPressed[2] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                isButtonPressed[3] = true;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = isButtonPressed[1] =
                    isButtonPressed[2] = isButtonPressed[3] = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            // Moves camera
             // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                cameraDistance *= 0.98;
                cameraPosition *= 0.98;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                cameraDistance *= 1.02;
                cameraPosition *= 1.02;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        c1.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        c3.body->calculateDerivedData();
        c4.body->calculateDerivedData();

        fixed1.calculateDerivedData();
        fixed2.calculateDerivedData();
        fixed3.calculateDerivedData();
        fixed4.calculateDerivedData();

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            g.updateForce(c1.body, substep);
            g.updateForce(c2.body, substep);
            g.updateForce(c3.body, substep);
            g.updateForce(c4.body, substep);

            force1.updateForce(c1.body, substep);
            force2.updateForce(c2.body, substep);
            force3.updateForce(c3.body, substep);
            force4.updateForce(c4.body, substep);

            std::vector<Contact> contacts;
            // Here we check for collision

            if (testIntersection(c1, c3)) {
                returnContacts(c1, c3, contacts);
            }
            //generateCollision(c1, c3, contacts);
            for (Contact& contact : contacts) {
                contact.friction = 1.0;
                contact.restitution = 0.5;
            }

            CollisionResolver resolver(5, 5, 0.01, 0.01);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            if (isButtonPressed[0]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c1.body->position.x = worldPos.x * 2;
                c1.body->position.y = worldPos.y * 2;
            }
            else if (isButtonPressed[1]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c2.body->position.x = worldPos.x * 2;
                c2.body->position.y = worldPos.y * 2;
            }
            else if (isButtonPressed[2]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c3.body->position.x = worldPos.x * 2;
                c3.body->position.y = worldPos.y * 2;
            }
            else if (isButtonPressed[3]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c4.body->position.x = worldPos.x * 2;
                c4.body->position.y = worldPos.y * 2;
            }

            c1.body->integrate(substep);
            c2.body->integrate(substep);
            c3.body->integrate(substep);
            c4.body->integrate(substep);

            c1.update();
            c2.update();
            c3.update();
            c4.update();
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
        glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
        glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
        glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
        glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
        glm::vec4 colorGrey(0.7, 0.7, 0.7, 1.0);

        EdgeData cordData;
        cordData.vertices.push_back(convertToGLM(fixed1.position));
        cordData.vertices.push_back(convertToGLM(c1.globalVertices[0]));
        cordData.vertices.push_back(convertToGLM(fixed2.position));
        cordData.vertices.push_back(convertToGLM(c2.globalVertices[0]));
        cordData.vertices.push_back(convertToGLM(fixed3.position));
        cordData.vertices.push_back(convertToGLM(c3.globalVertices[0]));
        cordData.vertices.push_back(convertToGLM(fixed4.position));
        cordData.vertices.push_back(convertToGLM(c4.globalVertices[0]));
        shader.drawEdges(cordData.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite);

        // Shape
        glm::vec3 lightPos[]{
            glm::vec3(0.0f, 100.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

        // Data
        FaceData data = getFaceData(c1);
        aniShader.drawFaces(data.vertices, data.normals, 
            data.tangents, data.bitangents,
            identity, viewMatrix, projectionMatrix, colorBlue,
            colorWhite, glm::vec4(0.2, 0.2, 0.2, 1.0), glm::vec3(0.0f, 100.0f, 0.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f), cameraPosition, 0.25, 0.15
        );

        data = getFaceData(c3);
        cookTexShader.drawFaces(data.vertices, data.normals, data.uvCoordinates, identity,
            viewMatrix, projectionMatrix, texture, 1, lightPos,
            lightColors, cameraPosition, 0.1, 1
        );

        data = getFaceData(c4);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 40
        );

        EdgeData e = getCollisionBoxData(c4);
        shader.drawEdges(e.vertices, identity,
            viewMatrix, projectionMatrix, colorWhite
        );

        FaceData d;
        getFaceData(c2.faces[0], &d);
        getFaceData(c2.faces[1], &d);
        phongShader.drawFaces(d.vertices, d.normals, identity,
            viewMatrix, projectionMatrix, colorGrey, 1, lightPos,
            lightColors, cameraPosition, 20
        );

        FaceData d2;
        for (int i = 2; i < c2.faces.size(); i++) {
            getFaceData(c2.faces[i], &d2);
        }
        phongShader.drawFaces(d2.vertices, d2.normals, identity,
            viewMatrix, projectionMatrix, colorYellow, 1, lightPos,
            lightColors, cameraPosition, 20
        );
        

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_2

int main() {
    
    GlfwWindowWrapper window(800, 800, 6, "window", false);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0, 0, 500),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.0005,
        0.001
    );

    // Shaders
    DiffuseLightingTextureShader poleShader;
    DiffuseLightingTextureShader lightShader;
    SkyboxShader skyboxShader;

    GLuint texture1 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\leb.jpg"
    );
    GLuint texture2 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\hit.png"
    );
    GLuint texture3 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\wood.jpg"
    );

    GLuint skybox = loadCubemap(std::vector<std::string>{
        "C:\\Users\\msaba\\Downloads\\cubemap_faces\\right.jpg",
            "C:\\Users\\msaba\\Downloads\\cubemap_faces\\left.jpg",
            "C:\\Users\\msaba\\Downloads\\cubemap_faces\\up.jpg",
            "C:\\Users\\msaba\\Downloads\\cubemap_faces\\down.jpg",
            "C:\\Users\\msaba\\Downloads\\cubemap_faces\\front.jpg",
            "C:\\Users\\msaba\\Downloads\\cubemap_faces\\back.jpg",
    });

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

    glm::vec3 lightPos[]{ 
        glm::vec3(-500 ,0 , 500), 
        glm::vec3(500, 0, 500) 
    };
    glm::vec4 lightColor[]{ 
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0)
    };

    int size = 21;
    real strength = 0.4;
    real mass = 0.45;
    real damping = 0.3;

    const real height = 400;
    const Vector3D topLeft(- 200, height / 2.0, 0);
    Cloth mesh(
        topLeft, 
        Vector3D(300, -height/2.0, 0),
        size, size,
        mass, damping, strength
    );

    Cylinder cylinder(10, 800, 10, 20, Vector3D(-205, -200, 0), new RigidBody());

    lightShader.setLightPosition(lightPos, 2);
    poleShader.setLightPosition(lightPos, 2);

    FaceData data = getFaceData(cylinder);
    std::vector<std::vector<glm::vec3>> d{
           data.vertices, data.normals, data.uvCoordinates
    };
    poleShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
    poleShader.setTrianglesNumber(data.vertices.size());

    skyboxShader.setSkybox(skybox);
    skyboxShader.setModelScale(2000);

    // The first row of particles is suspended
    for (int i = 0; i < size * size; i++) {
        if (i % size == 0) {
            mesh.particles[i]->setAwake(false);
        }
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    bool isPressed{false};
    
    float deltaT = 0.13;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 30;
    float frameRate = 1.0 / framesPerSecond;

    real windMultiplier = 1;

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
        if (glfwGetKey(window.getWindow(), GLFW_KEY_X) == GLFW_PRESS) {
            windMultiplier *= 1.0002;
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_Z) == GLFW_PRESS) {
            windMultiplier *= 0.9998;
        }

        int numSteps = 2;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            if (isPressed) {

                glm::vec2 mouse = window.getCursorPosition();

                Vector3D pos = Vector3D(
                    mouse.x,
                    mouse.y,
                    cylinder.body->position.z
                );

                cylinder.body->position = pos;

                for (int i = 0; i < size; i++) {
                    mesh.particles[i * size]->position = pos + 
                        Vector3D(0, ((size - i) * height / size), 0);
                }
            }

            cylinder.body->calculateDerivedData();

            for (Particle* particle : mesh.particles) {
                g.updateForce(particle, substep);
            }

            for (int i = 0; i < mesh.particles.size(); i++) {

                if (i < size) {
                    mesh.particles[i]->addForce(Vector3D(3, 6, 0) * windMultiplier);
                    continue;
                }
                else if (i >= (size - 1) * size) {
                    mesh.particles[i]->addForce(Vector3D(4, 3, 0) * windMultiplier);
                    continue;
                }

                int enter = generateRandomNumber(0, 5);
                if (enter) {
                    real x = generateRandomNumber(0.0f, 4.0f);
                    real y = generateRandomNumber(4.0f, 7.0f);
                    real z = generateRandomNumber(0.0f, 2.0f);
                    mesh.particles[i]->addForce(Vector3D(x, y, z) * windMultiplier);
                }
            }

            for (auto& force : mesh.forces) {
                force.force1.updateForce(force.force2.otherParticle, substep);
                force.force2.updateForce(force.force1.otherParticle, substep);
            }  
          
            mesh.applyConstraints();

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i]->isAwake) {
                    mesh.particles[i]->verletIntegrate(substep);
                }
            }

            mesh.update();
        }

        FaceData data = getFaceData(mesh);
        std::vector<std::vector<glm::vec3>> d{
            data.vertices, data.normals, data.uvCoordinates
        };
        lightShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
        lightShader.setTrianglesNumber(data.vertices.size());
        lightShader.setModelMatrix(identity);
        lightShader.setViewMatrix(camera.getViewMatrix());
        lightShader.setProjectionMatrix(camera.getProjectionMatrix());

        poleShader.setModelMatrix(convertToGLM(cylinder.getTransformMatrix()));
        poleShader.setViewMatrix(camera.getViewMatrix());
        poleShader.setProjectionMatrix(camera.getProjectionMatrix());

        skyboxShader.setProjectionMatrix(camera.getProjectionMatrix());
        skyboxShader.setViewMatrix(camera.getViewMatrix());
   
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // We set the texture here cause they use the same unit
            lightShader.setObjectTexture(texture1);
            lightShader.drawFaces();
            poleShader.setObjectTexture(texture3);
            poleShader.drawFaces();
            skyboxShader.drawFaces();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

    return 0;
}

#endif

#ifdef SIM_3

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    glFrontFace(GL_CCW);
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is
    // definitely best not to render both sides (enable culling) so it
    // appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    AnisotropicShader aniShader;
    TextureShader texShader;
    CookTorranceTextureShader cookTexShader;
    AnisotropicTextureShader aniTexShader;

    GLuint texture = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\world.jpg");

    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 1000.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 0.0f, 0.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,
        cameraTarget, upVector);

    // Projection matrix, used for perspective
    // Field of View (FOV) in degrees
    real fov = 90.0f;
    // Aspect ratio
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
    // Near and far clipping planes
    real nearPlane = 0.1f;
    real farPlane = 10000.0f;

    // Create a perspective projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),
        aspectRatio, nearPlane, farPlane);

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(1200, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.07;

    GLuint texture9 = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\stone.jpg");

    std::string filename = "C:\\Users\\msaba\\Documents\\textureMaps\\video_game.obj";
    Polyhedron p = ReturnPrimitive(filename, 100, Vector3D(0, 0, 0), new RigidBody, 100);
    //RectangularPrism p(150, 20, 100, 100, Vector3D(0, -100, -0), new RigidBody);
    p.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), 0.5);

    RectangularPrism c5(400, 200, 400, 100, Vector3D(0, -100, -0), new RigidBody);

    RectangularPrism c2(10000, 100, 10000, 0, Vector3D(0, -400, -0), new RigidBody);
    c2.body->inverseMass = 0;

    p.body->angularDamping = 0.9;
    p.body->linearDamping = 0.95;
    c5.body->angularDamping = 0.9;
    c5.body->linearDamping = 0.95;

    //c2.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(1, 0, 0), 0.2);

    RigidBodyGravity g(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false
    };

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                isButtonPressed[0] = true;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = isButtonPressed[1] =
                    isButtonPressed[2] = isButtonPressed[3] = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            // Moves camera
             // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                cameraDistance *= 0.98;
                cameraPosition *= 0.98;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                cameraDistance *= 1.02;
                cameraPosition *= 1.02;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        int numSteps = 1;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            c2.body->calculateDerivedData();
            p.body->calculateDerivedData();
            c5.body->calculateDerivedData();

            g.updateForce(p.body, substep);
            g.updateForce(c5.body, substep);

            std::vector<Contact> contacts;
           
            generateContactBoxAndBox(c2, p, contacts, 0.4, 0.0);
            generateContactBoxAndBox(c2, c5, contacts, 0.4, 0.0);
            generateContactBoxAndBox(c5, p, contacts, 0.4, 0.0);

            CollisionResolver resolver(10, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            c2.body->integrate(substep);
            p.body->integrate(substep);
            c5.body->integrate(substep);

            c2.update();
            // p.update();
            c5.update();
        }

        if (isButtonPressed[0]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            p.body->position.x = worldPos.x * 2;
            p.body->position.y = worldPos.y * 2;
            p.body->position.z = worldPos.x * 2;
            p.body->setAwake(true);
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
        glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
        glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
        glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
        glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
        glm::vec4 colorGrey(0.7, 0.7, 0.7, 1.0);

        // Shape
        glm::vec3 lightPos[]{
            glm::vec3(300.0f, 400.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

     
        FaceData data = getFaceData(c2);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );


        data = getFaceData(c5);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorRed, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );

        /*
        data = getFaceData(p);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorGrey, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );
        */


        EdgeData edata;
        edata = getCollisionBoxData(p);
        shader.drawEdges(edata.vertices,
            identity, viewMatrix, projectionMatrix, colorWhite
        );

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_4

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    glFrontFace(GL_CCW);
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is
    // definitely best not to render both sides (enable culling) so it
    // appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    AnisotropicShader aniShader;
    TextureShader texShader;
    CookTorranceTextureShader cookTexShader;
    AnisotropicTextureShader aniTexShader;

    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 600.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 0.0f, 0.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,
        cameraTarget, upVector);

    // Projection matrix, used for perspective
    // Field of View (FOV) in degrees
    real fov = 90.0f;
    // Aspect ratio
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
    // Near and far clipping planes
    real nearPlane = 0.1f;
    real farPlane = 10000.0f;

    // Create a perspective projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),
        aspectRatio, nearPlane, farPlane);

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(1200, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.02;

    real side = 100;
    RectangularPrism c1(side, side, side, 100, Vector3D(250, 0, -150), new RigidBody);

    real height = 150;
    side = 100;
    Pyramid c4(side, height, 50, Vector3D(-200, 0, 200), new RigidBody);

    RectangularPrism c3(side, side, side, 0.5, Vector3D(-200, 0, 200), new RigidBody);
    RectangularPrism c2(side, side, side, 0.005, Vector3D(-200, 0, 200), new RigidBody);

    Joint joint(
        c1.body, 
        c4.body,
        c1.localVertices[4],
        c4.localVertices[3],
        10
    );

    Joint joint2(
        c4.body,
        c3.body,
        c4.localVertices[1],
        c3.localVertices[1],
        10
    );

    Joint joint3(
        c3.body,
        c2.body,
        c3.localVertices[7],
        c2.localVertices[1],
        10
    );


    c1.body->angularDamping = 0.5;
    c1.body->linearDamping = 0.90;
    c4.body->angularDamping = 0.5;
    c4.body->linearDamping = 0.90;
    c3.body->angularDamping = 0.4;
    c3.body->linearDamping = 0.80;
    c2.body->angularDamping = 0.3;
    c2.body->linearDamping = 0.70;

    RigidBodyGravity g(Vector3D(0, -10, 0));

    RigidBody fixed1;
    fixed1.position = Vector3D(200, 200, -200);

    RigidBodySpringForce force1(c1.localVertices[0], &fixed1, Vector3D(), 6, 100);

    ParticleGravity pg(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
           
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            // Moves camera
             // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                cameraDistance *= 0.98;
                cameraPosition *= 0.98;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                cameraDistance *= 1.02;
                cameraPosition *= 1.02;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        c1.body->calculateDerivedData();
        c3.body->calculateDerivedData();
        c4.body->calculateDerivedData();
        c2.body->calculateDerivedData();

        fixed1.calculateDerivedData();

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            g.updateForce(c1.body, substep);
            g.updateForce(c4.body, substep);
            g.updateForce(c3.body, substep);
            g.updateForce(c2.body, substep);

            //force1.updateForce(c1.body, substep);

            // joint.update(substep);

            std::vector<Contact> contacts;
            joint.addContact(contacts);
            joint2.addContact(contacts);
            joint3.addContact(contacts);

            CollisionResolver resolver(10, 1, 0.01, 0.01);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            c1.body->integrate(substep);
            c4.body->integrate(substep);
            c3.body->integrate(substep);
            c2.body->integrate(substep);
        }

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        Vector3D vec(
            worldPos.x - c1.body->position.x,
            worldPos.y - c1.body->position.y,
            worldPos.x - c1.body->position.z
        );
        c1.body->position += vec;
     

        c1.update();
        c4.update();
        c3.update();
        c2.update();

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
        glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
        glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
        glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
        glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
        glm::vec4 colorGrey(0.7, 0.7, 0.7, 1.0);

        // Shape
        glm::vec3 lightPos[]{
            glm::vec3(0.0f, 100.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

        // Data
        FaceData data = getFaceData(c1);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorPurple, 1, lightPos,
            lightColors, cameraPosition, 0.05, 0.1
        );

        data = getFaceData(c4);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 40
        );

        data = getFaceData(c3);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorRed, 1, lightPos,
            lightColors, cameraPosition, 40
        );

        data = getFaceData(c2);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorBlue, 1, lightPos,
            lightColors, cameraPosition, 40
        );

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_5

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    glFrontFace(GL_CCW);
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is
    // definitely best not to render both sides (enable culling) so it
    // appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

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
        glm::vec3(200.0f, 300.0f, 0.0f),
        glm::vec3(200.0f, 900.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    glm::vec4 colors[9]{
        colorRed,
        colorBlue,
        colorPurple,
        colorGreen,
        colorMagenta,
        colorYellow,
        colorOrange,
        colorDarkBlue,
        colorGrey
    };

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    AnisotropicShader aniShader;
    TextureShader texShader;
    CookTorranceTextureShader cookTexShader;
    AnisotropicTextureShader aniTexShader;

    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 1000.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 0.0f, 0.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,
        cameraTarget, upVector);

    // Projection matrix, used for perspective
    // Field of View (FOV) in degrees
    real fov = 90.0f;
    // Aspect ratio
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
    // Near and far clipping planes
    real nearPlane = 0.1f;
    real farPlane = 10000.0f;

    // Create a perspective projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),
        aspectRatio, nearPlane, farPlane);

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(1200, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.05;

    std::vector<Polyhedron*> prisms;
    RectangularPrism* prism = new RectangularPrism(
        400, 10, 400, 100,
        Vector3D(0, 600, 0),
        new RigidBody
    );

    prism->body->angularDamping = 0.97;
    prism->body->linearDamping = 0.97;

    prism->body->canSleep = true;

    prisms.push_back(prism);

    RectangularPrism ground(10000, 100, 10000, 0, Vector3D(0, -350, -0), new RigidBody);
    ground.body->inverseMass = 0;

    RigidBodyGravity g(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false
    };

    bool contact = false;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                isButtonPressed[0] = true;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = isButtonPressed[1] =
                    isButtonPressed[2] = isButtonPressed[3] = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            // Moves camera
             // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                cameraDistance *= 0.98;
                cameraPosition *= 0.98;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                cameraDistance *= 1.02;
                cameraPosition *= 1.02;
                viewMatrix =
                    glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        int numSteps = 10;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (Polyhedron* prism : prisms) {
                prism->body->calculateDerivedData();
            }
            ground.body->calculateDerivedData();

            for (Polyhedron* prism : prisms) {
                g.updateForce(prism->body, substep);
            }

            std::vector<Contact> contacts;

            for (int i = 0; i < prisms.size(); i++) {
                generateContactBoxAndBox(*(prisms[i]), ground, contacts, 0.0, 0.0);
                for (int j = 0; j < prisms.size(); j++) {
                    if (i != j) {
                        generateContactBoxAndBox(*(prisms[i]), *(prisms[j]), contacts, 0.0, 0.0);
                    }
                }
            }

            if (contacts.size() > 0 && !contact) {
                RectangularPrism* p = ((RectangularPrism*)prisms[0]);
                prisms.clear();
                // We use deltaT even when substepping
                Vector3D dimensionPoint(p->width / 8.0, -p->height / 2.0, p->depth / 16.0);
                Vector3D point(-p->width / 5.0, 0.0, p->depth / 8.0);
                p->breakObject(prisms, contacts[0], deltaT, 2500, dimensionPoint, point);
                contact = true;
            }

            CollisionResolver resolver(1, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);


            for (Polyhedron* prism : prisms) {
                prism->body->integrate(substep);
                prism->update();
            }
            ground.body->integrate(substep);
            ground.update();

        }


        if (isButtonPressed[0]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            prisms[0]->body->position.x = worldPos.x * 3;
            prisms[0]->body->position.y = worldPos.y * 3;
            prisms[0]->body->position.z = worldPos.x * 3;
            prisms[0]->body->setAwake(true);
        }


        window.clear(sf::Color::Color(50, 50, 50));
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       

        FaceData data;
        for (int i = 0; i < prisms.size(); i++) {
            data = getFaceData(*prisms[i]);
            cookShader.drawFaces(data.vertices, data.normals,
                // Use i%9 for different colors
                identity, viewMatrix, projectionMatrix, colors[0], 2, lightPos,
                lightColors, cameraPosition, 0.1, 0.05
            );
        }

        data = getFaceData(ground);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorWhite, 2, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_6

int main() {

    GlfwWindowWrapper window(1200, 800, 6, "window");

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 200.0f, 500.0f),
        glm::vec3(0.0f, 200.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.001,
        0.005
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
        glm::vec3(200.0f, 1500.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };
    
    // Shaders
    std::vector<CookTorranceShader> cubeShaders(9);
    CookTorranceShader sphereShader;
    ShadowMappingShader groundShader;
    SolidColorShader lineShader;

    real mass = 1.5;

    std::vector<RectangularPrism*> prisms;
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {

                int index = i * 9 + j * 3 + k;

                real x = i * 200 - 200;
                real y = j * 200 - 200;
                real z = k * 200 - 200;

                RectangularPrism* prism = new RectangularPrism(
                    200, 200, 200, mass, Vector3D(x, y, z), new RigidBody
                );
                prism->body->angularDamping = 0.8;
                prism->body->linearDamping = 0.95;

                prisms.push_back(prism);

                FaceData data = getFaceData(*prism);
                std::vector<std::vector<glm::vec3>> vertices{
                    data.vertices, data.normals
                };

                cubeShaders[index].sendVaribleData(vertices, GL_STATIC_DRAW);
                cubeShaders[index].setTrianglesNumber(data.vertices.size());
                cubeShaders[index].setLightPosition(lightPos);
                cubeShaders[index].setLightColors(lightColors);
                cubeShaders[index].setFresnel(0.05);
                cubeShaders[index].setRoughness(0.5);
                cubeShaders[index].setObjectColor(colorPurple);
                cubeShaders[index].setActiveLightsCount(1);
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
    groundShader.setLightPosition(lightPos[0]);
    groundShader.setObjectColor(colorWhite);
    groundShader.setShadowStrength(1);
    groundShader.setPCF(true);

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
    sphereShader.setLightPosition(lightPos);
    sphereShader.setLightColors(lightColors);
    sphereShader.setFresnel(0.05);
    sphereShader.setRoughness(0.5);
    sphereShader.setObjectColor(colorRed);
    sphereShader.setActiveLightsCount(1);

    lineShader.setObjectColor(colorWhite);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    RigidBody b;
    b.position = Vector3D(800, 700, 0);
    RigidBodySpringForce f(sphere.localVertices[0], &b, Vector3D(), 0.09, 300);

    DepthMapper depthMapper(512, 512);

    DirectionalProjection projection(
        lightPos[0], 2000.0f, window.getWidth(), window.getHeight(), 0.1f, 5000.0f
    );

    float deltaT = 0.012;

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
            glm::vec2 worldPos = window.getCursorPosition();
            sphere.body->position.x = worldPos.x * 4;
            sphere.body->position.y = worldPos.y * 4;
            sphere.body->setAwake(true);
        }

        int numSteps = 10;
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
                    if (i != j) {
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


        std::vector<Shader*> shaders {
            &sphereShader     
        };
        for (int i = 0; i < prisms.size(); i++) {
            shaders.push_back(&cubeShaders[i]);
        }
        depthMapper.captureDepth(
            projection.getView(), projection.getProjection(), shaders
        );


        for (int i = 0; i < prisms.size(); i++) {
            cubeShaders[i].setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i].setViewMatrix(camera.getViewMatrix());
        }

        groundShader.setProjectionMatrix(camera.getProjectionMatrix());
        groundShader.setViewMatrix(camera.getViewMatrix());
        groundShader.setLightSpaceMatrix(projection.getProjectionView());
        groundShader.setShadowMap(depthMapper.getTexture());

        sphereShader.setProjectionMatrix(camera.getProjectionMatrix());
        sphereShader.setViewMatrix(camera.getViewMatrix());

        std::vector<std::vector<glm::vec3>> spring{ {
            convertToGLM(b.position),
            convertToGLM(sphere.getTransformMatrix().transform(sphere.localVertices[0]))
        }};
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

    return 0;
}

#endif

#ifdef SIM_7

int main() {

    GlfwWindowWrapper window(1200, 800, 6, "window");

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
    glm::vec4 colorBlack(0.1, 0.1, 0.1, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(30.0f, 400.0f, 30.0f),
        glm::vec3(100.0f, 40.0f, 30.0f),
        glm::vec3(30.0f, 40.0f, 100.0f),
        glm::vec3(-100.0f, 40.0f, 30.0f),
        glm::vec3(30.0f, 40.0f, -1000.0f),
        glm::vec3(100.0f, 40.0f, 100.0f),
        glm::vec3(-100.0f, 40.0f, 100.0f),
        glm::vec3(-100.0f, 40.0f, -100.0f),
        glm::vec3(1000.0f, 40.0f, -100.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    // Shaders
    DiffuseLightingTextureShader texShader;


    FirstPersonCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 500.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.2,
        0.0005
    );


    GLuint texture1 = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_branch.png"
    );
    GLuint opacity = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\tree\\opacity.png"
    );

    std::string filename = "C:\\Users\\msaba\\Documents\\physen\\spruce\\object_branch.obj";
    Polyhedron p = returnPrimitive(filename, 1, Vector3D::ZERO, new RigidBody(), 100);
    p.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D::FORWARD, PI/2);

    FaceData data = getFaceData(p);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals, data.uvCoordinates
    };
    texShader.sendVaribleData(d, GL_STATIC_DRAW);
    texShader.setActiveLightsCount(1);
    texShader.setLightPosition(lightPos, 1);
    texShader.setTrianglesNumber(data.vertices.size());

    texShader.setObjectTexture(texture1);

    // texShader.setNoLight(true);

    float deltaT = 0.001;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    bool isPressed = false;

    p.body->calculateDerivedData();

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

        texShader.setModelMatrix(convertToGLM(p.body->transformMatrix));
        texShader.setViewMatrix(camera.getViewMatrix());
        texShader.setProjectionMatrix(camera.getProjectionMatrix());

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            texShader.drawFaces();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }

    }

    return 0;
}

#endif

#ifdef SIM_8

int main() {


    Order defaultEngineOrder = Order::COUNTER_CLOCKWISE;

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    if (defaultEngineOrder == Order::COUNTER_CLOCKWISE) {
        glFrontFace(GL_CCW);
    }
    else {
        glFrontFace(GL_CW);
    }
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is definitely
    // best not to render both sides (enable culling) so it appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    TextureShader texShader;

    GLuint texture = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\blue.jpg");

    RotatingCamera camera(
        window,
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        500,
        0.02,
        0.1
    );

    sf::Clock clock;
    real deltaT = 0.07;

    int size = 20;
    real strength = 0.5;
    real mass = 0.5;
    real damping = 0.5;

    Cloth mesh(
        Vector3D(-200, 200, 0),
        Vector3D(200, -200, 0),
        size, size,
        mass, damping, strength
    );

    SolidSphere c(100, 100, 20, 20, Vector3D(0, 0, 400), new RigidBody);

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        mesh.particles[i]->setAwake(false);
    }

    ParticleGravity g(Vector3D(0, -10, 0));
    RigidBodyGravity g2(Vector3D(0, -10, 0));

    real rotationSpeed = 0.10;
    real angle = PI / 2;
    bool isButtonPressed[2]{ false , false };


    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left) {
                isButtonPressed[0] = false;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                isButtonPressed[1] = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Right) {
                isButtonPressed[1] = false;
            }

            camera.update(event, deltaT);
        }

        int numSteps = 10;
        real substep = deltaT / numSteps;

        while (numSteps--) {
            
            c.body->calculateDerivedData();

            g2.updateForce(c.body, substep);

            for (auto& particle : mesh.particles) {
                g.updateForce(particle, deltaT);
            }

            for (auto& force : mesh.forces) {
                force.force1.updateForce(force.force2.otherParticle, deltaT);
                force.force2.updateForce(force.force1.otherParticle, deltaT);
            }

            if (isButtonPressed[0] || isButtonPressed[1]) {

                if (isButtonPressed[0]) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                    Vector3D move;
                    move.x = worldPos.x - mesh.particles[size * size / 2]->position.x;
                    move.y = worldPos.y - mesh.particles[size * size / 2]->position.y;
                    move.z = worldPos.x - mesh.particles[size * size / 2]->position.z;

                    move *= mass;
                    ParticleGravity f(move);

                    for (int i = 0; i < size; i++) {
                        f.updateForce(mesh.particles[size + i * size - 1], deltaT);
                    }
                }
                else if (isButtonPressed[1]) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                    Vector3D move;
                    move.x = worldPos.x - mesh.particles[size * size / 2]->position.x;
                    move.y = worldPos.y - mesh.particles[size * size / 2]->position.y;
                    move.z = -(worldPos.x + mesh.particles[size * size / 2]->position.z);

                    move *= mass;
                    ParticleGravity f(move);

                    for (int i = 0; i < size; i++) {
                        f.updateForce(mesh.particles[i * size], deltaT);
                    }
                }
            }

            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c.body->position = {
                c.body->position.x,
                worldPos.y,
                worldPos.x
            };

            
            ParticleContactResolver r;
            vector<ParticleContact> contacts;
            for (Particle* particle : mesh.particles) {
                generateContactParticleAndSphere(particle, c, contacts, 0.0);
            }
            r.setIterations(contacts.size() * 1);
            if (contacts.size()) {
                r.resolveContacts(contacts.data(), contacts.size(), substep);
            }


            mesh.applyConstraints();

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i]->isAwake) {
                    mesh.particles[i]->verletIntegrate(deltaT);
                }
            }

            c.body->integrate(substep);
            mesh.update();
            c.update();
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
        glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
        glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

        //shader.drawEdges(edgeData.vertices, identity, viewMatrix, 
          //   projectionMatrix, colorWhite);

        FaceData data = getTwoSidedFaceData(mesh);
        EdgeData edata = getEdgeData(mesh);
        FrameVectors fdata = getFrameVectors(mesh, 30);

        glm::vec3 lightPos[]{ glm::vec3(500, 0, 500), glm::vec3(-500, 0, -500) };
        glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
            glm::vec4(1.0, 1.0, 1.0, 1.0) };

        lightShader.drawFaces(
            data.vertices, data.normals,
            identity, camera.getViewMatrix(),
            camera.getProjectionMatrix(), colorRed, 2, lightPos,
            lightColor
        );

        data = getFaceData(c);
        cookShader.drawFaces(
            data.vertices, data.normals,
            identity, camera.getViewMatrix(),
            camera.getProjectionMatrix(), colorBlue, 2, lightPos,
            lightColor, camera.getPosition(), 0.1, 0.05
        );


        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_9

int main() {


    Order defaultEngineOrder = Order::COUNTER_CLOCKWISE;

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    if (defaultEngineOrder == Order::COUNTER_CLOCKWISE) {
        glFrontFace(GL_CCW);
    }
    else {
        glFrontFace(GL_CW);
    }
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is definitely
    // best not to render both sides (enable culling) so it appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    TextureShader texShader;

    GLuint texture = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\blue.jpg");

    RotatingCamera camera(
        window,
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        500,
        0.02,
        0.1
    );

    sf::Clock clock;
    real deltaT = 0.065;

    int size = 10;
    real strength = 0.5;
    real mass = 0.5;
    real damping = 0.5;

    Blob mesh(
        Vector3D(0, 0, 0),
        100,
        size, size,
        mass, damping, strength
    );

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        // mesh.particles[i]->setAwake(false);
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    real rotationSpeed = 0.10;
    real angle = PI / 2;
    bool isButtonPressed[2]{ false , false };


    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left) {
                isButtonPressed[0] = false;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                isButtonPressed[1] = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Right) {
                isButtonPressed[1] = false;
            }

            camera.update(event, deltaT);
        }

        int numSteps = 20;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (auto& particle : mesh.particles) {
                // g.updateForce(particle, deltaT);
            }

            vector<ParticleContact> contacts;
            for (auto& force : mesh.forces) {
                force.force1.updateForce(force.force2.otherParticle, deltaT);
                force.force2.updateForce(force.force1.otherParticle, deltaT);
            }

            if (isButtonPressed[0] || isButtonPressed[1]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                Vector3D move;
                move.x = worldPos.x - mesh.particles[size * size / 2]->position.x;
                move.y = worldPos.y - mesh.particles[size * size / 2]->position.y;
                move.z = worldPos.x - mesh.particles[size * size / 2]->position.z;

                if (isButtonPressed[0]) {
                    for (int i = 0; i < mesh.particles.size()/2; i++) {
                        mesh.particles[i]->position += move;
                    }
                }
                else if (isButtonPressed[1]) {
                    for (int i = 0; i < mesh.particles.size()/2; i++) {
                        mesh.particles[i]->position += move;
                    }
                }
            }

            //mesh.applyConstraints();

            for (int i = 0; i < mesh.particles.size(); i++) {
                mesh.particles[i]->verletIntegrate(deltaT);
            }

            mesh.update();
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
        glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
        glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);

        //shader.drawEdges(edgeData.vertices, identity, viewMatrix, 
          //   projectionMatrix, colorWhite);

        FaceData data = getTwoSidedFaceData(mesh);
        EdgeData edata = getEdgeData(mesh);
        FrameVectors fdata = getFrameVectors(mesh, 30);

        glm::vec3 lightPos[]{ glm::vec3(500, 0, 500), glm::vec3(-500, 0, -500) };
        glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
            glm::vec4(1.0, 1.0, 1.0, 1.0) };


        /*
        lightShader.drawFaces(
            data.vertices,
            data.normals,
            identity,
            camera.getViewMatrix(),
            camera.getProjectionMatrix(),
            colorRed,
            2,
            lightPos,
            lightColor
        );
        */

        shader.drawEdges(
            edata.vertices,
            identity,
            camera.getViewMatrix(),
            camera.getProjectionMatrix(),
            colorWhite
        );
        


        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_10

int main() {


    Order defaultEngineOrder = Order::COUNTER_CLOCKWISE;

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    if (defaultEngineOrder == Order::COUNTER_CLOCKWISE) {
        glFrontFace(GL_CCW);
    }
    else {
        glFrontFace(GL_CW);
    }
    // This only displays faces from one side, depending on the order of
    // vertices, and what is considered front facce in the above option.
    // Disable to show both faces (but lose on performance).
    // Set to off in case our faces are both clockwise and counter clockwise
    // (mixed), so we can't consisently render only one.
    // Note that if we have opacity of face under 1 (opaque), it is definitely
    // best not to render both sides (enable culling) so it appears correct.
    glEnable(GL_CULL_FACE);

    // Enables blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;
    CookTorranceShader cookShader;
    CookTorranceTextureShader cookTexShader;
    CookTorranceReflectionShader refShader;
    SkyboxShader skyboxShader;
    CookTorranceReflectionShaderWithSkybox refShader2;

    GLuint texture = loadTexture("C:\\Users\\msaba\\Documents\\textureMaps\\blue.jpg");
    GLuint skybox = loadCubemap(std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\cubemaps\\right.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\left.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\top.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\bottom.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\front.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\back.jpg"
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

    RotatingCamera camera(
        window,
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        500,
        0.02,
        0.1
    );

    sf::Clock clock;
    real deltaT = 0.07;

    RectangularPrism c(100, 100, 100, 20, Vector3D(0, 0, 400), new RigidBody);

    real radius = 150;
    std::string filename = "C:\\Users\\msaba\\Documents\\textureMaps\\moai.obj";
    Polyhedron c2 = returnPrimitive(filename, 1, Vector3D::ZERO, new RigidBody(), 2);
    c2.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), PI / 2.0);

    skyboxShader.setSkybox(skybox);
    skyboxShader.setModelScale(2000);

    FaceData data = getFaceData(c2);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals
    };
    refShader2.sendVaribleData(d);
    refShader2.setTrianglesNumber(data.vertices.size());
    refShader2.setActiveLightsCount(2);
    refShader2.setLightPosition(lightPos);
    refShader2.setLightColors(lightColor);
    refShader2.setBaseColor(colorWhite);
    refShader2.setRoughness(0.05);
    refShader2.setFresnel(0.5);
    refShader2.setSkybox(skybox);
    refShader2.setLightInfluence(0.0);
    refShader2.setReflectionStrength(1.0);

    data = getFaceData(c);
    d = {
        data.vertices, data.normals
    };
    cookShader.sendVaribleData(d);
    cookShader.setTrianglesNumber(data.vertices.size());
    cookShader.setActiveLightsCount(2);
    cookShader.setLightPosition(lightPos);
    cookShader.setLightColors(lightColor);
    cookShader.setObjectColor(colorRed);
    cookShader.setRoughness(0.05);
    cookShader.setFresnel(0.5);

    EnvironmentMapper mapper(512, 512);

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            camera.update(event, deltaT);
        }

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();


        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        c.body->position = {
            c.body->position.x,
            worldPos.y,
            worldPos.x
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

        /*
            When we are done, we set the active texture back to the one at
            index 0 (this must be done everytime we set an active texture).
        */
        glActiveTexture(GL_TEXTURE0);

        refShader2.setEnvironmentMap(mapper.getTexture());

        cookShader.setViewMatrix(camera.getViewMatrix());
        cookShader.setProjectionMatrix(camera.getProjectionMatrix());

        skyboxShader.setViewMatrix(camera.getViewMatrix());
        skyboxShader.setProjectionMatrix(camera.getProjectionMatrix());

        // Unbind framebuffer to render to default framebuffer (window)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clear default framebuffer (window)
        window.clear(sf::Color::Color(0, 0, 0));
        glViewport(0, 0, 800, 800);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cookShader.drawFaces();
        skyboxShader.drawFaces();
        refShader2.drawFaces();

        // Display the rendered scene
        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_11

int main() {

    GlfwWindowWrapper window(800, 800, 6, "window");

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        500,
        0.0001,
        0.001
    );

    CookTorranceShader cookShader;
    ShadowMappingShader cookShader2;
    SkyboxShader skyboxShader;
    SimpleShader simpleShader;
    SimpleShader simpleShader2;

    GLuint texture = loadTexture(
        "C:\\Users\\msaba\\Documents\\textureMaps\\blue.jpg"
    );
    GLuint skybox = loadCubemap(
        std::vector<std::string>{
            "C:\\Users\\msaba\\Documents\\cubemaps\\right.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\left.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\top.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\bottom.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\front.jpg",
            "C:\\Users\\msaba\\Documents\\cubemaps\\back.jpg"
        }
    );

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
    glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
    glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
    glm::vec4 colorGrey(0.35, 0.35, 0.35, 1);
    glm::vec4 colorTrans(0, 0, 0, 1.0);

    glm::vec3 lightPos[]{ glm::vec3(0, 0, -500) };
    glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0) };

    RectangularPrism c(50, 50, 50, 20, Vector3D(0, 0, 400), new RigidBody);

    real radius = 150;
    std::string filename = "C:\\Users\\msaba\\Documents\\textureMaps\\moai.obj";
    Polyhedron c2 = returnPrimitive(filename, 1, Vector3D::ZERO, new RigidBody(), 2);
    c2.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), PI / 2.0);

    skyboxShader.setSkybox(skybox);
    skyboxShader.setModelScale(2000);

    FaceData data = getFaceData(c2);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices
    };
    simpleShader2.sendVaribleData(d, GL_STATIC_DRAW);
    simpleShader2.setTrianglesNumber(data.vertices.size());
    d = {
        data.vertices, data.normals
    };
    cookShader2.sendVaribleData(d, GL_STATIC_DRAW);
    cookShader2.setTrianglesNumber(data.vertices.size());
    cookShader2.setShadowStrength(0.8);
    cookShader2.setLightPosition(lightPos[0]);
    cookShader2.setObjectColor(colorGrey);

    data = getFaceData(c);
    d = {
        data.vertices
    };
    simpleShader.sendVaribleData(d, GL_STATIC_DRAW);
    simpleShader.setTrianglesNumber(data.vertices.size());
    d = {
        data.vertices, data.normals
    };
    cookShader.sendVaribleData(d, GL_STATIC_DRAW);
    cookShader.setTrianglesNumber(data.vertices.size());
    cookShader.setActiveLightsCount(1);
    cookShader.setLightPosition(lightPos);
    cookShader.setLightColors(lightColor);
    cookShader.setObjectColor(colorRed);
    cookShader.setRoughness(0.05);
    cookShader.setFresnel(0.5);

    DepthMapper depthMapper(512, 512);

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

        glm::vec2 position = window.getCursorPosition();
        c.body->position = {
            c.body->position.x,
            position.y,
            -position.x
        };

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0, 0.0, -500),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        float near_plane = 1.0f, far_plane = 1000.0f;
        glm::mat4 projection = glm::ortho(
            -200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane
        );

        simpleShader.setModelMatrix(convertToGLM(c.body->transformMatrix));
        simpleShader2.setModelMatrix(convertToGLM(c2.body->transformMatrix));
        std::vector<Shader*> shaders{
            &simpleShader, &simpleShader2
        };

        depthMapper.captureDepth(view, projection, shaders);

        glm::mat4 viewM = camera.getViewMatrix();
        glm::mat4 projectionM = camera.getProjectionMatrix();

        cookShader2.setModelMatrix(convertToGLM(c2.body->transformMatrix));
        cookShader2.setLightSpaceMatrix(projection * view);
        cookShader2.setShadowMap(depthMapper.getTexture());
        cookShader2.setViewMatrix(viewM);
        cookShader2.setProjectionMatrix(projectionM);

        cookShader.setModelMatrix(convertToGLM(c.body->transformMatrix));
        cookShader.setViewMatrix(viewM);
        cookShader.setProjectionMatrix(projectionM);

        skyboxShader.setViewMatrix(viewM);
        skyboxShader.setProjectionMatrix(projectionM);

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cookShader2.drawFaces();
            cookShader.drawFaces();
            skyboxShader.drawFaces();

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
    }

    return 0;
}


#endif

#ifdef SIM_12

int main() {

    GlfwWindowWrapper window(1200, 800, 6, "window", true);

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 200.0f, 500.0f),
        glm::vec3(0.0f, 200.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.001,
        0.005
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
        glm::vec3(200.0f, 1500.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    // Shaders
    std::vector<CookTorranceShader> cubeShaders(9);
    CookTorranceShader sphereShader;
    ShadowMappingShader groundShader;
    SolidColorShader lineShader;

    real mass = 1.5;

    std::vector<RectangularPrism*> prisms;
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {

                int index = i * 9 + j * 3 + k;

                real x = i * 200 - 200;
                real y = j * 200 - 200;
                real z = k * 200 - 200;

                RectangularPrism* prism = new RectangularPrism(
                    200, 200, 200, mass, Vector3D(x, y, z), new RigidBody
                );
                prism->body->angularDamping = 0.8;
                prism->body->linearDamping = 0.95;

                prisms.push_back(prism);

                FaceData data = getFaceData(*prism);
                std::vector<std::vector<glm::vec3>> vertices{
                    data.vertices, data.normals
                };

                cubeShaders[index].sendVaribleData(vertices, GL_STATIC_DRAW);
                cubeShaders[index].setTrianglesNumber(data.vertices.size());
                cubeShaders[index].setLightPosition(lightPos);
                cubeShaders[index].setLightColors(lightColors);
                cubeShaders[index].setFresnel(0.05);
                cubeShaders[index].setRoughness(0.5);
                cubeShaders[index].setObjectColor(colorPurple);
                cubeShaders[index].setActiveLightsCount(1);
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
    groundShader.setLightPosition(lightPos[0]);
    groundShader.setObjectColor(colorWhite);
    groundShader.setShadowStrength(1);
    groundShader.setPCF(true);

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
    sphereShader.setLightPosition(lightPos);
    sphereShader.setLightColors(lightColors);
    sphereShader.setFresnel(0.05);
    sphereShader.setRoughness(0.5);
    sphereShader.setObjectColor(colorRed);
    sphereShader.setActiveLightsCount(1);

    lineShader.setObjectColor(colorWhite);

    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    RigidBody b;
    b.position = Vector3D(800, 700, 0);
    RigidBodySpringForce f(sphere.localVertices[0], &b, Vector3D(), 0.09, 300);


    // Coarse Collision
    BoundingVolumeHierarchy<BoundingSphere> tree;
    for (RectangularPrism* prism : prisms) {
        tree.insert(
            prism,
            BoundingSphere(prism->boundingSphereOffset, prism->boundingSphereRadius)
        );
    }
    tree.insert(
        &sphere,
        BoundingSphere(Vector3D::ZERO, sphere.radius)
    );
    tree.insert(
        &ground,
        BoundingSphere(ground.boundingSphereOffset, ground.boundingSphereRadius)
    );


    DepthMapper depthMapper(512, 512);

    DirectionalProjection projection(
        lightPos[0], 2000.0f, window.getWidth(), window.getHeight(), 0.1f, 5000.0f
    );

    float deltaT = 0.012;

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
            glm::vec2 worldPos = window.getCursorPosition();
            sphere.body->position.x = worldPos.x * 4;
            sphere.body->position.y = worldPos.y * 4;
            sphere.body->setAwake(true);
        }

        int numSteps = 10;
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

            // Coarse Collision
            BoundingVolumeHierarchy<BoundingSphere> tree;
            for (RectangularPrism* prism : prisms) {
                tree.insert(
                    prism,
                    BoundingSphere(
                        prism->getCentre(),
                        prism->getFurthestPoint().magnitude()
                    )
                );
            }
            tree.insert(
                &sphere,
                BoundingSphere(sphere.getCentre(), sphere.radius)
            );
            tree.insert(
                &ground,
                BoundingSphere(
                    ground.getCentre(),
                    ground.getFurthestPoint().magnitude()
                )
            );

            PotentialContact* potentialContacts = new PotentialContact[1000];
            int n = tree.getRoot()->getPotentialContacts(potentialContacts, 1000);

            std::cout << n << "\n";

            for (int i = 0; i < n; i++) {
                PotentialContact p = potentialContacts[i];
                if (p.polyhedron[0] == &sphere) {
                    generateContactBoxAndSphere(*p.polyhedron[1], *p.polyhedron[0], contacts, 0.25, 0.0);
                }
                else if (p.polyhedron[1] == &sphere) {
                    generateContactBoxAndSphere(*p.polyhedron[0], *p.polyhedron[1], contacts, 0.25, 0.0);
                }
                else {
                    generateContactBoxAndBox(*p.polyhedron[0], *p.polyhedron[1], contacts, 0.25, 0.0);
                }
            }

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


        std::vector<Shader*> shaders{
            &sphereShader
        };
        for (int i = 0; i < prisms.size(); i++) {
            shaders.push_back(&cubeShaders[i]);
        }
        depthMapper.captureDepth(
            projection.getView(), projection.getProjection(), shaders
        );


        for (int i = 0; i < prisms.size(); i++) {
            cubeShaders[i].setProjectionMatrix(camera.getProjectionMatrix());
            cubeShaders[i].setViewMatrix(camera.getViewMatrix());
        }

        groundShader.setProjectionMatrix(camera.getProjectionMatrix());
        groundShader.setViewMatrix(camera.getViewMatrix());
        groundShader.setLightSpaceMatrix(projection.getProjectionView());
        groundShader.setShadowMap(depthMapper.getTexture());

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

    return 0;
}

#endif

#ifdef SIM_13

int main() {

    GlfwWindowWrapper window(1200, 800, 6, "window");

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
    sphereShader.setFresnel(0.05);
    sphereShader.setRoughness(0.5);
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

    return 0;
}

#endif

#ifdef SIM_14

int main() {

    GlfwWindowWrapper window(1200, 800, 6, "window");

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
        cubeShaders[i].setLightPosition(lightPos);
        cubeShaders[i].setLightColors(lightColors);
        cubeShaders[i].setFresnel(0.05);
        cubeShaders[i].setRoughness(0.5);
        cubeShaders[i].setObjectColor(colorWhite);
        cubeShaders[i].setActiveLightsCount(1);
    }

    // Sphere

    SolidSphere sphere(60, 5, 20, 20, Vector3D(0, 0, 0), new RigidBody);

    FaceData data = getFaceData(sphere);
    std::vector<std::vector<glm::vec3>>vertices = {
        data.vertices, data.normals
    };
    sphereShader.sendVaribleData(vertices, GL_STATIC_DRAW);
    sphereShader.setTrianglesNumber(data.vertices.size());
    sphereShader.setLightPosition(lightPos);
    sphereShader.setLightColors(lightColors);
    sphereShader.setFresnel(0.05);
    sphereShader.setRoughness(0.5);
    sphereShader.setObjectColor(colorBlue);
    sphereShader.setActiveLightsCount(1);


    // Forces

    RigidBodyGravity g(Vector3D(0, -10, 0));

    float deltaT = 0.0005;

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
                int n = generateRandomNumber(-300, 300);
                int m = generateRandomNumber(-500, 500);
                // We can generate only 1 vertex for
                    // these spheres as they will be shaded later
                SolidSphere* v = new SolidSphere(60, 0.1, 1, 1, Vector3D(n, 1000, m), new RigidBody);
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
                    generateContactBoxAndSphere(*(walls[i]), *s, contacts, 0.8, 0.5);
                }
            }
            for (SolidSphere* s : spheres) {
                for (SolidSphere* s2 : spheres) {
                    if (
                        (s2->getCentre() - s->getCentre()).magnitudeSquared() <
                        (s2->radius + s->radius) * (s2->radius + s->radius)
                    ) {
                        generateContactSphereAndSphere(*s2, *s, contacts, 0.8, 0.5);
                    }
                }
            }


            CollisionResolver resolver(2, 0);
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

    return 0;
}

#endif

#ifdef SIM_15

struct Object {

    DiffuseLightingTextureShader texShader;
    SimpleShader shader;
    GLuint texture;

    Object(
        string object,
        int scale,
        Vector3D position,
        Vector3D rotationAxis,
        float angle,
        const glm::vec3* lightPos,
        int lightNumber,
        GLuint texture
    ) : texture{texture} {

        std::string filename =
            "C:\\Users\\msaba\\Documents\\physen\\" + object;
        Polyhedron p = (returnPrimitive(filename, 1, 
            position, new RigidBody(), scale));

        if (rotationAxis != Vector3D::ZERO) {
            p.body->orientation = Quaternion::rotatedByAxisAngle(
                rotationAxis, glm::radians(angle)
            );
            p.body->calculateDerivedData();
        }

        FaceData data = getFaceData(p);
        std::vector<std::vector<glm::vec3>> d = {
            data.vertices, data.normals, data.uvCoordinates
        };
        texShader.sendVaribleData(d, GL_STATIC_DRAW);
        texShader.setTrianglesNumber(data.vertices.size());
        texShader.setLightPosition(lightPos, lightNumber);

        glm::mat4 modelMatrix = glm::mat4(convertToGLM(p.getTransformMatrix()));
        texShader.setModelMatrix(modelMatrix);
        shader.setModelMatrix(modelMatrix);
    }



    Object(
        string object,
        float scale,
        Vector3D position,
        Quaternion q,
        const glm::vec3* lightPos,
        int lightNumber,
        GLuint texture
    ) : texture{ texture } {

        std::string filename =
            "C:\\Users\\msaba\\Documents\\physen\\" + object;
        Polyhedron p = (returnPrimitive(filename, 1,
            position, new RigidBody(), scale));

        p.body->orientation = q;
        p.body->calculateDerivedData();

        FaceData data = getFaceData(p);
        std::vector<std::vector<glm::vec3>> d = {
            data.vertices, data.normals, data.uvCoordinates
        };
        texShader.sendVaribleData(d, GL_STATIC_DRAW);
        texShader.setTrianglesNumber(data.vertices.size());
        texShader.setLightPosition(lightPos, lightNumber);

        glm::mat4 modelMatrix = glm::mat4(convertToGLM(p.getTransformMatrix()));
        texShader.setModelMatrix(modelMatrix);
        shader.setModelMatrix(modelMatrix);
    }


    void setTexture() {
        texShader.setObjectTexture(texture);
    }

    void setVP(
        const glm::mat4& view,
        const glm::mat4& projection
    ){
        texShader.setViewMatrix(view);
        texShader.setProjectionMatrix(projection);
    }

    void render() {
        texShader.drawFaces();
    }
};

int main() {

    GlfwWindowWrapper window(1920, 1080, 6, "window", false);

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
    glm::vec4 colorBlack(0.1, 0.1, 0.1, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(40.0f, 500.0f, -800.0f)
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    GLuint texture = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\mansion\\texture.jpg"
    );
    GLuint textureCabin = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\cabin\\texture.jpg"
    );
    GLuint textureBranch = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_branch.png"
    );
    GLuint textureTrunk = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_trunk.jpeg"
    );
    GLuint textureWatch = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\watchtower\\texture.png"
    );    
    GLuint textureLong = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\long_house\\texture.jpg"
    );
    GLuint textureFence = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\fence\\texture.png"
    );
    GLuint textureCart = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\cart\\texture.jpg"
    );
    GLuint textureWell = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\well\\texture.png"
    );
    GLuint textureGrass = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\grassLarge.jpg"
    );
    GLuint textureGrey = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\grey_house\\texture.png"
    );
    GLuint textureBlack = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\black_smith\\texture.jpg"
    );
    GLuint textureBarrel = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\barrel\\texture.png"
    );
    GLuint textureStoneHouse = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\stone_house\\texture.png"
    );



    GLuint skybox = loadCubemap(std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\right.jpg",
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\left.jpg",
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\up.jpg",
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\down.jpg",
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\front.jpg",
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\back.jpg",
    });

    FirstPersonCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, -40.0f,-1200.0f),
        glm::vec3(0.0f, -40.0f, 0.0f),
        90.0,
        0.1,
        10000,
        7,
        0.03
    );

    std::vector<Object*> objects{

        // Right trees

        new Object("spruce\\object_branch.obj", 40, Vector3D(-370, 150, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(-370, 150, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(-200, 150, -1200), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(-200, 150, -1200), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(-200, 150, -1050), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(-200, 150, -1050), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 60, Vector3D(-480, 210, -1150), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 60, Vector3D(-480, 210, -1150), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        // Left trees

        new Object("spruce\\object_branch.obj", 40, Vector3D(260, 150, -1200), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(260, 150, -1200), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(300, 150, -1000), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(300, 150, -1000), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(240, 150, -1350), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(240, 150, -1350), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(200, 150, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(200, 150, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 40, Vector3D(360, 150, -950), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(360, 150, -950), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        // Fences

        new Object("fence\\object.obj", 20, Vector3D(-80, -70, -1300), Vector3D(0, 1, 0), 180, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(80, -70, -1300), Vector3D(0, 1, 0), 190, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(-80, -70, -1200), Vector3D(0, 1, 0), 10, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(80, -70, -1200), Vector3D(0, 1, 0), 10, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(-70, -70, -1100), Vector3D(0, 1, 0), -20, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(90, -70, -1100), Vector3D(0, 1, 0), 170, lightPos, 1, textureFence),

        // Well

        new Object("well\\object.obj", 1, Vector3D(0, -40, -200), Quaternion(0.5, 0, 0.5, 0), lightPos, 1, textureWell),

        // Houses front

        new Object("cabin\\object.obj", 4, Vector3D(-250, 0, -800), Quaternion(0.5, 0, 0.5, 0), lightPos, 1, textureCabin),
        new Object("grey_house\\object.obj", 1.3, Vector3D(340, 50, -750), Quaternion(0.5, 0, 0.5, 0), lightPos, 1, textureGrey),
               
        new Object("cart\\object.obj", 30, Vector3D(110, -70, -1000), Quaternion(0.8, 0, 0.2, 0), lightPos, 1, textureCart),

        new Object("long_house\\object.obj", 40, Vector3D(610, 130, -450), Quaternion(0.6, 0, -0.3, 0), lightPos, 1, textureLong),

        // Trees

        new Object("spruce\\object_branch.obj", 40, Vector3D(350, 150, -520), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 40, Vector3D(350, 150, -520), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 60, Vector3D(650, 270, -880), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 60, Vector3D(650, 270, -880), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 70, Vector3D(800, 300, -640), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 70, Vector3D(800, 300, -640), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 60, Vector3D(-650, 270, -880), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 60, Vector3D(-650, 270, -880), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 60, Vector3D(-800, 270, -640), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 60, Vector3D(-800, 270, -640), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 70, Vector3D(-770, 300, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 70, Vector3D(-770, 300, -1100), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 70, Vector3D(-920, 300, -900), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 70, Vector3D(-920, 300, -900), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        new Object("spruce\\object_branch.obj", 60, Vector3D(-780, 280, -450), Vector3D(0, 0, 1), 90, lightPos, 1, textureBranch),
        new Object("spruce\\object_trunk.obj", 60, Vector3D(-780, 280, -450), Vector3D(0, 0, 1), 90, lightPos, 1, textureTrunk),

        // Houses 

        new Object("barrel\\object.obj", 2, Vector3D(-370, -65, -635), Quaternion(0.7, 0, 0.3, 0), lightPos, 1, textureBarrel),
        new Object("black_smith\\object.obj", 140, Vector3D(-450, -30, -450), Quaternion(0.7, 0, 0.3, 0), lightPos, 1, textureBlack),
        new Object("grey_house\\object.obj", 1.3, Vector3D(-650, 50, -100), Quaternion(-0.5, 0, -0.5, 0), lightPos, 1, textureGrey),

        new Object("cart\\object.obj", 30, Vector3D(-500, -70, -290), Quaternion(0.8, 0, 0.2, 0), lightPos, 1, textureCart),

        // Opposite side

    };

    RectangularPrism ground(6000, 10, 6000, 0, Vector3D(0, -100, 0), new RigidBody());
    FaceData data = getFaceData(ground);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals, data.uvCoordinates
    };

    ShadowMappingTextureShader groundShader;
    groundShader.sendVaribleData(d, GL_STATIC_DRAW);
    groundShader.setTrianglesNumber(data.vertices.size());
    groundShader.setLightPosition(lightPos[0]);
    groundShader.setPCF(true);
    groundShader.setModelMatrix(convertToGLM(ground.getTransformMatrix()));
    groundShader.setShadowStrength(0.9);

    SkyboxShader skyShader;
    skyShader.setSkybox(skybox);
    skyShader.setModelScaleAndTranslate(3000, glm::vec3(0, -40, 0));

    float deltaT = 0.001;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    bool isPressed = false;

    DirectionalProjection projection(lightPos[0], 1000, 800, 800, 0.1, 5000);
    DepthMapper mapper(512, 512);

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
            lightPos[0].z += 0.02;
            for (Object* o : objects) {
                o->texShader.setLightPosition(lightPos, 1);
            }
            groundShader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            lightPos[0].z -= 0.02;
            for (Object* o : objects) {
                o->texShader.setLightPosition(lightPos, 1);
            }
            groundShader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);
        }


        std::vector<Shader*> shaders;
        for (Object* o : objects) {
            shaders.push_back(&(o->shader));
        }
        mapper.captureDepth(projection.getView(), projection.getProjection(), shaders);
        
        //saveDepthMap(mapper.getTexture(), 512, 512, "C:\\Users\\msaba\\Downloads\\cubemap_faces\\p.png");

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 vm = camera.getViewMatrix();
            glm::mat4 pm = camera.getProjectionMatrix();

            groundShader.setViewMatrix(vm);
            groundShader.setProjectionMatrix(pm);
            groundShader.setObjectTexture(textureGrass);
            // groundShader.setShadowMap(mapper.getTexture());
            groundShader.setLightSpaceMatrix(projection.getProjectionView());
            groundShader.drawFaces();

            for (Object* o : objects) {
                o->setVP(vm, pm);
                o->setTexture();
                o->render();
            }

            skyShader.setViewMatrix(vm);
            skyShader.setProjectionMatrix(pm);
            skyShader.drawFaces();

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }

    }

    return 0;
}

#endif
