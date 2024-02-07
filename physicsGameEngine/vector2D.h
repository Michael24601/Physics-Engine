
#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include "accuracy.h"

namespace pe {

	class Vector2D {

	public:

		// Vector coordinates 
		real x;
		real y;

		// No arg constructor
		Vector2D() : x{ 0.0f }, y{ 0.0f } {}

		// Argumented constructor
		Vector2D(real x, real y) : x{ x }, y{ y } {}
	};
}

#endif

