
#ifndef SPHERE
#define SPHERE

#include "geometricShape.h"
#include <vector>

namespace pe {

    class Sphere : public GeometricShape {

    public:

        // Number of points for each circle
        int pointNum = 100;

        // Circle radius
        real radius;

        /*
            Sets things like the inertia tensor and creates the graphics of
            the sphere.
        */
        Sphere(real radius, real mass, Vector3D position,
            sf::Color color) : GeometricShape(mass, position, color),
            radius(radius) {

            vertices.resize(3 * pointNum);
            coordinates.resize(3 * pointNum);
            // We draw 3 perpendicular cirlce.
            // To do that, we need each of the three axis to each time be either 0, cos, or sin,
            // and at no time are two axes the same thing
            for (int i = 0; i < pointNum; i++) {
                vertices[i] = Vector3D(radius * cos((real)(2 * PI * i / pointNum)),
                    radius * sin((real)(2 * PI * i / pointNum)), 0);
                vertices[i + pointNum] = Vector3D(0, radius * cos((real)(2 * PI * i / pointNum)),
                    radius * sin((real)(2 * PI * i / pointNum)));
                vertices[i + 2 * pointNum] = Vector3D(radius * sin((real)(2 * PI * i / pointNum)), 0,
                    radius * cos((real)(2 * PI * i / pointNum)));
            }

            real ixx = (2.0f / 5.0f) * mass * radius / radius;
            real iyy = (2.0f / 5.0f) * mass * radius / radius;
            real izz = (2.0f / 5.0f) * mass * radius / radius;
            Matrix3x3 inertiaTensor(ixx, 0, 0, 0, iyy, 0, 0, 0, izz);
            body->setInertiaTensor(inertiaTensor);

            body->angularDamping = 1;
            body->linearDamping = 1;

            body->calculateDerivedData();
            recalculateVertices();

            /*
                Because the offset is 0, the transform as a whole is the
                same as the body's.
            */
            transform = body->transformMatrix;
        }

        /*
            Returns the lines used in drawing the object.
        */
        virtual std::vector<sf::VertexArray> drawLines() {
            std::vector<sf::VertexArray> lines;
            lines.resize(3 * pointNum);
            // Connects the vertices circularly in each circle
            for (int i = 0; i < pointNum - 1; i++) {
                lines[i] = drawLine(coordinates[i], coordinates[i+1]);
                lines[pointNum + i] = drawLine(coordinates[pointNum + i],
                    coordinates[pointNum + i + 1]);
                lines[2 * pointNum + i] = drawLine(coordinates[2*pointNum + i],
                    coordinates[2*pointNum + i + 1]);
            }
            lines[pointNum - 1] = drawLine(coordinates[pointNum - 1], coordinates[0]);
            lines[2 * pointNum - 1] = drawLine(coordinates[2 * pointNum - 1], coordinates[pointNum]);
            lines[3 * pointNum - 1] = drawLine(coordinates[3 * pointNum - 1], coordinates[2*pointNum]);

            return lines;
        }
    };
}

#endif