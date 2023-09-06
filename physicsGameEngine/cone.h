
#ifndef CONE
#define CONE

#include "rigidbody.h"
#include "geometricShape.h"
#include <vector>

namespace pe {

    class Cone : public GeometricShape {

    public:

        real radius;
        real height;
        int pointNum = 100;
        // Max is half the number of points, and should be a divisor of pointNum
        int lineNum = pointNum / 4;

        Cone(real radius, real height, real mass, Vector3D position,
            sf::Color color) : GeometricShape(mass, position, color),
            height{ height }, radius{ radius } {

            vertices.resize(1 + pointNum);
            coordinates.resize(1 + pointNum);
            vertices[0] = Vector3D(0, (3 * height / 4.0), 0);
            for (int i = 1; i <= pointNum; i++) {
                vertices[i] = Vector3D(radius * cos((real)(2 * PI * i / pointNum)),
                    -height / 4.0, radius * sin((real)(2 * PI * i / pointNum)));
            }

            real ixx = (3.0 / 20.0) * mass * (std::pow(height, 2)
                + 4 * std::pow(radius, 2));
            real iyy = (3.0 / 10.0) * mass * std::pow(radius, 2);
            real izz = (3.0 / 20.0) * mass * (std::pow(height, 2)
                + 4 * std::pow(radius, 2));
            Matrix3x3 inertiaTensor(ixx, 0, 0, 0, iyy, 0, 0, 0, izz);
            body->setInertiaTensor(inertiaTensor);

            body->angularDamping = 1;
            body->linearDamping = 1;

            body->calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(pointNum + lineNum);

            for (int i = 0; i < pointNum - 1; i++) {
                lines[i] = drawLine(coordinates[i + 1], coordinates[i + 2]);
            }
            lines[pointNum - 1] = drawLine(coordinates[pointNum], coordinates[1]);

            for (int i = 0; i < lineNum; i++) {
                // Draw lines from two opposite ends of the ellipse
                lines[pointNum + i] = drawLine(coordinates[0],
                    coordinates[i * (pointNum / lineNum) + 1]);
            }

            return lines;
        }
    };
}

#endif