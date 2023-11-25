
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>
#include "rigidBody.h"
#include <vector>

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "cube.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "rigidBodyCableForce.h"

using namespace pe;
using namespace std;

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 32;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Test",
        sf::Style::Default, settings);
    
    // Sets up OpenGL states (for 3D)
    glEnable(GL_DEPTH_TEST);

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

    // Field of View(FOV) in degrees
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
    Cube c(new RigidBody(), side, 150, Vector3D(100, 100, 0));

    real side2 = 200;
    Cube c2(new RigidBody(), side2, 150, Vector3D(-200, 0, 0));


    RigidBody fixed;
    fixed.position = Vector3D(100, 200, 0);

    c.body->angularDamping = 0.75;
    c.body->linearDamping = 0.90;


    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side / 2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 10, 100);

    real rotationSpeed = 0.05;
    real angle = PI/2;
    bool isButtonPressed = false;

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
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left){
                isButtonPressed = false;
            }
            // Rotates camera
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
                
                cout << cameraPosition.x << " " << cameraPosition.z << "\n";
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

        s.updateForce((c.body), deltaT);
        g.updateForce((c.body), deltaT);

        if (isButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c.body->position.x = worldPos.x;
            c.body->position.y = worldPos.y;
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

        // Draw cable/spring
        Vector3D point = c.body->transformMatrix.transform(app);
        vector<pair<Vector3D, Vector3D>> v(1);
        v[0].first = point;
        v[0].second = fixed.position;

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Now we combine the two matrices to create our final view
        glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

        // Shape
        drawVectorOfLines3D(c.getEdges(), viewProjectionMatrix, window, sf::Color::White);

        // Spring/Cable
        drawVectorOfLines3D(v, viewProjectionMatrix, window, sf::Color::Green);

        // Second shape
        drawVectorOfLines3D(c2.getEdges(), viewProjectionMatrix, window, sf::Color::White);

        // Normal vectors of the collision
        drawVectorOfLines3D(normals, viewProjectionMatrix, window, sf::Color::Red);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}