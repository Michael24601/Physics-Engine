/*
	Header file for class representing a node in the BHV tree,
	and associated structures.
*/

#ifndef BOUNDING_VOLUME_HIERARCHY_H
#define BOUNDING_VOLUME_HIERARCHY_H

#include "rigidBody.h"

namespace pe {

	/*
		The BHV hierarchy is used not to generate a list of bodies in contact,
		but rather, a list of potential contacts based on a coarse (innacurate
		but fast) collision detection system, which are then sent to the
		accurate but slow collision detection system using this struct,
		which store the two bodies that may need to be checked.
	*/
	struct PotentialContact {
		RigidBody* body[2];
	};

	/*
		The node is templated to allow for multiple bounding volume types
		to be used. The bounding volume type specifies the simple shape
		used in checking for a potential collision, like a sphere shape
		or prism shape. The bodies that are potentially colliding are
		inside these bounding spaces, are can only be colliding if the
		spaces themselves interpenetrate. The bounding volumes thus
		sometimes give false positives, where the spaces collide but not
		the objects, as the space won't always be a perfect fit. However,
		the simplicty of the space also makes the collision easier and
		faster to calculate, removing some of the burden from the slower
		sophisticated collision detection system. All in all, the coarse
		collision detection system's bounding volume spaces need to be
		accurate enough to eliminate most false positives, but not so
		accurate as to slow down the coarse collision detection, eliminating
		its original purpose as a fast but innacurate detection system.
	*/
	template<class BoundingVolumeClass>
	class BVHNode {

	public:
		// Children of this node
		BVHNode* children[2];

		// The bounding volume encompassing all children of this node
		BoundingVolumeClass volume;

		/*
			In the bounding volume tree algorithms we are working with in
			this system, only leaf nodes hold actual bodies, and the rest
			only have the volume space encompassing its children's bodies,
			or in case the children aren't leaves, whichever bodies exist
			in its subtree. In this case, this variable is left null.
		*/
		RigidBody* body;

		/*
			Checks if this node is a leaf or not, by checking if it has a
			body in its bounding space.
		*/
		bool isLeaf() const {
			return (body != NULL);
		}

		/*
			Checks all of the potential contacts between the bodies in the
			left and right subtree of this node. This means that to get the
			full list of potential contacts, we can start with the upper
			nodes and work our way down to the lower ones. The upper nodes
			only have to check for intersections between bodies in their left
			and right children, without having to check for intersections
			between bodies the left subtree and right subtree respectively,
			as that is latter checked when this function is called in the
			children nodes, up to the leaf nodes, which contain one body
			each and thus don't need to have the function called for them.
			The contacts parameter is filled with an array of potential body
			contacts in case their bounding volume spaces collide, and the
			number of generated potential contacts is also returned. A limit
			may be placed to limit the number of possible contacts to
			generate.
		*/
		unsigned int getPotentialContacts(PotentialContact* contacts,
			unsigned int  limit) const;


		/*
			Checks if there is some overlapping with another node in the BHV
			tree (their bouding volumes meaning).
		*/
		bool overlaps(const BVHNode<BoundingVolumeClass>* other) const;

		/*
			Fills the given array, up to a limit, with potential contacts
			between this node and the given node, meaning any potential
			contacts between rigid bodies (leaf nodes) in their respective
			subtrees. Again, we do not need to check, at this node's stage,
			for intersections within each subtree individually, as that
			will be checked as we move down the tree and call this function
			at different nodes.
		*/
		unsigned int getPotentialContactsWith(
			const BVHNode<BoundingVolumeClass>* other,
			PotentialContact* contacts, unsigned int limit) const;
	};
}

#endif