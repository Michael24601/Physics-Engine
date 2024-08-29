
#ifndef COLLISION_BOX
#define COLLISION_BOX

#include "polyhedron.h"
#include "rectangularPrism.h"
#include "solidSphere.h"

namespace pe {

    /*
        An abstraction used in all of this collision detection system,
        so that any shape (not necessarily a box shape) can use the
        box collision detection and contact generation functions if
        we allow for some small inaccuracies.
        Any polyhedron, so long that it provides a halfSize (dimensions
        for the box) can be considered a box collision-wise,
        with the halfSize as the side length, and the centre of gravity
        of the box as the centre of gravity of the object (they both
        share the same Body object, and by extension the transform matrix).
        Only RectangularPrism will perfectly fit in a box however.
    */
    struct Box {

        /*
            Constructs a box from a polyhedron.
            For any shape, there will be a smallest collision box that can
            contain the entire shape. This shape may not be centred at the
            centre of gravity however
            (the geometric centre may not be the centre of gravity,
            which will be closer to areas of the shape with more vertices.
            This is why we need an offset.

            We can use an OBB, which is calculated once and can be
            transformed each frame using a modified transform matrix of the
            original body.

            We can use an AABB, which recalculated each frame as to remain
            axis aligned.

            We choose an OBB.

            We also send the body of the cuboidal polyhedron as it is not 
            in the Cauboidal interface.
        */
        Box(const Cuboidal& cuboid, RigidBody* body) {
            halfSize = cuboid.boundingBox.getHalfsize();
            this->body = body;

            transformMatrix = cuboid.boundingBox.getTransformMatrix();
        }

        Matrix3x4 transformMatrix;
        Vector3D halfSize;
        RigidBody* body;

        Vector3D getAxis(int index) const {
            return transformMatrix.getColumnVector(index);
        }
    };


    /*
        Similarly, this is an abstraction for all the sphere collision
        detection and contact generation functions.
        Only a radius needs to be provided, and any Polyhedron's collisions
        can be simulated as a sphere's with that radius and the centre at
        the centre of gravity of the Polyhedron (they both share the same
        Body object, and by extension the transform matrix).
        Only the SolidSphere object will perfectly fit in a sphere however.
   */
    struct Ball {

        /*
            Constructs a sphere from a spherical polyhedron.
        */
        Ball(const Spherical& sphere, RigidBody* body) {
            this->body = body;
            radius = sphere.boundingSphere.getRadius();
            transformMatrix = sphere.boundingSphere.getTransformMatrix();
        }

        Matrix3x4 transformMatrix;
        RigidBody* body;
        real radius;

        Vector3D getAxis(int index) const {
            return body->transformMatrix.getColumnVector(index);
        }
    };
}


#endif