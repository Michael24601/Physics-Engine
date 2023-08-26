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
		RigidBody* rigidBody[2];
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
		BoundingVolumeClass boundingVolume;

		/*
			In the bounding volume tree algorithms we are working with in
			this system, only leaf nodes hold actual bodies, and the rest
			only have the volume space encompassing its children's bodies,
			or in case the children aren't leaves, whichever bodies exist
			in its subtree. In this case, this variable is left null.
		*/
		RigidBody* rigidBody;

		/*
			Pointer to the parent node in the hierarchy(NULL if it's the
			root). Used in order to modify the bounding volumes of the
			ancestors (chain of parents up to to the root) when a node
			is inserted or deleted.
		*/
		BVHNode* parent;

		/*
			Argumented constructor that sets the parent, body, and bounding
			volumes while keeping the children null. If the node is not a 
			leaf node, the first parameter should be NULL.
		*/
		BVHNode(RigidBody* rigidBody,
			const BoundingVolumeClass& boundingVolume, BVHNode* parent) :
			rigidBody{ rigidBody }, boundingVolume{boundingVolume},
			parent{parent} {
			children[0] = children[1] = NULL;
		}

		/*
			Checks if this node is a leaf or not, by checking if it has a
			body in its bounding space.
		*/
		bool isLeaf() const {
			return (rigidBody != NULL);
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
			Note that because we always replace a leaf by a parent of the old
			leaf and the new node, the hierarchy is always a full binary tree
			where each node has 0 or 2 children, but never 1.
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
			Because we are always moving the remaining sibling up, the
			hierarchy remains a full binary tree where each node has 0 or
			2 children.
			This function does not have the ability to rebalance the tree
			after the removal however, which might improve performance.
			Note that while in the hierarchy as a whole, this is the
			remove function, it acts as the destrructor of this class, 
			as the node itself is deleted. 
		*/
		~BVHNode();

		/*
			Realculates the bounding volume of the node, granted it is
			not a leaf, and the bounding volumes of all of its ancestors
			(chain of parents). Used after removing or inserting a new node.
		*/
		void recalculateBoundingVolume();
	};


	template<class BoundingVolumeClass>
	bool BVHNode<BoundingVolumeClass>::overlaps(
		const BVHNode<BoundingVolumeClass>* other) const {
		/*
			To check that two nodes overlap is to check that their bounding
			volumes intersect.
		*/
		return boundingVolume->overlaps(other->volume);
	}


	template<class BoundingVolumeClass>
	unsigned int BVHNode<BoundingVolumeClass>::getPotentialContacts(
		PotentialContact* contacts, unsigned int limit) const {
		/*
			The function returns the number of contacts in this node's subtree,
			so if the node is a leaf or the limit is 0, the array should not
			be filled with any contatcs, and 0 is returned.
		*/
		if (isLeaf() || limit == 0) return 0;

		/*
			Otherwise, the list of all contacts in the subtree of this node
			is just the list of potential contacts that bodies in the first
			child may have with bodies of the second child.
		*/
		return children[0]->getPotentialContactsWith(children[1], contacts,
			limit);
	}

	// Function definitions (included in the header because of the template)

	template<class BoundingVolumeClass>
	unsigned int BVHNode<BoundingVolumeClass>::getPotentialContactsWith(
		const BVHNode<BoundingVolumeClass>* other,
		PotentialContact* contacts, unsigned limit) const {
		/*
			If the bounding volumes of the two nodes don't overlap, then
			none of the bodies in the left subtree will overlap with a body in
			the right subtree (again, when called in this node, there is no
			need to check for overlapping bodies in each subtree alone as that
			is handled by lower nodes in the tree).
		*/
		if (!overlaps(other) || limit == 0) {
			return 0;
		}
		// Otherwise, if they do overlap
		else {
			/*
				If both nodes are leaf nodes, then there is one intersection
				(those two).
			*/
			if (isLeaf() && other->isLeaf()) {
				contacts->rigidBody[0] = rigidBody;
				contacts->rigidBody[1] = other->rigidBody;
				return 1;
			}
			/*
				If at least one of them isn't a leaf, then an intersection
				between the bounding volumes of the nodes doesn't necessarily
				guarantee that the bounding volumes of the rigid bodies at the
				leaf nodes will overlap, so we need to recursively go down the
				tree until we reach all the leaf nodes.
				If one is a leaf and the other isn't, we recursivly repeat the
				function while descending into the non-leaf, until we reach
				leaves and either return or not return a contact.
				If neither is a leaf, then we descend into the longer branch.
				This guarantees we always reach a point where we are comparing
				two leaves.
			*/
			if (other->isLeaf() || (!isLeaf() && boundingVolume->getSize() >=
				other->boundingVolume->getSize())) {

				unsigned int  count = children[0]->getPotentialContactsWith(
					other, contacts, limit);
				if (limit > count) {
					return count + children[1]->getPotentialContactsWith(
						other, contacts + count, limit - count);
				}
				else {
					return count;
				}
			}
			else {
				unsigned int count = getPotentialContactsWith(
					other->children[0], contacts, limit);
				if (limit > count) {
					return count + getPotentialContactsWith(
						other->children[1], contacts + count, limit - count);
				}
				else {
					return count;
				}
			}
		}
	}


	template<class BoundingVolumeClass>
	void BVHNode<BoundingVolumeClass>::insertInSubtree(RigidBody* rigidBody,
		const BoundingVolumeClass& boundingVolume) {
		/*
			If the node we have arrived at after recursively descending the tree
			is a leaf, we replace this leaf by a parent and make the old leaf
			and the new node its children. Neither one of the nodes will have
			children at this point so they are left null.
		*/
		if (isLeaf()) {
			/*
				Because we recursively call this function through the child
				nodes, the calling object at this point (the old leaf) will
				be the parent of the two nodes, and a copy of it is used for
				the first of the two children, while the new node is used for
				the second.
			*/
			children[0] = new BVHNode<BoundingVolumeClass>(this->rigidBody,
				this->boundingVolume, this);
			// Child two holds the new body
			children[1] = new BVHNode<BoundingVolumeClass>(rigidBody,
				boundingVolume, this);

			// And we remove the body from the node that now became a parent
			this->rigidBody = nullptr;

			// And we then recalculate the bounding volumes of all the ancestors
			recalculateBoundingVolume();
		}
		/*
			Otherwise, if we haven't yet arrived at a leaf, then we need to
			choose to go left or right. Each parent always has two children
			or no children (leaf), so we always have a choice to make, and the
			choice always depends on whichever side changes their volume less
			due to the added node.
		*/
		else
		{
			/*
				Chooses the bounding volume, left or right, changes the least,
				and recursively calls the insert function until we arrive at a
				leaf.
			*/
			if (children[0]->boundingVolume.getNewGrowth(boundingVolume) >
				children[1]->boundingVolume.getNewGrowth(boundingVolume)) {
				children[0]->insertInSubtree(rigidBody, boundingVolume);
			}
			else {
				children[1]->insertInSubtree(rigidBody, boundingVolume);
			}
		}
	}


	template<class BoundingVolumeClass>
	BVHNode<BoundingVolumeClass>::~BVHNode<BoundingVolumeClass>()
	{
		/*
			This first step is about turning the sibling into the parent. Since
			the tree is always full, only the root doesn't have a sibling, so
			we only do this step if the node has a parent (isn't the root).
		*/
		if (parent) {
			// Find our sibling.
			BVHNode<BoundingVolumeClass>* sibling;
			if (parent->children[0] == this) {
				sibling = parent->children[1];
			}
			else {
				sibling = parent->children[0];
			}
			// Write its data to our parent.
			parent->boundingVolume = sibling->boundingVolume;
			parent->rigidBody = sibling->body;
			parent->children[0] = sibling->children[0];
			parent->children[1] = sibling->children[1];

			// We then delete the sibling
			sibling->parent = nullptr;
			sibling->rigidBody = nullptr;
			sibling->children[0] = nullptr;
			sibling->children[1] = nullptr;
			delete sibling;

			// And then recalculate the ancestors bounding volumes
			parent->recalculateBoundingVolume();
		}
		/*
			We then delete the actual node by recursing.This stops when a node
			no longer has children(is a leaf).
		*/
		if (children[0]) {
			children[0]->parent = nullptr;
			delete children[0];
		}
		if (children[1]) {
			children[1]->parent = nullptr;
			delete children[0];
		}
	}


	template<class BoundingVolumeClass>
	void BVHNode<BoundingVolumeClass>::recalculateBoundingVolume() {
		/*
			As mentioned earlier, this function can only recalculate the
			volume of a parent.
		*/
		if (!isLeaf()) {
			/*
				The new volume is then calculated by creating a new bounding
				volume now encompassing its new children.
			*/
			boundingVolume = BoundingVolumeClass(children[0]->boundingVolume,
				children[1]->boundingVolume);
			/*
				Recurse up the tree as long as we're not at the root
				(no parent).
			*/
			if (parent) {
				parent->recalculateBoundingVolume();
			}
		}
	}

}

#endif