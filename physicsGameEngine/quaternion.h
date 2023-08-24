/*
	Header file for the Quaternion class, which is a mathematical structure
	used in rotating and orienting rigid bodies. The quaternions are
	converted into rotation matrices in order to apply the transformation.
*/

#ifndef QUATERNION_H
#define QUATERNION_H

#include "accuracy.h"
#include "vector3D.h"

namespace pe {

	class Quaternion {

	public:

		/*
			Union is used here because we may want to switch between using
			the struct and array representations of the quaternion. The
			union keyword lets us do that without wasting space as it only
			allows allocating memory for one representation at a time.
			Placing a value in one of the formats makes the other
			inaccessible.
		*/
		union {
			// Struct representation of a quaternion
			struct {
				// Real component of the quaternion
				real r;

				// Imaginary components of the quaternion
				real i;
				real j;
				real k;
			};

			/*
				Array representation of the quaternion, where a[0] is the
				real part and i, j, and k are a[1], a[2], and a[3]
				respectively.
			*/
			real data[4];
		};

		/*
			Default constructor creates the quaternion of the zero rotation.
		*/
		Quaternion() : r(1), i(0), j(0), k(0) {}

		// Argumented constructor
		Quaternion(real r, real i, real j, real k) : r{ r }, i{ i }, j{ j },
			k{ k } {}

		/*
			Normalizes the quaternion to unit length, a requirement for it
			to become a valid orientation/rotation quaternion.
		*/
		void normalize() {
			// Caluclates the magnitude squared using 4D Pythagorean formula
			real magnitudeSquared = (r * r) + (i * i) + (j * j) + (k * k);

			/*
				If the magnitude is 0, then each of the 4 components was 0
				(as each sum is positive since it's a square). To normalize
				it, we can turn it into the no-rotation quaternion, 1 0 0 0,
				which is normalized.
			*/
			if (magnitudeSquared == 0) {
				// Th rest are already 0, no need to reassign them.
				r = 1;
			}
			else {
				// Dividing by the magnitude normalizes the quaternion
				real coefficient = real(1.0f) / realSqrt(magnitudeSquared);
				r *= coefficient;
				i *= coefficient;
				j *= coefficient;
				k *= coefficient;
			}
		}

		// Multiplies two quaternions following the well known formula
		void operator*=(const Quaternion& right) {
			r = r * right.r - i * right.i - j * right.j - k * right.k;
			i = r * right.i + i * right.r + j * right.k - k * right.j;
			j = r * right.j + j * right.r + k * right.i - i * right.k;
			k = r * right.k + k * right.r + i * right.j - j * right.i;
		}

		/*
			If the quaternion represents the orientation of an object, this
			function will reorient the quaternion after an angular velocity
			represented by a vector is applied to it for a given time.
			The vector is the angular velocity, and the scale is the amount
			of the vector to add to the quaternion (representing the time).
		*/
		void addScaledVector(const Vector3D& vector, real scale) {
			// Multiplies by scaled vector as a quaternion (real part is 0).
			Quaternion q(0, vector.x * scale, vector.y * scale,
				vector.z * scale);
			q *= *this;

			// Then adds and scales result to calling object
			r += q.r * ((real)0.5);
			i += q.i * ((real)0.5);
			j += q.j * ((real)0.5);
			k += q.k * ((real)0.5);
		}

		/*
			Rotates a quaternion representing an orientation by a fixed
			angular change (instead of angular velocity over time). The
			rotation is represented by the vector and is applied by
			multiplying the quaternion resulting from the vector (real
			part is 0) with the calling object.
		*/
		void rotateByVector(const Vector3D& vector) {
			Quaternion q(0, vector.x, vector.y, vector.z);
			(*this) *= q;
		}
	};
}

#endif