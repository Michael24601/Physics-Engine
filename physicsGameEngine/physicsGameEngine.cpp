
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

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "rectangularPrism.h"
#include "pyramid.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "rigidBodyCableForce.h"

#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "diffuseSpecularLightingShader.h"

using namespace pe;
using namespace std;

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Test",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
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
    real aspectRatio = window.getSize().x / static_cast<real>(window.getSize().y);
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
    real deltaT = 0;

    real side = 100;
    RectangularPrism c(new RigidBody(), side, side, side, 150, Vector3D(100, 100, 0));

    real height = 200;
    Pyramid c2(new RigidBody(), 150, height, 150, Vector3D(-200, 0, 0));

    RigidBody fixed;
    fixed.position = Vector3D(100, 200, 0);
    RigidBody fixed2;
    fixed2.position = Vector3D(-200, 200, 0);

    c.body->angularDamping = 0.75;
    c.body->linearDamping = 0.90;
    c2.body->angularDamping = 0.75;
    c2.body->linearDamping = 0.90;

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D origin;
    // Applies it to the first vertex
    RigidBodySpringForce s(c.localVertices[0], &fixed, origin, 10, 100);
    RigidBodySpringForce s2(c2.localVertices[0], &fixed2, origin, 10, 100);

    real rotationSpeed = 0.05;
    real angle = PI/2;
    bool isButtonPressed = false;
    bool isSecondButtonPressed = false;

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed = true;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                isSecondButtonPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left){
                isButtonPressed = false;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Right) {
                isSecondButtonPressed = false;
            }
            // Rotates camera
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);;
            }
        }

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        fixed.calculateDerivedData();
        fixed2.calculateDerivedData();

        s.updateForce((c.body), deltaT);
        g.updateForce((c.body), deltaT);
        s2.updateForce((c2.body), deltaT);
        g.updateForce((c2.body), deltaT);

        if (isButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c.body->position.x = worldPos.x;
            c.body->position.y = worldPos.y;
        } else if (isSecondButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c2.body->position.x = worldPos.x;
            c2.body->position.y = worldPos.y;
        }

        vector<std::pair<Vector3D, Vector3D>> normals;
        // Resolves collisions
        if (c.isColliding(c2)) {
            std::vector<Contact> contacts;
            returnMaxContact(c, c2, contacts);
            for (int i = 0; i < contacts.size(); i++) {
                vector<std::pair<Vector3D, Vector3D>> a = 
                    contacts[i].drawNormals(100);
                for (int i = 0; i < a.size(); i++) {
                    normals.push_back(a[i]);
                }
            }
        }

        c.body->integrate(deltaT);
        c2.body->integrate(deltaT);
        c.updateVertices();
        c2.updateVertices();

        // Draw cables/springs
        // (Could laos use global here)
        Vector3D point = c.body->transformMatrix.transform(c.localVertices[0]);
        vector<pair<Vector3D, Vector3D>> v(2);
        v[0].first = point;
        v[0].second = fixed.position;
        point = c2.body->transformMatrix.transform(c2.localVertices[0]);
        v[1].first = point;
        v[1].second = fixed2.position;

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        shader.drawEdges(v, identity, viewMatrix, projectionMatrix, color);

        // Shape
        glm::mat4 cTransform = convertToGLM(c.body->transformMatrix);
        glm::vec4 colorPurple = glm::vec4(0.6, 0.2, 0.95, 1.0);
        glm::vec3 lightPos[]{
            glm::vec3(0.0f, 300.0f, 0.0f),
            glm::vec3(200.0f, -200.0f, -200.0f),
            glm::vec3(-200.0f, -200.0f, 200.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        };
        lightShader.drawFaces(c.getLocalFaces(), cTransform, viewMatrix,
            projectionMatrix, colorPurple, 3, lightPos, lightColors);

        // Second shape
        glm::mat4 c2Transform = convertToGLM(c2.body->transformMatrix);
        glm::vec4 colorGreen = glm::vec4(0.4, 1.0, 0.4, 1.0);
        lightShader.drawFaces(c2.getLocalFaces(), c2Transform, viewMatrix,
            projectionMatrix, colorGreen, 3, lightPos, lightColors);

        // Normal vectors of the collision
        // Likewise, the collision normal is in world coordinates
        shader.drawEdges(normals, identity, viewMatrix, projectionMatrix, color);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    glDisable(GL_DEPTH_TEST);

    return 0;
}