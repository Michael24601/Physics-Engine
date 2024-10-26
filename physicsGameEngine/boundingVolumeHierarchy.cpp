
#include "boundingVolumeHierarchy.h"

using namespace pe;


void BoundingVolumeHierarchy::insert(
	RigidObject* object,
	const Vector3D& centre,
	real radius
) {

	/*
		The sphere containing the object is centred where the object's
		bounding volume is. Its radius is the furthest point in the
		bounding volume from that centre.
	*/
	BVHSphere sphere(centre, radius);

	// If the tree is empty, create a root with the body in it, and no parent
	if (root == nullptr) {
		root = new BVHNode(object, sphere, nullptr);
	}
	// Otherwise, call the insert function at the root
	else {
		root->insertInSubtree(object, sphere);
	}
}


void BoundingVolumeHierarchy::removeSubtree(BVHNode* node) {
	node->~BVHNode();
}



unsigned int BoundingVolumeHierarchy::auxGetPotentialContacts(
	const BVHNode* node,
	PotentialContact* contacts,
	unsigned int limit
) const {
	if (node == nullptr || limit == 0) return 0;

	unsigned int count = 0;

	// Traversing left subtree
	count += auxGetPotentialContacts(node->children[0], contacts, limit);
	limit -= count;

	// Current node
	if (limit > 0) {
		count += node->getPotentialContacts(contacts + count, limit);
		limit -= count;
	}

	// Traversing the right subtree
	count += auxGetPotentialContacts(node->children[1], contacts + count, limit);

	return count;
}


unsigned int BoundingVolumeHierarchy::getPotentialContacts(
	PotentialContact* contacts, 
	unsigned int limit
) const {
	// Calling the auxiliary function with the root node
	return auxGetPotentialContacts(root, contacts, limit);
}