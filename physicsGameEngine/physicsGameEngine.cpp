
#include <SFML/Graphics.hpp>
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

    // Camera's position in world coordinates
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 400.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Create a view matrix
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, 
        cameraTarget, upVector);

    sf::RenderWindow window(sf::VideoMode(800, 800), "Test");

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0;

    real side = 100;
    Cube c(new RigidBody(), side, 150, Vector3D(0, 100, 0));

    real side2 = 200;
    Cube c2(new RigidBody(), side2, 150, Vector3D(-200, 0, 0));


    RigidBody fixed;
    fixed.position = Vector3D(200, 200, 0);

    c.body->angularDamping = 0.80;
    c.body->linearDamping= 0.80;


    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side/2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 10, 100);

    real rotationSpeed = 0.1;
  
    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                RigidBodyGravity force(Vector3D(3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                RigidBodyGravity force(Vector3D(-3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                float newX = cos(rotationSpeed) * cameraPosition.x
                    - sin(rotationSpeed) * cameraPosition.z;
                float newZ = sin(rotationSpeed) * cameraPosition.x
                    + cos(rotationSpeed) * cameraPosition.z;
                cameraPosition.x = newX;
                cameraPosition.z = newZ;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                float newX = cos(-rotationSpeed) * cameraPosition.x
                    - sin(-rotationSpeed) * cameraPosition.z;
                float newZ = sin(-rotationSpeed) * cameraPosition.x
                    + cos(-rotationSpeed) * cameraPosition.z;
                cameraPosition.x = newX;
                cameraPosition.z = newZ;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        fixed.calculateDerivedData();

        s.updateForce((c.body), deltaT);
        g.updateForce((c.body), deltaT);

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

        vector<sf::VertexArray> vertexArray;
        vertexArray = transformLinesToVertexArray(c.getEdges(), 
            viewMatrix, sf::Color::White);
        drawVectorOfVertexArray(vertexArray, window);
        vertexArray = transformLinesToVertexArray(v, viewMatrix, 
            sf::Color::Green);
        drawVectorOfVertexArray(vertexArray, window);
        vertexArray = transformLinesToVertexArray(c2.getEdges(), 
            viewMatrix, sf::Color::White);
        drawVectorOfVertexArray(vertexArray, window);
        vertexArray = transformLinesToVertexArray(normals, 
            viewMatrix, sf::Color::Red);
        drawVectorOfVertexArray(vertexArray, window);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}