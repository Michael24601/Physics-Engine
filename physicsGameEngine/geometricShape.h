/*
    In the book, this is called a primitive, and it does not include any
    of the graphics.
*/

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
        RigidBody* body;
        
        /*
			Because the geometric centre may not be the same as the centre
			of mass in all cases (especially in the case of complex shapes),
			we need a 3 by 4 matrix to represent the offset in terms of a
			rotation and translation.
		*/
        Matrix3x4 offset;

        /*
            The final transformation matrix, which combines the transform of
            the bodie's transform to get the rotation and translation, and
            the offset, to get the geometric centre in terms of the bodie's
            centre. If there is no offset, the transform is the same as the
            body's transform matrix.
        */
        Matrix3x4 transform;


        // Default constructor which leaves the body as null
        GeometricShape() {}

        GeometricShape(real mass, Vector3D position, sf::Color color) :
            color{ color } {
            body = new RigidBody();
            body->setMass(mass);
            body->position = position;
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
                    body->getPointInWorldCoordinates(vertices[i]);
                // 800 - because the axis is flipped
                coordinates[i] = sf::Vector2f(absolute.x, absolute.y);
            }
        }

        /*
            Returns the first, second, third or fourth column in the body's
            transform matrix, starting with i = 0.
        */
        Vector3D getAxis(unsigned int i) const {
            return transform.getColumnVector(i);
        }
    };
}

#endif
