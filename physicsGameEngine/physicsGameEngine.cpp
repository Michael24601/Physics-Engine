
// For SFML and GLEW static version (no dlls)
#define SFML_STATIC
#define GLEW_STATIC

// Must be before any SFML or glfw or glm or glew files                        
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
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
#include "textureShader.h"
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


using namespace pe;
using namespace std;

#define SIM_10

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


    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\dirty-metal.jpg");
    GLuint texture2 = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\world.jpg");
    GLuint texture3 = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\wood.jpg");
    GLuint texture4 = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\coke.jpg");


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
        cookShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorRed, 1, lightPos,
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
        cookShader.drawFaces(d.vertices, d.normals, identity,
            viewMatrix, projectionMatrix, colorGrey, 1, lightPos,
            lightColors, cameraPosition, 0.05, 1
        );

        FaceData d2;
        for (int i = 2; i < c2.faces.size(); i++) {
            getFaceData(c2.faces[i], &d2);
        }
        cookShader.drawFaces(d2.vertices, d2.normals, identity,
            viewMatrix, projectionMatrix, colorYellow, 1, lightPos,
            lightColors, cameraPosition, 0.1, 1
        );
        

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_2

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

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\blue.jpg");

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

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        mesh.particles[i]->setAwake(false);
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    real rotationSpeed = 0.10;
    real angle = PI / 2;
    bool isButtonPressed[2] { false , false};


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
                g.updateForce(particle, deltaT);
            }

            vector<ParticleContact> contacts;
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

            mesh.applyConstraints();

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i]->isAwake) {
                    mesh.particles[i]->verletIntegrate(deltaT);
                }
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
        
        /*
        shader.drawEdges(
            edata.vertices,
            identity,
            viewMatrix,
            projectionMatrix,
            colorWhite
        );
        */
        

        window.display();
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

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\world.jpg");

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

    GLuint texture9 = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\stone.jpg");

    std::string filename = "C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\video_game.obj";
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

    real width = 1200, height = 800;

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Physics Simulation",
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
    CookTorranceReflectionShader refShader;


    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 1000.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 200.0f, 0.0f);
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
    real deltaT = 0.1;

    real mass = 1.5;

    std::vector<RectangularPrism*> prisms{
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, -200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 0, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, -200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 0, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 200, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, 0, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(0, -200, -205), new RigidBody),

        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, -200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 0, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, -200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 0, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 200, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, 0, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-205, -200, -205), new RigidBody),

        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, -200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 0, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 200, 0), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, -200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 0, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 200, 205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 200, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, 0, -205), new RigidBody),
        new RectangularPrism(200, 200, 200, mass, Vector3D(-405, -200, -205), new RigidBody)
    };

    RectangularPrism ground(10000, 100, 10000, 0, Vector3D(0, -350, -0), new RigidBody);
    ground.body->inverseMass = 0;

    SolidSphere s(200, 5, 20, 20, Vector3D(800, 400, 0), new RigidBody);
    s.body->canSleep = false;

    for (RectangularPrism* prism : prisms) {
        prism->body->angularDamping = 0.9;
        prism->body->linearDamping = 0.95;
        prism->body->canSleep = true;
    }
    s.body->angularDamping = 0.8;
    s.body->linearDamping = 0.95;

    // ground.body->orientation = Quaternion(1, 0.2, 1, 0.3).normalized();

    RigidBodyGravity g(Vector3D(0, -10, 0));

    RigidBody b;
    b.position = Vector3D(800, 700, 0);
    RigidBodySpringForce f(s.localVertices[0], &b, Vector3D(), 0.24, 300);

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

        int numSteps = 20;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (RectangularPrism* prism : prisms) {
                prism->body->calculateDerivedData();
            }
            ground.body->calculateDerivedData();
            s.body->calculateDerivedData();
            b.calculateDerivedData();

            for (RectangularPrism* prism : prisms) {
                g.updateForce(prism->body, substep);
            }
            g.updateForce(s.body, substep);
            f.updateForce(s.body, substep);

            std::vector<Contact> contacts;

            for (int i = 0; i < prisms.size(); i++) {
                generateContactBoxAndBox(*(prisms[i]), ground, contacts, 0.25, 0.0);
                generateContactBoxAndSphere(*(prisms[i]), s, contacts, 0.25, 0.0);
                for (int j = 0; j < prisms.size(); j++) {
                    if (i != j) {
                        generateContactBoxAndBox(*(prisms[i]), *(prisms[j]), contacts, 0.25, 0.0);
                    }
                }
            }
            generateContactBoxAndSphere(ground, s, contacts, 0.25, 0.0);

            CollisionResolver resolver(10, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            for (RectangularPrism* prism : prisms) {
                prism->body->integrate(substep);
                prism->update();
            }
            ground.body->integrate(substep);
            ground.update();
            s.body->integrate(substep);
            s.update();
        }

        if (isButtonPressed[0]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            s.body->position.x = worldPos.x * 4;
            s.body->position.y = worldPos.y * 4;
            //s.body->position.z = worldPos.x;
            s.body->setAwake(true);
        }

        window.clear(sf::Color::Color(50, 50, 50));
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



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
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

        // Data
        FaceData data;
        for (int i = 0; i < prisms.size(); i++) {
            data = getFaceData(*prisms[i]);
            cookShader.drawFaces(data.vertices, data.normals,
                identity, viewMatrix, projectionMatrix, colorPurple, 1, lightPos,
                lightColors, cameraPosition, 0.1, 0.05
            );
        }

        data = getFaceData(ground);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorWhite, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );

        data = getUniformFaceData(s);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorRed, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );

        EdgeData ed;
        ed.vertices.push_back(convertToGLM(b.position));
        ed.vertices.push_back(convertToGLM(s.globalVertices[0]));
        shader.drawEdges(ed.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite
        );

        window.display();
    }

    return 0;
}

