
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

		
		// Addition of two vectors (component-wise)
		Vector2D operator+(const Vector2D& vector) const {
			return Vector2D(x + vector.x, y + vector.y);
		}


		void operator+=(const Vector2D& vector) {
			x += vector.x;
			y += vector.y;
		}


		// Subtraction of two vectors (component-wise)
		Vector2D operator-(const Vector2D& vector) const {
			return Vector2D(x - vector.x, y - vector.y);
		}


		void operator-=(const Vector2D& vector) {
			x -= vector.x;
			y -= vector.y;
		}


		void display() const {
			std::cout << "x: " << x << ", y: " << y << "\n";
		}
	};
}


#endif

