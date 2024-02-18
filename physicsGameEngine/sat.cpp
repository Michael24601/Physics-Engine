
#include "sat.h"

using namespace pe;


int pe::whichSide(const Polyhedron& C, const Vector3D& P,
	const Vector3D& D) {

	/*
		The vertices are projected to the form P + t * D. The return
		value is +1 if all t > 0, = 1 if all t < 0, but 0 otherwise, in
		which case the line splits the polygon projection.
	*/
	int positive = 0, negative = 0;
	for (int i = 0; i < C.globalVertices.size(); i++) {
		// Project a vertex onto the line.
		real t = D.scalarProduct(C.globalVertices[i] - P);
		if (t > 0) {
			positive++;
		}
		else if (t < 0) {
			negative++;
		}
		if (positive && negative) {
			/*
				The polygon has vertices on both sides of the line, so the
				line is not a separating axis. Time is saved by not having
				to project the remaining vertices.
			*/
			return 0;
		}
	}
	// Either positive > 0 or negative > 0 but not both are positive.
	return (positive > 0 ? 1 : -1);
}


bool pe::testIntersection(const Polyhedron& C0, const Polyhedron& C1) {
	/*
		Test faces of C0 for separation. Because of the counter clockwise
		ordering, the  projection interval for C0 is [T,0] whereT<0.
		Determine whether C1 is on the  positive side of the line.
	*/

	/*
		On the first frame, the face data may not yet have been calculated
		(it's calculated once per frame, and there's no guarantee the code
		calculating it is executed before the collision detection), so
		if that's the case, we automatically return false (no collision).
		This only applies to the first frame.
	*/
	if (C0.faces.size() == 0 || C1.faces.size() == 0) {
		return false;
	}

	for (int i = 0; i < C0.faces.size(); i++) {
		pe::Vector3D P = C0.faces[i]->getVertex(0);
		pe::Vector3D N = C0.faces[i]->getNormal(); // outward pointing
		if (whichSide(C1, P, N) > 0) {
			// C1 is entirely on the positive side of the line P + t * N.
			return false;
		}
	}
	/*
		Test faces of C1 for separation. Because of the counterclockwise
		ordering, the  projection interval for C1 is [T,0] whereT<0.
		Determine whether C0 is on the  positive side of the line.
	*/
	for (int i = 0; i < C1.faces.size(); ++i) {
		pe::Vector3D P = C1.faces[i]->getVertex(0);
		pe::Vector3D N = C1.faces[i]->getNormal(); // outward pointing
		if (whichSide(C0, P, N) > 0) {
			// C1 is entirely on the positive side of the line P + t * N.
			return false;
		}
	}

	/*
		Test cross products of pairs of edge directions, one edge
		direction from each polyhedron.
	*/
	for (int i0 = 0; i0 < C0.globalVertices.size(); ++i0) {
		pe::Vector3D D0 = C0.edges[i0]->getVertex(1) -
			C0.edges[i0]->getVertex(0);
		pe::Vector3D P = C0.edges[i0]->getVertex(0);
		for (int i1 = 0; i1 < C1.globalVertices.size(); ++i1) {
			pe::Vector3D D1 = C1.edges[i1]->getVertex(1) -
				C1.edges[i1]->getVertex(0);
			pe::Vector3D N = D0.vectorProduct(D1);
			if (N != pe::Vector3D(0, 0, 0)) {
				int side0 = whichSide(C0, P, N);
				if (side0 == 0) {
					continue;
				}
				int side1 = whichSide(C1, P, N);
				if (side1 == 0) {
					continue;
				}
				if (side0 * side1 < 0) {
					/*
						The projections of C0 and C1 onto the line
						P + t * N are on opposite sides of the projection
						of P.
					*/
					return false;
				}
			}
		}
	}

	return true;
}
