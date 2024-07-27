/*
    Header file for the Resizable concept.
    Bounding volumes can be used for frustum culling, fine collision
    detection, or coarse collision detection. In the case of coarse
    collision detection, the volume shapes are exceedingly simple but
    may fit the object rather loosely. In any case, the coarse collision
    detection is faster but less accurate than the later fine collision
    detection phase, which is only done on objects that were colliding
    in the coarse phase.
    A bounding volume hierarchy is used to efficiently test
    the collision of every pair of objects on screen during the coarse
    collision detection phase. The nodes of the tree are the bounding
    volumes encompassing the objects. The leaf nodes are the bounding
    volumes that contain an object, while the parent nodes are the
    smallest bounding volume containing all the children (this is used
    to speed up the collision detection).
    Objects in this tree are often added, removed, rotated, and moved,
    so the bounding volumes need to have certain functions that enable
    this functionality.
    This concept includes all the necessary functions a bounding volume
    must have in order to be usable as a bounding volume hierarchy (BVH)
    node. Because the shape of the bounding volume is unknown however,
	these functions are pure virtual.

	There are two reasons this is a concept and not an interface:

	1 - An interface wouldn't work because some of the functions we need
	require sending an argument of the same type, like the overlaps()
	function, which needs the derived class type to be sent to check
	if it overlaps with the calling volume. With an interface, the
	virtual overlap function must take the base class type, not the derived
	class type. Concepts solve this problem, by allowing us to specify
	that functions must take the same type as the concept template type.
	A solution for this issue without using concepts would have involved
	using a template for the type of the object sent to the function in the
	base class, or doing dynamic casting in the derived class, neither of
	which is as clean and efficient as the use of a concept.

	2 - Moreover, the bounding volume hierarchy class will be templated,
	where the template type must be resizable. In C++, there is no way
	to force a template type to inherit from an interface, but concepts
	can be used to force the template to have certain functions,
	such as the ones we need here, which makes them perfect for this use
	case.
*/

#ifndef RESIZABLE_H
#define RESIZABLE_H

#include <concepts>
#include "accuracy.h"

namespace pe {

    template<typename T>
    concept Resizable = requires(T t, const T & otherVolume) {
        
        /*
            Constructor that creates a bounding volume that just barily
            contains two other bounding volumes(the calling volume
            and the one that is sent).
            Used in creating a parent of two nodes in the bounding volume
            hierarchy tree. Note that this isn't the smallest volume
            containing the polyhedra of its descendants, but rather, the
            smallest volume encompassing its descendants' bounding volumes
            (which is best performance-wise).
        */
        { T{ otherVolume, otherVolume } } -> std::same_as<T>;

        /*
            Returns the volume of the bounding volume, whatever it may be.
            The volume of the bounding volume is needed in some calculations.
        */
        { t.getVolume() } -> std::convertible_to<real>;

        /*
            Returns true if the calling object volume (whatever it may be)
            overlaps with the volume sent.
        */
        { t.overlaps(&otherVolume) } -> std::convertible_to<bool>;

        /*
            Returns how much the bounding volume sphere would have to grow
            in order to encompass a new bounding volume added now to it
            (given as a parameter).
            Used in order to determine where to insert a new body in the
            BVH tree (we prioritize keeping the parent volumes as small as
            possible to limit false positive collisions).
            We don't necessarily have to return the volume as a measure of
            growth. In the case of a bounding sphere, the metric used could
            be the new radius. Any measure that grows as the volume grows and
            shrinks as the volume shrinks works in this case.
        */
        { t.getNewGrowth(otherVolume) } -> std::convertible_to<real>;
    };

}

#endif
