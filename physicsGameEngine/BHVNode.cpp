
#include "BHVNode.h"

using namespace pe;

template<class BoundingVolumeClass>
bool BVHNode<BoundingVolumeClass>::overlaps(
	const BVHNode<BoundingVolumeClass>* other) const {
	/*
		To check that two nodes overlap is to check that their bounding
		volumes intersect.
	*/
	return volume->overlaps(other->volume);
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


template<class BoundingVolumeClass>
unsigned BVHNode<BoundingVolumeClass>::getPotentialContactsWith(
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
			contacts->body[0] = body;
			contacts->body[1] = other->body;
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
		if (other->isLeaf() ||
			(!isLeaf() && volume->getSize() >= other->volume->getSize())){
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