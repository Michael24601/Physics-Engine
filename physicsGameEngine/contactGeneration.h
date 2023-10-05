/*
    Header file for functions that perform contact generation after a
    collision has been detected. These tests are expensive, so they should
    only be done after a less costly collision detection system like SAT
    has determined that there is a collision. 
    There are 4 types of collisions we would like to detect:
    face to face,
    edge to face,
    point to edge,
    and edge to edge.
    The former two can always be handled by instead replacing them by the
    latter two. This only applied for straight surfaces though. For curved
    faces, we still need to do all four. This header only contains contact
    generation between convex polyhedra, so the only contacts generated
    are of the latter two types. Other types of contact like point to point
    aren't common enough that omitting them is noticeable.
*/

#include "primitive.h"
#include "contact.h"

namespace pe {

    /*
        Returns a contact if this point intersects the given convex
        polyhedron. We send the second polyhdron as an argument as we will
        need to set its adress in the resulting contact object.
    */
    unsigned pointAndConvexPolyhedron(const Primitive& polyhedron,
        const Vector3D& point, const Primitive& secondPolyhedron,
        std::vector<Contact>& data);


    /*
        Returns a contact if these two egdes in two different polyhedra
        intersect. We send the two polyhdra as an argument as we will
        need to set their adress in the resulting contact object.
    */
    unsigned edgeToEdge(const Edge& edgeA, const Edge& edgeB,
        const Primitive& p1, const Primitive& p2,
        std::vector<Contact>& data);


    /*
        In this function we test all vertices in both polyhedra against the
        other, as well as all combinations of edges. Trading accuracy for
        performance, we then resolve only one of them. There are several
        criteria to follow in choosing which contact to resolve for this
        collision. For instance, we can think of the one with the most
        penetration depth as being the most urgent.
        In our case, we choose the one closest to the velocity (travel)
        direction of the two objects.
    */
    bool returnMaxContact(const Primitive& p1, const Primitive& p2,
        std::vector<Contact>& contactsToBeResolved);
}