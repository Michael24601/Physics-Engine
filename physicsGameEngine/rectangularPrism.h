
#ifndef RECTANGULAR_PRISM
#define RECTANGULAR_PRISM

#include "rigidbody.h"
#include "geometricShape.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace pe{

    class RectangularPrism: public GeometricShape {

    public:

        // Side lengths
        Vector3D side;

        RectangularPrism(const Vector3D& side, real mass,
            Vector3D position, sf::Color color) :
            GeometricShape(mass, position, color), side{ side } {
            vertices.resize(8);
            coordinates.resize(8);
            vertices[0] = Vector3D(-side.x / 2, -side.y / 2, -side.z / 2);
            vertices[1] = Vector3D(side.x / 2, -side.y / 2, -side.z / 2);
            vertices[2] = Vector3D(side.x / 2, -side.y / 2, side.z / 2);
            vertices[3] = Vector3D(-side.x / 2, -side.y / 2, side.z / 2);
            vertices[4] = Vector3D(-side.x / 2, side.y / 2, -side.z / 2);
            vertices[5] = Vector3D(side.x / 2, side.y / 2, -side.z / 2);
            vertices[6] = Vector3D(side.x / 2, side.y / 2, side.z / 2);
            vertices[7] = Vector3D(-side.x / 2, side.y / 2, side.z / 2);

            Matrix3x3 inertiaTensor((side.y* side.y + side.z * side.z),
                0, 0, 0, (side.x* side.x + side.z * side.z),
                0, 0, 0, (side.y* side.y + side.x * side.x));
            inertiaTensor *= (mass / 12.0f);
            body->setInertiaTensor(inertiaTensor);

            body->angularDamping = 1;
            body->linearDamping = 1;

            /*
                Because the offset is 0, the transform as a whole is the
                same as the body's.
            */
            transform = body->transformMatrix;

            body->calculateDerivedData();
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
