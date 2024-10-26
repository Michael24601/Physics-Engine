
#ifndef FINE_COLLISION_DETECTION_H
#define FINE_COLLISION_DETECTION_H

#include "contact.h"
#include "rigidObject.h"
#include "boundingBox.h"
#include "boundingSphere.h"

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

        Matrix3x4 transformMatrix;
        Vector3D halfSize;
        RigidBody* body;

        /*
            Constructs a box from a polyhedron.
            For any shape, there will be a smallest collision box that can
            contain the entire shape. This shape may not be centred at the
            centre of gravity however
            (the geometric centre may not be the centre of gravity,
            which will be closer to areas of the shape with more vertices.
            This is why we need an offset.
        */
        Box(
            const BoundingBox* box, 
            const Matrix3x4& boundingVolumeTransform,
            RigidBody* body
        ) {
            halfSize = box->getHalfsize();
            this->body = body;
            transformMatrix = boundingVolumeTransform;
        }

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

        Matrix3x4 transformMatrix;
        RigidBody* body;
        real radius;

        /*
            Constructs a sphere from a spherical polyhedron.
        */
        Ball(
            const BoundingSphere* ball,
            const Matrix3x4& boundingVolumeTransform,
            RigidBody* body
        ) {
            radius = ball->getRadius();
            this->body = body;
            transformMatrix = boundingVolumeTransform;
        }

        Vector3D getAxis(int index) const {
            return body->transformMatrix.getColumnVector(index);
        }
    };


    static inline real transformToAxis(
        const Box& box,
        const Vector3D& axis
    );


    /*
        This function checks if the two boxes overlap
        along the given axis, returning the ammount of overlap.
        The final parameter toCentre
        is used to pass in the vector between the boxes centre
        points, to avoid having to recalculate it each time.
    */
    static inline real penetrationOnAxis(
        const Box& one,
        const Box& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    );


    /*
        This function checks if the two boxes overlap
        along the given axis. The final parameter toCentre
        is used to pass in the vector between the boxes centre
        points, to avoid having to recalculate it each time.
    */
    static inline bool overlapOnAxis(
        const Box& one,
        const Box& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    );


    bool boxAndBox(
        const Box one,
        const Box& two
    );


    static inline bool tryAxis(
        const Box& one,
        const Box& two,
        Vector3D axis,
        const Vector3D& toCentre,
        unsigned int index,
        // These values may be updated
        real& smallestPenetration,
        unsigned int& smallestCase
    );


    void fillPointFaceBoxBox(
        const Box& one,
        const Box& two,
        const Vector3D& toCentre,
        std::vector<Contact>& contacts,
        unsigned best,
        real pen
    );


    static inline Vector3D contactPoint(
        const Vector3D& pOne,
        const Vector3D& dOne,
        real oneSize,
        const Vector3D& pTwo,
        const Vector3D& dTwo,
        real twoSize,
        bool useOne
    );


    bool sphereAndSphere(
        const Ball& one,
        const Ball& two
    );


    unsigned int boxAndBox(
        const Box& one,
        const Box& two,
        std::vector<Contact>& contacts
    );


    unsigned int sphereAndSphere(
        const Ball& one,
        const Ball& two,
        std::vector<Contact>& data
    );


    unsigned int boxAndSphere(
        const Box& box,
        const Ball& sphere,
        std::vector<Contact>& data
    );


    void generateContacts(
        RigidObject& one,
        RigidObject& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    );

}

#endif