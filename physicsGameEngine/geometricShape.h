
#ifndef GEOMETRIC_SHAPE
#define GEOMETRIC_SHAPE

#include "rigidBody.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace pe {

    class GeometricShape {

    public:

        // relative position of vertices to center of cube
        std::vector<Vector3D> vertices;
        // Absolute coordinates of the vertices
        std::vector<sf::Vector2f> coordinates;
        sf::Color color;
        // Rigid body containing the physics
        RigidBody body;

        GeometricShape(real mass, Vector3D position, sf::Color color) :
            color{ color } {
            body.setMass(mass);
            body.position = position;
        }

        sf::VertexArray drawLine(sf::Vector2f c1, sf::Vector2f c2) {
            sf::VertexArray line(sf::LineStrip, 2);

            line[0].position = c1;
            line[1].position = c2;
            line[0].color = line[1].color = color;

            return line;
        }

        virtual std::vector<sf::VertexArray> drawLines() = 0;

        /*
            Called each frame, calculates aboslute coordinates from relative
            position.
        */
        void recalculateVertices() {
            for (int i = 0; i < vertices.size(); i++) {
                Vector3D absolute =
                    body.getPointInWorldCoordinates(vertices[i]);
                // 800 - because the axis is flipped
                coordinates[i] = sf::Vector2f(absolute.x, absolute.y);
            }
        }
    };
}

#endif
