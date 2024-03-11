
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

#include "rectangularCloth.h"

#include "fixedJoint.h"
#include "springJoint.h"
#include "joint.h"
#include "ballJoint.h"
#include "sphericalJoint.h"

#include "openglUtility.h"


using namespace pe;
using namespace std;


#define SIM_1

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
    RectangularPrism c1(side, side, side, 150, Vector3D(250, 0, -150));

    real height = 150;
    real radius = 50;
    Cylinder c2(radius, height, 150, 20, Vector3D(200, 0, 200));

    radius = 100;
    SolidSphere c3(radius, 150, 20, 20, Vector3D(-200, 0, -200));

    height = 150;
    side = 100;
    Pyramid c4(side, height, 150, Vector3D(-200, 0, 200));


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


    // Bounding volume hierarchy, used for coarse collision detection
    BoundingVolumeHierarchy<BoundingSphere> hierarchy;
    hierarchy.insert(&c1, c1.boundingSphere);
    hierarchy.insert(&c2, c2.boundingSphere);
    hierarchy.insert(&c3, c3.boundingSphere);
    hierarchy.insert(&c4, c4.boundingSphere);


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
            for (Contact& contact : contacts) {
                contact.friction = 0.5;
                contact.restitution = 0.5;
            }

            CollisionResolver resolver(5, 5);
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
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorBlue, 1, lightPos,
            lightColors, cameraPosition, 0.05, 0.1
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

    // View matrix, used for positioning and angling the camera
    // Camera's position in world coordinates
    real cameraDistance = 600.0f;
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, cameraDistance);
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
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0.06;

    int size = 22;
    real strength = 0.5;
    real mass = 0.5;
    real damping = 0.5;

    RectangularCloth mesh(mass, damping, strength, size, size,
        Vector3D(-200, 200, 0), Vector3D(200, -200, 0));

    // The first row of particles is suspended
    for (int i = 0; i < size; i++) {
        mesh.particles[i].setAwake(false);
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
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        int numSteps = 20;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            for (auto& particle : mesh.particles) {
                g.updateForce(&particle, deltaT);
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
                    move.x = worldPos.x - mesh.particles[size * size / 2].position.x;
                    move.y = worldPos.y - mesh.particles[size * size / 2].position.y;
                    move.z = worldPos.x - mesh.particles[size * size / 2].position.z;

                    move *= 1.5;
                    ParticleGravity f(move);

                    for (int i = 0; i < size; i++) {
                        f.updateForce(&mesh.particles[size + i * size - 1], deltaT);
                    }
                }
                else if (isButtonPressed[1]) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                    Vector3D move;
                    move.x = worldPos.x - mesh.particles[size * size / 2].position.x;
                    move.y = worldPos.y - mesh.particles[size * size / 2].position.y;
                    move.z = -(worldPos.x + mesh.particles[size * size / 2].position.z);

                    move *= 1.5;
                    ParticleGravity f(move);

                    for (int i = 0; i < size; i++) {
                        f.updateForce(&mesh.particles[i * size], deltaT);
                    }
                }
            }

            mesh.applyConstraints();

            for (int i = 0; i < size * size; i++) {
                if (mesh.particles[i].isAwake) {
                    mesh.particles[i].verletIntegrate(deltaT);
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

        FaceData data = getFaceData(mesh);
        EdgeData edata = getEdgeData(mesh);
        FrameVectors fdata = getFrameVectors(mesh, 30);

        glm::vec3 lightPos[]{ glm::vec3(500, 0, 500), glm::vec3(-500, 0, -500) };
        glm::vec4 lightColor[]{ glm::vec4(1.0, 1.0, 1.0, 1.0),
            glm::vec4(1.0, 1.0, 1.0, 1.0) };
        
      
        
        texShader.drawFaces(
            data.vertices,
            data.normals,
            data.uvCoordinates,
            identity,
            viewMatrix,
            projectionMatrix,
            texture,
            2,
            lightPos,
            lightColor
        );
        

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
    real deltaT = 0.035;

    real side = 150;
    RectangularPrism c1(side, side, side, 150, Vector3D(0, 0, 0));

    RectangularPrism c2(1000, 50, 1000, 150, Vector3D(0, -400, -0));

    c2.body->setAwake(false);

    c1.body->angularDamping = 0.95;
    c1.body->linearDamping = 0.95;

    c2.body->orientation = Quaternion(1, 0.2, 1, 0.3).normalized();

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D origin;

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

        c1.body->calculateDerivedData();
        c2.body->calculateDerivedData();

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            g.updateForce(c1.body, substep);

            std::vector<Contact> contacts;
            // Here we check for collision

            
            if (testIntersection(c1, c2)) {
                returnContacts(c1, c2, contacts);
            }

            for (Contact& contact : contacts) {
                contact.friction = 0;
                contact.restitution = 1;
            }

            CollisionResolver resolver(5, 5);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);


            if (isButtonPressed[0]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c1.body->position.x = worldPos.x * 2;
                c1.body->position.y = worldPos.y * 2;
                c1.body->position.z = worldPos.x * 2;
            }

            c1.body->integrate(substep);

            c1.update();
            c2.update();
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
            glm::vec3(0.0f, 100.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

        // Data
        FaceData data = getFaceData(c1);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorPurple, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
        );
        data = getFaceData(c2);
        cookShader.drawFaces(data.vertices, data.normals,
            identity, viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05
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
    real deltaT = 0.03;

    real side = 100;
    RectangularPrism c1(side, side, side, 150, Vector3D(250, 0, -150));

    real height = 150;
    side = 100;
    Pyramid c4(side, height, 150, Vector3D(-200, 0, 200));


    Joint joint(
        c1.body, 
        c4.body,
        c1.localVertices[4],
        c4.localVertices[3],
        5
    );


    c1.body->angularDamping = 0.5;
    c1.body->linearDamping = 0.90;
    c4.body->angularDamping = 0.5   ;
    c4.body->linearDamping = 0.90;

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D origin;

    int particleNum = 2;

    RigidBody fixed1;
    fixed1.position = Vector3D(200, 200, -200);
    RigidBody fixed4;
    fixed4.position = Vector3D(-200, 200, 200);

    RigidBodySpringForce force1(c1.localVertices[0], &fixed1, Vector3D(), 10, 100);

    ParticleGravity pg(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false,
        false,
        false,
        false
    };


    // Bounding volume hierarchy, used for coarse collision detection
    BoundingVolumeHierarchy<BoundingSphere> hierarchy;
    hierarchy.insert(&c1, c1.boundingSphere);

    hierarchy.insert(&c4, c4.boundingSphere);

    GLuint texture3 = loadTexture("C:\\Users\\msaba\\OneDrive\\Desktop\\textureMaps\\wood.jpg");

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
        c4.body->calculateDerivedData();

        fixed1.calculateDerivedData();
        fixed4.calculateDerivedData();

        int numSteps = 5;
        real substep = deltaT / numSteps;

        while (numSteps--) {

            g.updateForce(c1.body, substep);
            g.updateForce(c4.body, substep);

            force1.updateForce(c1.body, substep);

            // joint.update(substep);

            std::vector<Contact> contacts;
            joint.addContact(contacts);

            CollisionResolver resolver(5, 5);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            if (isButtonPressed[0]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c1.body->position.x = worldPos.x * 2;
                c1.body->position.y = worldPos.y * 2;
            }
            else if (isButtonPressed[3]) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                c4.body->position.x = worldPos.x * 2;
                c4.body->position.y = worldPos.y * 2;
            }

            c1.body->integrate(substep);
            c4.body->integrate(substep);

            c1.update();
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
        cookTexShader.drawFaces(data.vertices, data.normals,
            data.uvCoordinates,
            identity, viewMatrix, projectionMatrix, texture3, 1, lightPos,
            lightColors, cameraPosition, 0.05, 0.1
        );

        data = getFaceData(c4);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 40
        );

        window.display();
    }

    return 0;
}

#endif
