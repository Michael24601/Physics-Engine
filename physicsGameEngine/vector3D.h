// Header file for 3D vector class

#ifndef VECTOR_3D
#define VECTOR_3D

#include <math.h>
#include "accuracy.h"
#include <iostream>

namespace pe {

	class Matrix3x4;

	class Vector3D {

	public:

		// Static constants
		static const Vector3D ZERO;
		static const Vector3D RIGHT;
		static const Vector3D UP;
		static const Vector3D FORWARD;

		// Vector coordinates 
		real x;
		real y;
		real z;

		// No arg constructor
		Vector3D() : x{0.0f}, y{0.0f}, z{0.0f}{}

		// Argumented constructor
		Vector3D(real x, real y, real z) : x{ x }, y{ y }, z{ z } {}

		// Inverts vector coordinates (multiplies by -1)
		void invert() {
			x = -x;
			y = -y;
			z = -z;
		}


		inline Vector3D inverse() const {
			return Vector3D(-x, -y, -z);
		}


		inline Vector3D operator-() const {
			return inverse();
		}


		// Length of vector
		real magnitude() const {
			return realSqrt(x * x + y * y + z * z);
		}

		// Magnitude squared (avoid expensive call to sqrt)
		real magnitudeSquared() const {
			return x * x + y * y + z * z;
		}

		// Turns non-zero vector into one with unit length (1)
		void normalize() {
			real m = magnitude();
			if (m > 0) {
				// multiplies vector by scalar
				(*this) *= ((real)1) / m;
			}
		}

		// Retunrs normalized vector without modifying the object
				// Turn non-zero vector into one with unit length (1)
		Vector3D normalized() const {
			Vector3D newVector = *this;
			newVector.normalize();
			return newVector;
		}

		// Multiplication by a scalar (scaling the vector)
		Vector3D operator*(const real scalar) const {
			return Vector3D(x * scalar, y * scalar, z * scalar);
		}

		void operator*=(const real scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
		}

		// Addition of two vectors (component-wise)
		Vector3D operator+(const Vector3D& vector) const {
			return Vector3D(x + vector.x, y + vector.y, z + vector.z);
		}

		void operator+=(const Vector3D& vector) {
			x += vector.x;
			y += vector.y;
			z += vector.z;
		}


		// Subtraction of two vectors (component-wise)
		Vector3D operator-(const Vector3D& vector) const {
			return Vector3D(x - vector.x, y - vector.y, z - vector.z);
		}

		void operator-=(const Vector3D& vector) {
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
		}

		bool operator==(const Vector3D& vector) {
			return (
				x == vector.x
				&& y == vector.y
				&& z == vector.z
			);
		}

		bool operator!=(const Vector3D& vector) {
			return (
				x != vector.x
				|| y != vector.y
				|| z != vector.z
				);
		}

		// Adds a scaled vector to the calling object
		void linearCombination(const Vector3D& vector, const real scalar) {
			x += scalar * vector.x;
			y += scalar * vector.y;
			z += scalar * vector.z;
		}

		// Component multiplication of two vectors (component-wise)
		Vector3D componentProduct(const Vector3D& vector) const {
			return Vector3D(x * vector.x, y * vector.y, z * vector.z);
		}

		void componentProductUpdate(const Vector3D& vector) {
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
		}

		/*
			Scalar or dot product. Used in calculating the angle between
			two vectors, as well as the magnitude of one vector in the
			direction of another.
		*/
		real scalarProduct(const Vector3D& vector) const {
			return x * vector.x + y * vector.y + z * vector.z;
		}

		/*
			Vector or cross product (not commutative). Used to calculate the
			angle as well as the magnitude of one vector not in the direction
			of another. Can also be used to construct a triplet of orthogonal
			vectors.
		*/
		Vector3D vectorProduct(const Vector3D& vector) const {
			return Vector3D(y * vector.z - z * vector.y,
				z * vector.x - x * vector.z, x * vector.y - y * vector.x);
		}


		Vector3D operator%(const Vector3D& vector) const {
			return vectorProduct(vector);
		}


		real operator*(const Vector3D& vector) const {
			return scalarProduct(vector);
		}


		void vectorProductUpdate(const Vector3D& vector) {
			x = y * vector.z - z * vector.y;
			y = z * vector.x - x * vector.z;
			z = x * vector.y - y * vector.x;
		}


		// Addition of a vector and scalar (adds it to each component)
		Vector3D operator+(real c) const {
			return Vector3D(x + c, y + c, z + c);
		}


		void operator+=(real c) {
			x += c;
			y += c;
			z += c;
		}


		Vector3D operator-(real c) const {
			return Vector3D(x - c, y - c, z - c);
		}


		void operator-=(real c) {
			x -= c;
			y -= c;
			z -= c;
		}


		// Returns each of the coordinates to 0
		void clear() {
			x = y = z = 0;
		}


		void rotate90Degrees() {
			int temp = x;
			x = -y;
			y = temp;
		}


		real operator[](unsigned int i) const {
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}


		real& operator[](unsigned int i){
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}


		void display() const {
			std::cout << "x: " << x << ", y: " << y << ", z: " << z << "\n";
		}

	};


	/*
		As explained in the matrix class header files, for a vector that
		represents the relative position of an object to the origin of
		its rigid body, multiplying the transform matrix by it gives us
		a vector that represents the new coordinates of the object in
		absolute terms (non relational).

		This is not a member function, as the required vector is supplied
		as a parameter
	*/
	Vector3D localToWorld(const Vector3D& relativePosition, const Matrix3x4&
		transformation);


	/*
		After a transformation is applied, to get the relative position of
		the object to the origin, we multiply the new, absolute coordinates
		by the inverse of the transform matrix, both of which are given
		as parameters.

		This is not a member function, as the required vector is supplied
		as a parameter
	*/
	Vector3D worldToLocal(const Vector3D& absolutePosition, const Matrix3x4&
		transformation);


	/*
		Returns the absolute direction vector after a transformation is
		complete, given its relative direction to the origin of a body.
		This function is distinct as the conversion process is different
		for direction vectors (in contrast to distance vectors).
	*/
	Vector3D localToWorldDirection(const Vector3D& relativeDirection, const
		Matrix3x4& transformation);


	/*
		Returns the relative direction vector to the origin of a body after
		a transformation is complete, given its absolute direction. This
		function is distinct as the conversion process is different
		for direction vectors (in contrast to distance vectors).
	*/
	Vector3D worldToLocalDirection(const Vector3D& absoluteDirection, const
		Matrix3x4& transformation);
	

	/*
		Creates an orthonormal basis with the given x vector.
		The y vector is also taken into consideration, but can be changed.
		Both y and z are written to, x is not, and is kept as is.
		We assume x is already normalized, and the written y and z are
		normalized in the function.
	*/
	void makeOrthonormalBasis(const Vector3D& x, Vector3D* y, Vector3D* z);


	/*
		Returns the furthest point in a vector of points from a given
		coordinate.
	*/
	Vector3D furthestPoint(
		const Vector3D& coordinate, 
		const std::vector<Vector3D>& points
	);


	// Prints the vector
	std::ostream& operator<<(std::ostream& out, const Vector3D& vector);
}

#endif