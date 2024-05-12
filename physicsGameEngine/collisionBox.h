
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

            We opt to use a mix of the two; we generate an AABB originally,
            such that it is axis aligned, but then we freely trasnform it;
            AABB is easier to initially calculate, but since our collision
            resolution system can handle non axis aligned boxes, we can
            transform it instead of recalculating it, effectively making it
            an OBB (that may not be as tight of a fit an OBB since it initially
            was calculated to be axis aligned).
        */
        Box(const Polyhedron& polyhedron) {
            halfSize = polyhedron.getAABBHalfsize();
            body = polyhedron.body;

            transformMatrix = polyhedron.getAABBTransformMatrix();
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

        Ball (RigidBody* body, real radius) :
            body{ body }, radius{ radius } {}

        /*
            Constructs a sphere from a general polyhedron.
        */
        Ball(const Polyhedron& polyhedron) {
            body = polyhedron.body;
            radius = polyhedron.getBoundingSphereRadius();
            offset = polyhedron.getSphereOffset();
            transformMatrix = polyhedron.body->transformMatrix;

            transformMatrix = polyhedron.getBoundingSphereTransformMatrix();
        }

        Matrix3x4 transformMatrix;
        RigidBody* body;
        Vector3D offset;
        real radius;

        Vector3D getAxis(int index) const {
            return body->transformMatrix.getColumnVector(index);
        }
    };
}


#endif