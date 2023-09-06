
#ifndef PYRAMID
#define PYRAMID

#include "rigidbody.h"
#include <vector>
#include "geometricShape.h"

namespace pe {

    class Pyramid: public GeometricShape {

    public:

        // Side length
        real side;
        real height;

        Pyramid(real side, real height, real mass, Vector3D position,
            sf::Color color) : GeometricShape(mass, position, color),
            side{ side }, height{height} {
            vertices.resize(5);
            coordinates.resize(5);
            vertices[0] = Vector3D(0, -3 * height / 4.0, 0);
            vertices[1] = Vector3D(-side / 2, height / 4.0, -side / 2);
            vertices[2] = Vector3D(side / 2, height / 4.0, -side / 2);
            vertices[3] = Vector3D(side / 2, height / 4.0, side / 2);
            vertices[4] = Vector3D(-side / 2, height / 4.0, side / 2);

            Matrix3x3 inertiaTensor(
                mass* (3 * height * height + side * side) / 12.0f,
                0, 0, 0, mass* (3 * height * height + side * side) / 12.0f,
                0, 0, 0, (mass* side* side) / 6.0f);
            body->setInertiaTensor(inertiaTensor);

            body->angularDamping = 1;
            body->linearDamping = 1;

            body->calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(8);
            lines[0] = drawLine(coordinates[0], coordinates[1]);
            lines[1] = drawLine(coordinates[0], coordinates[2]);
            lines[2] = drawLine(coordinates[0], coordinates[3]);
            lines[3] = drawLine(coordinates[0], coordinates[4]);
            lines[4] = drawLine(coordinates[1], coordinates[2]);
            lines[5] = drawLine(coordinates[2], coordinates[3]);
            lines[6] = drawLine(coordinates[3], coordinates[4]);
            lines[7] = drawLine(coordinates[4], coordinates[1]);

            return lines;
        }
    };
}

#endif

