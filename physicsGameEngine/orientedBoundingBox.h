/*
	Header file for class representing an oriented bounding box.
*/

#ifndef ORIENTED_BOUNDING_BOX_H
#define ORIENTED_BOUNDING_BOX_H

#include "boundingBox.h"
#include "axisAlignedBoundingBox.h"
#include <Eigen/Dense>

namespace pe {

	class OrientedBoundingBox : public BoundingBox {

	public:

		OrientedBoundingBox(Polyhedron* polyhedron);

		void update(Polyhedron* polyhedron) override;
	};
}

#endif