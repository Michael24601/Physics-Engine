/*
	Header file for class representing a node in the BHV tree,
	and associated structures.
*/

#ifndef BOUNDING_VOLUME_HIERARCHY_NODE_H
#define BOUNDING_VOLUME_HIERARCHY_NODE_H

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
		to be used.
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
			Pointer to the parent node in the hierarchy(NULL if it's the
			root). Used in order to modify the bounding volumes of the
			ancestors (chain of parents up to to the root) when a node
			is inserted or deleted.
		*/
		BVHNode* parent;

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
			between bodies in the left subtree and right subtree alone,
			as that is later checked when this function is called in their
			children nodes, up to the leaves, which contain one body
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

		/*
			Inserts the given rigid body into the subtree underneath the
			calling node. This function was added in order to allow for the
			insertion approach of building a tree, where the insert function
			is called for the root node whenever we need to add a new body
			into the tree. Note that the bounding volume of any object
			depends on its geometry, and thus can't be deduced from the rigid
			body class involved only in its movement aspect, so the burden
			of creating it does not lie with this class (although the
			creation of parent bounding volumes does, and this function
			will do just that everytime we add a new body as a leaf node,
			necessitating that a parent be created for it to replace
			whichever leaf there was). All the ancestors of the new node
			(which we can access by recursively or iteratively going back up
			to the parent until we reach the root) then recalculate their
			bounding volume, as the new node may not be in the volume they
			already occupy. We can do that because each node holds a pointer
			to its parent. We can do that with a simple loop above the newly
			removed node until we reach the root.
			When inserting the node in a particulat subtree, we descend it
			in such a way as to minimize the amount we would need to expand
			the bounding volume of the node.
			While this function tries to insert the bodies in the best
			positions based on their position in space, it does not have the
			power to rebalance the tree to improve performance.
		*/
			void insertInSubtree(RigidBody* rigidBody,
				const BoundingVolumeClass& boundingVolume);

		/*
			Deletes the calling object's node, and the entire subtree
			underneath it (the bodies themselves are not deleted but
			simply removed from the tree). The function also has the effect
			of removing the sibling of the node we delete and making it the
			parent instead. It also forces every ancestor (which we can
			access by iteratively or recusively going back up to the parent
			until we reach the root) to recalculate their bounding volume as
			it might have shrinked. We can do that using the parent pointer
			in each node with a simple loop above the newly removed node
			until we reach the root.
			This function does not have the ability to rebalance the tree
			after the removal however, which might improve performance.
			Note that while in the hierarchy as a whole, this is the
			remove function, it acts as the destrructor of this class, 
			as the node itself is deleted.
		*/
		~BVHNode();

		/*
			For non-leaf nodes, this method recalculates the bounding volume
			based on the bounding volumes of its children.
		*/
		void recalculateBoundingVolume(bool recurse = true)

	};
}

#endif