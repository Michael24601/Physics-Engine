
#ifndef TETRAHEDRON
#define TETRAHEDRON

#include "rigidbody.h"
#include <vector>
#include "geometricShape.h"

namespace pe {

    class Tetrahedron : public GeometricShape {

    public:

        // Side length
        real side;

        Tetrahedron(real side, real mass, Vector3D position,
            sf::Color color) : GeometricShape(mass, position, color),
            side{ side } {
            vertices.resize(4);
            coordinates.resize(4);
            /*
                In order for the inertia tensor to be this simple, we have
                to set the 3 heights of the tetrahedron to lie along the x,
                y, and z axes, and from there find the coordinates of the
                vertices.
            */
            vertices[0] = Vector3D(0, side / realSqrt(3.0),
                -side / (2.0 * realSqrt(6.0)));
            vertices[1] = Vector3D(-side / 2.0, -side / (2.0 * realSqrt(3.0)),
                -side / (2.0 * realSqrt(6.0)));
            vertices[2] = Vector3D(side / 2.0, -side / (2.0 * realSqrt(3.0)),
                -side / (2.0 * realSqrt(6.0)));
            vertices[3] = Vector3D(0, 0,
                side* realSqrt(2.0) / (2.0 * realSqrt(6.0)));

            double Ixx, Iyy, Izz = (1.0f / 60.0f) * mass * side * side;
            Matrix3x3 inertiaTensor(Ixx, 0, 0, 0, Iyy, 0, 0, 0, Izz);
            inertiaTensor *= (mass * side * side / 60.0f);
            body->setInertiaTensor(inertiaTensor);

            body->angularDamping = 1;
            body->linearDamping = 1;

            body->calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(6);
            lines[0] = drawLine(coordinates[1], coordinates[2]);
            lines[1] = drawLine(coordinates[2], coordinates[3]);
            lines[2] = drawLine(coordinates[3], coordinates[1]);
            lines[3] = drawLine(coordinates[0], coordinates[3]);
            lines[4] = drawLine(coordinates[0], coordinates[1]);
            lines[5] = drawLine(coordinates[0], coordinates[2]);

            return lines;
        }
    };
}

#endif

