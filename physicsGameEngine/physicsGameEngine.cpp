
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
#include <GL/gl.h>

#include <iostream>
#include <vector>

#include "particle.h"
#include "particleSpringForce.h"
#include "particleBungeeForce.h"
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
#include "clothWithBungeeCord.h"
#include "cone.h"

#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "diffuseSpecularLightingShader.h"
#include "cookTorranceShader.h"
#include "tesselationUtil.h"
#include "shaderInterface.h"
#include "cord.h"

#include "bodyCordAdapter.h"

#include "sat.h"
#include "contact.h"
#include "contactGeneration.h"
#include "collisionResolver.h"

#include "BVHNode.h"
#include "boundingVolumeHierarchy.h"
#include "boundingSphere.h"


using namespace pe;
using namespace std;


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
    real deltaT = 0;

    real side = 100;
    RectangularPrism c1(new RigidBody(), side, side, side, 
        150, Vector3D(250, 200, -150));

    real height = 150;
    real radius = 50;
    Cylinder c2(new RigidBody(), radius, height, 150, 20, 
        Vector3D(200, 0, 200));

    radius = 100;
    SolidSphere c3(new RigidBody(), radius, 150, 25, 25,
        Vector3D(-200, 0, -200));

    height = 150;
    side = 100;
    Pyramid c4(new RigidBody(), side, height, 150, 
        Vector3D(-200, 0, 200));


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

    Cord cord1(0.2, 0.9, 5, particleNum, Vector3D(200, 200, -200), Vector3D(100, 200, -100));
    Cord cord2(0.2, 0.9, 5, particleNum, Vector3D(200, 200, 200), Vector3D(100, 200, 100));
    Cord cord3(0.2, 0.9, 5, particleNum, Vector3D(-200, 200, -200), Vector3D(-100, 200, -100));
    Cord cord4(0.2, 0.9, 5, particleNum, Vector3D(-200, 200, 200), Vector3D(-100, 200, 100));

    ParticleGravity pg(Vector3D(0, -10, 0));

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false,
        false,
        false,
        false
    };

    BodyCordAdapter adapter1(
        c1.body, 
        &cord1, 
        c1.localVertices[0], 
        100, 
        10, 
        Vector3D(100, 200, -200)
    );

    BodyCordAdapter adapter2(
        c2.body,
        &cord2,
        c2.localVertices[0],
        100,
        10,
        Vector3D(200, 200, 200)
    );

    BodyCordAdapter adapter3(
        c3.body,
        &cord3,
        c3.localVertices[0],
        100,
        10,
        Vector3D(-200, 200, -200)
    );

    BodyCordAdapter adapter4(
        c4.body,
        &cord4,
        c4.localVertices[0],
        100,
        10,
        Vector3D(-200, 200, 200)
    );

    // Bounding volume hierarchy, used for coarse collision detection
    BoundingVolumeHierarchy<BoundingSphere> hierarchy;
    hierarchy.insert(c1.body, c1.boundingSphere);
    hierarchy.insert(c2.body, c2.boundingSphere);
    hierarchy.insert(c3.body, c3.boundingSphere);
    hierarchy.insert(c4.body, c4.boundingSphere);

    while (window.isOpen()) {

        clock.restart();

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
        }

        c1.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        c3.body->calculateDerivedData();
        c4.body->calculateDerivedData();

        g.updateForce((c1.body), deltaT);
        g.updateForce((c2.body), deltaT);
        g.updateForce((c3.body), deltaT);
        g.updateForce((c4.body), deltaT);

        adapter1.updateForce(deltaT);
        adapter2.updateForce(deltaT);
        adapter3.updateForce(deltaT);
        adapter4.updateForce(deltaT);

        for (Particle& p : cord1.particles) {
            pg.updateForce(&p, deltaT);
        }
        for (Particle& p : cord2.particles) {
            pg.updateForce(&p, deltaT);
        }
        for (Particle& p : cord3.particles) {
            pg.updateForce(&p, deltaT);
        }
        for (Particle& p : cord4.particles) {
            pg.updateForce(&p, deltaT);
        }

        std::vector<Contact> contacts;
        // Here we check for collision

        /*
            constexpr int LIMIT = 100;
            PotentialContact contactArray[LIMIT];
            int num = hierarchy.getRoot()->getPotentialContacts(contactArray, LIMIT);
            for (int i = 0; i < num; i++) {
                if (testIntersection(contactArray[i].rigidBody[0], c3)) {
                    returnMaxContact(c1, c3, contacts);
                }
            }
        */
        if (testIntersection(c1, c3)) {
            returnMaxContact(c1, c3, contacts);
        }

        edgeData contactEdges;
        for (Contact& contact : contacts) {
            // Stores drawing data
            contactEdges.vertices.push_back(convertToGLM(contact.contactPoint));
            real length = 100;
            Vector3D otherPoint = contact.contactPoint +
                contact.contactNormal * length;
            contactEdges.vertices.push_back(convertToGLM(otherPoint));

            contact.friction = 0.5;
            contact.restitution = 1;
        }

        CollisionResolver resolver(1, 1);
        resolver.resolveContacts(contacts.data(), contacts.size(), deltaT);

        if (isButtonPressed[0]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c1.body->position.x = worldPos.x;
            c1.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[1]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c2.body->position.x = worldPos.x;
            c2.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[2]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c3.body->position.x = worldPos.x;
            c3.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[3]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c4.body->position.x = worldPos.x;
            c4.body->position.y = worldPos.y;
        }

        c1.body->integrate(deltaT);
        c2.body->integrate(deltaT);
        c3.body->integrate(deltaT);
        c4.body->integrate(deltaT);

        for (Particle& p : cord1.particles) {
            p.verletIntegrate(deltaT);
        }
        for (Particle& p : cord2.particles) {
            p.verletIntegrate(deltaT);
        }
        for (Particle& p : cord3.particles) {
            p.verletIntegrate(deltaT);
        }
        for (Particle& p : cord4.particles) {
            p.verletIntegrate(deltaT);
        }

        adapter1.reposition();
        adapter2.reposition();
        adapter3.reposition();
        adapter4.reposition();

        c1.update();
        c2.update();
        c3.update();
        c4.update();

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
        glm::vec4 colorPurple = glm::vec4(0.4, 0.1, 0.8, 1.0);

        edgeData cordData = getMeshEdgeData(cord1);
        shader.drawEdges(cordData.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite);
        cordData = getMeshEdgeData(cord2);
        shader.drawEdges(cordData.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite);
        cordData = getMeshEdgeData(cord3);
        shader.drawEdges(cordData.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite);
        cordData = getMeshEdgeData(cord4);
        shader.drawEdges(cordData.vertices, identity, viewMatrix, projectionMatrix,
            colorWhite);
        //shader.drawEdges(contactEdges.vertices, identity, viewMatrix, projectionMatrix,
          //  colorWhite);

        // Shape
        glm::vec3 lightPos[]{
            glm::vec3(0.0f, 100.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        }; 

        // Data
        faceData data = getPolyhedronFaceData(c1);
        cookShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorPurple, 1, lightPos,
            lightColors, cameraPosition, 0.1, 0.05);

        // Second shape
        data = getCylinderFaceData(c2);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorBlue, 1, lightPos,
            lightColors, cameraPosition, 40);

        data = getSphereFaceData(c3);
        cookShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorRed, 1, lightPos,
            lightColors, cameraPosition, 0.05, 1);

        data = getPolyhedronFaceData(c4);
        phongShader.drawFaces(data.vertices, data.normals, identity,
            viewMatrix, projectionMatrix, colorGreen, 1, lightPos,
            lightColors, cameraPosition, 40);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}
