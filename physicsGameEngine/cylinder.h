
#ifndef CYLINDER
#define CYLINDER

#include "rigidBody.h"
#include "geometricShape.h"
#include <vector>

namespace pe {

    class Cylinder : public GeometricShape {

    public:

        real radius;
        real height;
        int pointNum = 100;
        // Max is half the number of points, and should be a divisor of pointNum
        int lineNum = pointNum / 4;

        Cylinder(real radius, real height, real mass, Vector3D position,
            sf::Color color) : GeometricShape(mass, position, color),
            height{ height }, radius{ radius } {

            vertices.resize(2 * pointNum);
            coordinates.resize(2 * pointNum);
            for (int i = 0; i < pointNum; i++) {
                vertices[i] = Vector3D(radius * cos((real)(2 * PI * i / pointNum)),
                    height / 2.0, radius * sin((real)(2 * PI * i / pointNum)));
                vertices[i + pointNum] = Vector3D(radius * cos((real)(2 * PI * i / pointNum)),
                    -height / 2.0, radius * sin((real)(2 * PI * i / pointNum)));
            }

            real ixx = (1.0f / 12.0f) * mass * (3 * radius * radius + height * height);
            real iyy = (1.0f / 12.0f) * mass * (3 * radius * radius + height * height);
            real izz = (1.0f / 2.0f) * mass * radius * radius;
            Matrix3x3 inertiaTensor(ixx, 0, 0, 0, iyy, 0, 0, 0, izz);
            body.setInertiaTensor(inertiaTensor);

            body.angularDamping = 1;
            body.linearDamping = 1;

            body.calculateDerivedData();
            recalculateVertices();
        }

        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(2 * pointNum + lineNum);

            for (int i = 0; i < pointNum - 1; i++) {
                lines[i] = drawLine(coordinates[i], coordinates[i + 1]);
                lines[i + pointNum] = drawLine(coordinates[i + pointNum],
                    coordinates[i + pointNum + 1]);
            }
            lines[pointNum - 1] = drawLine(coordinates[pointNum - 1], coordinates[0]);
            lines[2 * pointNum - 1] = drawLine(coordinates[2 * pointNum - 1], coordinates[pointNum]);

            for (int i = 0; i < lineNum; i++) {
                // Draw lines from two opposite ends of the ellipse
                lines[2 * pointNum + i] = drawLine(coordinates[i * (pointNum / lineNum)],
                    coordinates[i * (pointNum / lineNum) + pointNum]);
            }

            return lines;
        }
    };
}

#endif