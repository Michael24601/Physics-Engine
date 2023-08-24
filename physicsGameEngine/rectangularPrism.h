
#ifndef RECTANGULAR_PRISM
#define RECTANGULAR_PRISM

#include "rigidBody.h"
#include "geometricShape.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace pe{

    class RectangularPrism: public GeometricShape {

    public:

        // Side length
        real side1;
        real side2;
        real side3;

        RectangularPrism(real side1, real side2, real side3, real mass,
            Vector3D position, sf::Color color) :
            GeometricShape(mass, position, color), side1{ side1 },
            side2{ side2 }, side3{ side3 } {
            vertices.resize(8);
            coordinates.resize(8);
            vertices[0] = Vector3D(-side1 / 2, -side2 / 2, -side3 / 2);
            vertices[1] = Vector3D(side1 / 2, -side2 / 2, -side3 / 2);
            vertices[2] = Vector3D(side1 / 2, -side2 / 2, side3 / 2);
            vertices[3] = Vector3D(-side1 / 2, -side2 / 2, side3 / 2);
            vertices[4] = Vector3D(-side1 / 2, side2 / 2, -side3 / 2);
            vertices[5] = Vector3D(side1 / 2, side2 / 2, -side3 / 2);
            vertices[6] = Vector3D(side1 / 2, side2 / 2, side3 / 2);
            vertices[7] = Vector3D(-side1 / 2, side2 / 2, side3 / 2);

            Matrix3x3 inertiaTensor((side2* side2 + side3 * side3),
                0, 0, 0, (side1* side1 + side3 * side3),
                0, 0, 0, (side2* side2 + side1 * side1));
            inertiaTensor *= (mass / 12.0f);
            body.setInertiaTensor(inertiaTensor);

            body.angularDamping = 1;
            body.linearDamping = 1;

            body.calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(12);
            lines[0] = drawLine(coordinates[0], coordinates[1]);
            lines[1] = drawLine(coordinates[1], coordinates[2]);
            lines[2] = drawLine(coordinates[2], coordinates[3]);
            lines[3] = drawLine(coordinates[3], coordinates[0]);
            lines[4] = drawLine(coordinates[4], coordinates[5]);
            lines[5] = drawLine(coordinates[5], coordinates[6]);
            lines[6] = drawLine(coordinates[6], coordinates[7]);
            lines[7] = drawLine(coordinates[7], coordinates[4]);
            lines[8] = drawLine(coordinates[4], coordinates[0]);
            lines[9] = drawLine(coordinates[5], coordinates[1]);
            lines[10] = drawLine(coordinates[6], coordinates[2]);
            lines[11] = drawLine(coordinates[7], coordinates[3]);

            return lines;
        }
    };
}


#endif
