/*
	The bounding volume hierarchy is a tree that contains, at its leaves,
	all the rigid bodies in a world, along with their bounding volumes, and
	is used in order to quickly determine which bounding volumes are
	potentially in contact for the corase collision detection engine, which
	is faster but also less accurate than the real collision detection
	engine. This approach lessens the load on the accurate but slow collision
	detection engine.
	There are several ways to build the tree, which takes into account how
	and when new bodies can be added, and which bodies are best grouped
	together as children of the same parent.
		The first approach is bottom-up, in which, using a list of all the
		rigid bodies of the world, we pick the two best canditates to be
		grouped together based on their bounding volume, create a parent
		node for them, and place the parent node back in the list with its
		bounding volume, and repeat the process untill we have one node left,
		the root. Bodies are usually grouped together when they are close.
		The second approach is similar, but is a top-down approach. We
		likewise start with a list, but this time, partition it into halves,
		and continuously do that to all sublists until all partitions contain
		no more than one rigid body. Again, we usually divide groups based
		on how closed the objects are clustered together.
		The final approach is the insertion approach. We build the tree by
		inserting rigid bodies into it one by one. We can choose a node
		in which to insert our new body, and then recursively descend the
		subtree, choosing obtimal subtrees based on the rigid body we have
		and while keeping an eye on the balance of the tree (balanced trees
		are faster to work with). When we arrive at a leaf node, we replace
		it by a parent not containing any body, and then place the old body
		and the new body we just added as its children, two new leaf nodes.
	For this system, we use the last approach, as it allows us to build the
	tree dynamically (e.g. there is no need to build the tree during a
	loading time before the physics can be rendered). It also allows us to
	add and remove objects from the hierarchy in real time.
	The insertion and removal of a node are already functions in the BHVNode
	class (the removal is just the destructor, although it also handles things
	like recalculating the bounding volumes of the ancestors of the node,
	just like the insertion function), where they operate on inserting nodes
	only in the subtree descending from the calling object node, or removing
	the calling object node itself. This class provides the ability to add
	and remove nodes from the entire tree, so the insertion function always
	calls the insert function of the root node, while the remove function
	calls whichever node (or entire subtree for non-leaf nodes) we want
	removed. So this class is little more than a wrapper for the root node
	of the tree.
*/

#ifndef BOUNDING_VOLUME_HIERARCHY_H
#define BOUNDING_VOLUME_HIERARCHY_H

#include "BVHNode.h"

namespace pe {

	template<class BoundingVolumeClass>
	class BoundingVolumeHierarchy {

	private:

		// Represents the root of the tree
		BVHNode* root;

	public:



	};
}

#endif