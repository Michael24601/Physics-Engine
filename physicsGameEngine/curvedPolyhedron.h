
#ifndef CURVED_POLYHEDRON_H
#define CURVED_POLYHEDRON_H

#include "polyhedron_2.h"

namespace pe {

	class CurvedPolyhedron : Polyhedron {

	public:


		/*
			Returns the normals and vertices for of each vertex in each
			face in the curved polyhedron. However, the faces may be
			curved, and when that is tha case, the normals might not
			match the face's. 
			The function should be overridden by subclasses that know
			which faces are curved, and which are not, and return the
			normal data accordingly.
		*/
		virtual void getSmoothPolyhedronData(
			std::vector<Vector3D>* vertices,
			std::vector<Vector3D>* normals
		) const = 0;
	};
}

#endif