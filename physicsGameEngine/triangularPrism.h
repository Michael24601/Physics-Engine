
#ifndef TRIANGULAR_PRISM
#define TRIANGULAR_PRISM

#include "rigidBody.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include "geometricShape.h"

namespace pe {

    class TriangularPrism : public GeometricShape {

    public:

        // Side length
        real side1;
        real side2;
        real height;

        TriangularPrism(real side1, real side2, real height, real mass,
            Vector3D position, sf::Color color) :
            GeometricShape(mass, position, color), side1{ side1 },
            side2{ side2 }, height{ height } {
            vertices.resize(6);
            coordinates.resize(6);
            vertices[0] = Vector3D(-side1 / 2, height / 3, -side2 / 2);
            vertices[1] = Vector3D(side1 / 2, height / 3, -side2 / 2);
            vertices[2] = Vector3D(side1 / 2, height / 3, side2 / 2);
            vertices[3] = Vector3D(-side1 / 2, height / 3, side2 / 2);
            vertices[4] = Vector3D(0, -2 * height / 3, -side2 / 2);
            vertices[5] = Vector3D(0, -2 * height / 3, side2 / 2);

            Matrix3x3 inertiaTensor(
                mass* (side1* side1 + 3 * side2 * side2) / 18.0f,
                0, 0, 0, mass* (3 * side1 * side1 + 3 * height * height) / 36.0f
                + (mass * side2 * side2) / 6.0f,
                0, 0, 0, mass* (height* height + side1 * side1) / 18.0f);
            body.setInertiaTensor(inertiaTensor);

            body.angularDamping = 1;
            body.linearDamping = 1;

            body.calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(9);
            lines[0] = drawLine(coordinates[0], coordinates[1]);
            lines[1] = drawLine(coordinates[1], coordinates[2]);
            lines[2] = drawLine(coordinates[2], coordinates[3]);
            lines[3] = drawLine(coordinates[3], coordinates[0]);
            lines[4] = drawLine(coordinates[0], coordinates[4]);
            lines[5] = drawLine(coordinates[1], coordinates[4]);
            lines[6] = drawLine(coordinates[2], coordinates[5]);
            lines[7] = drawLine(coordinates[3], coordinates[5]);
            lines[8] = drawLine(coordinates[4], coordinates[5]);
            return lines;
        }
    };
}

#endif
