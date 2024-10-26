
#include "BVHNode.h"

using namespace pe;


bool BVHNode::overlaps(
	const BVHNode* other
) const {
	/*
		To check that two nodes overlap is to check that their bounding
		volumes intersect.
	*/
	return boundingVolume.overlaps(&(other->boundingVolume));
}


unsigned int BVHNode::getPotentialContacts(
	PotentialContact* contacts, unsigned int limit
) const {
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
	return children[0]->getPotentialContactsWith(
		children[1],
		contacts,
		limit
	);
}

// Function definitions (included in the header because of the template)

unsigned int BVHNode::getPotentialContactsWith(
	BVHNode* other,
	PotentialContact* contacts,
	unsigned int limit
) const {
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
			contacts->object[0] = object;
			contacts->object[1] = other->object;
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
		if (other->isLeaf() || (!isLeaf() && boundingVolume.getSize() >=
			other->boundingVolume.getSize())) {

			unsigned int  count = children[0]->getPotentialContactsWith(
				other, contacts, limit
			);
			if (limit > count) {
				return count + children[1]->getPotentialContactsWith(
					other, contacts + count, limit - count
				);
			}
			else {
				return count;
			}
		}
		else {
			unsigned int count = getPotentialContactsWith(
				other->children[0], contacts, limit
			);
			if (limit > count) {
				return count + getPotentialContactsWith(
					other->children[1], contacts + count, limit - count
				);
			}
			else {
				return count;
			}
		}
	}
}


void BVHNode::insertInSubtree(
	RigidObject* object,
	const BVHSphere& boundingVolume
) {

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
		children[0] = new BVHNode(
			this->object,
			this->boundingVolume,
			this
		);
		// Child two holds the new body
		children[1] = new BVHNode(
			object,
			boundingVolume,
			this
		);

		// And we remove the body from the node that now became a parent
		this->object = nullptr;

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
	else {
		/*
			Chooses the bounding volume, left or right, changes the least,
			and recursively calls the insert function until we arrive at a
			leaf.
		*/
		if (children[0]->boundingVolume.getNewGrowth(boundingVolume) >
			children[1]->boundingVolume.getNewGrowth(boundingVolume)) {
			children[0]->insertInSubtree(object, boundingVolume);
		}
		else {
			children[1]->insertInSubtree(object, boundingVolume);
		}
	}
}


BVHNode::~BVHNode() {
	/*
		This first step is about turning the sibling into the parent. Since
		the tree is always full, only the root doesn't have a sibling, so
		we only do this step if the node has a parent (isn't the root).
	*/
	if (parent) {
		// Find our sibling.
		BVHNode* sibling;
		if (parent->children[0] == this) {
			sibling = parent->children[1];
		}
		else {
			sibling = parent->children[0];
		}
		// Write its data to our parent.
		parent->boundingVolume = sibling->boundingVolume;
		parent->object = sibling->object;
		parent->children[0] = sibling->children[0];
		parent->children[1] = sibling->children[1];

		// We then delete the sibling
		sibling->parent = nullptr;
		sibling->object = nullptr;
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
		delete children[1];
	}
}


void BVHNode::recalculateBoundingVolume() {
	/*
		As mentioned earlier, this function can only recalculate the
		volume of a parent.
	*/
	if (!isLeaf()) {
		/*
			The new volume is then calculated by creating a new bounding
			volume now encompassing its new children.
		*/
		boundingVolume = BVHSphere(
			children[0]->boundingVolume,
			children[1]->boundingVolume
		);
		/*
			Recurse up the tree as long as we're not at the root
			(no parent).
		*/
		if (parent) {
			parent->recalculateBoundingVolume();
		}
	}
}