#endif

#ifdef SIM_7

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
    glm::vec4 colorBlack(0.1, 0.1, 0.1, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(200.0f, 300.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
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


    RotatingCamera camera(
        window,
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        500,
        0.02,
        0.025
    );

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(1200, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.07;

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\scratched-metal.jpg");

    std::string filename = "C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\moai.obj";
    Polyhedron p = ReturnPrimitive(filename, 1, Vector3D::ZERO, new RigidBody(), 2);
    p.body->orientation = Quaternion::rotatedByAxisAngle(Vector3D(0, 0, 1), PI/2.0);


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
            camera.update(event, deltaT);
        }

        int numSteps = 1;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            p.body->calculateDerivedData();

            std::vector<Contact> contacts;

            CollisionResolver resolver(2, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            p.update();

        }


        window.clear(sf::Color::Color(0, 0, 0));
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        FaceData data;
        data = getFaceData(p);

        aniTexShader.drawFaces(data.vertices, data.normals,
            data.tangents, data.bitangents, data.uvCoordinates,
            identity, camera.getViewMatrix(), camera.getProjectionMatrix(),
            texture, colorWhite, colorBlack,
            glm::vec3(500.0f, 500.0f, 0.0f),
            colorWhite,
            camera.getPosition(),
            0.2, 0.2
        );
        
        /*
        cookTexShader.drawFaces(
            data.vertices, data.normals, data.uvCoordinates,
            identity, camera.getViewMatrix(), camera.getProjectionMatrix(),
            texture,
            1,
            lightPos,
            lightColors,
            camera.getPosition(),
            0.1, 0.5
        );
        
        
        phongShader.drawFaces(data.vertices, data.normals,
            identity, camera.getViewMatrix(), camera.getProjectionMatrix(),
            colorGrey,
            1,
            lightPos,
            lightColors,
            camera.getPosition(),
            10
        ); */

        
        
       EdgeData edata;
       FrameVectors vh = getUniformFrameVectors(p, 10);
       //shader.drawEdges(vh.tangents,
       //     identity, camera.getViewMatrix(), 
       //     camera.getProjectionMatrix(), colorWhite
       //);
        
        window.display();
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

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\blue.jpg");

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

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\blue.jpg");

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
    TextureShader texShader;
    CookTorranceReflectionShader refShader;

    GLuint texture = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\blue.jpg");

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
    SolidSphere c2(radius * 2, 10, 30, 30, Vector3D(0, 0, 400), new RigidBody);

    RigidBodyGravity g(Vector3D(0, -10, 0));

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
            c2.body->calculateDerivedData();

            if (isButtonPressed[0] || isButtonPressed[1]) {

                if (isButtonPressed[0]) { 
                }
                else if (isButtonPressed[1]) {
                }
            }

            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c.body->position = {
                c.body->position.x,
                worldPos.y,
                worldPos.x
            };

            c.body->integrate(substep);
            c.update();
            c2.body->integrate(substep);
            c2.update();
        }

        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(1.0, 0.2, 0.2, 1);
        glm::vec4 colorBlue(0.2, 0.2, 1.0, 1);
        glm::vec4 colorGreen(0.2, 1.0, 0.2, 1);
        glm::vec4 colorTrans(0, 0, 0, 1.0);


        glm::vec3 lightPos[]{ glm::vec3(0, 500, 0), glm::vec3(-500, 0, -500) };
        glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
            glm::vec4(1.0, 1.0, 1.0, 1.0) };


        FaceData data = getFaceData(c);

        GLuint cubemapTexture;
        glGenTextures(1, &cubemapTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Allocate storage for each face of the cubemap (should be same as window)
        int width = 800;
        int height = 800;
        for (GLuint face = 0; face < 6; ++face) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
                width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
            );
        }

        // Create and bind a framebuffer object
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Define the six directions for cubemap faces
        glm::vec3 directions[6] = {
            glm::vec3(1.0f, 0.0f, 0.0f),  // +X
            glm::vec3(-1.0f, 0.0f, 0.0f), // -X
            glm::vec3(0.0f, 1.0f, 0.0f),  // +Y
            glm::vec3(0.0f, -1.0f, 0.0f), // -Y
            glm::vec3(0.0f, 0.0f, 1.0f),  // +Z
            glm::vec3(0.0f, 0.0f, -1.0f)  // -Z
        };

        // Set up perspective projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 4000.0f);

        for (GLuint face = 0; face < 6; face++) {
            // Bind cubemap face as render target
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemapTexture, 0
            );

            glm::vec3 pos = convertToGLM(c2.body->position) + (directions[face] * (float)radius);

            // Set up view matrix for current face
            glm::mat4 view = glm::lookAt(
                pos,
                pos + directions[face],
                glm::vec3(0.0f, -1.0f, 0.0f)
            );

            // Set viewport for the current cubemap face
            glViewport(0, 0, width, height);

            // Clear the framebuffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cookShader.drawFaces(
                data.vertices, data.normals,
                identity, view,
                projection, colorBlue, 2, lightPos,
                lightColor, convertToGLM(c2.body->position), 0.1, 0.05
            );
        }

        // Unbind framebuffer to render to default framebuffer (window)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Save each face of the cubemap as an image file
       // saveCubemapFaces(
       //     cubemapTexture, width, height, 
       //     "C:\\Users\\msaba\\OneDrive\\Desktop\\cubemaps"
       // );

        // Clear default framebuffer (window)
        window.clear(sf::Color::Color(0, 0, 0));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cookShader.drawFaces(
            data.vertices, data.normals,
            identity, camera.getViewMatrix(),
            camera.getProjectionMatrix(), colorBlue, 2, lightPos,
            lightColor, camera.getPosition(), 0.1, 0.05
        );

        // Render the textured cube using the texture
        data = getFaceData(c2);
        refShader.drawFaces(
            data.vertices, data.normals, data.uvCoordinates,
            identity, camera.getViewMatrix(),
            camera.getProjectionMatrix(), cubemapTexture, colorRed,
            2, lightPos, lightColor, camera.getPosition(), 0.1, 0.05, 1.0
        );

        // Display the rendered scene
        window.display();

        // Cleanup: delete framebuffer and texture
        glDeleteTextures(1, &cubemapTexture);
        glDeleteFramebuffers(1, &framebuffer);
    }

    return 0;
}

#endif