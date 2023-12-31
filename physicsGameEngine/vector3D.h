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

		// Length of vector
		real magnitude() const {
			return realSqrt(x * x + y * y + z * z);
		}

		// Magnitude squared (avoid expensive call to sqrt)
		real magnitudeSquared() const {
			return x * x + y * y + z * z;
		}

		// Turn non-zero vector into one with unit length (1)
		void normalize() {
			real m = magnitude();
			if (m > 0) {
				// multiplies vector by scalar
				(*this) *= ((real)1) / m;
			}
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

		void vectorProductUpdate(const Vector3D& vector) {
			x = y * vector.z - z * vector.y;
			y = z * vector.x - x * vector.z;
			z = x * vector.y - y * vector.x;
		}

		// Overriden operator !=
		bool operator!=(const Vector3D& vector) const {
			return !(x == vector.x && y == vector.y && z == vector.z);
		}

		// Returns each of the coordinates to 0
		void clear() {
			x = y = z = 0;
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
}

#endif