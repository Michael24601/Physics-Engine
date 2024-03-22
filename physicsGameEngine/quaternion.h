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


		Quaternion normalized() const {
			Quaternion normalizedQuaternion = *this;
			normalizedQuaternion.normalize();
			return normalizedQuaternion;
		}


		Quaternion operator*(const Quaternion& right) {
			Quaternion q(
				r * right.r - i * right.i - j * right.j - k * right.k,
				r * right.i + i * right.r + j * right.k - k * right.j,
				r * right.j + j * right.r + k * right.i - i * right.k,
				r * right.k + k * right.r + i * right.j - j * right.i
			);
			return q;
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


		Quaternion operator*(real scalar) const {
			return Quaternion(r * scalar, i * scalar, j * scalar, k * scalar);
		}


		void operator+=(const Quaternion& q2) {
			r += q2.r;
			i += q2.i;
			j += q2.j;
			k += q2.k;
		}


		static Quaternion rotatedByAxisAngle(const Vector3D& axis, real angle) {
			// Calculate half angle
			real halfAngle = angle * real(0.5);

			// Calculate sine and cosine of half angle
			real sinHalfAngle = sin(halfAngle);
			real cosHalfAngle = cos(halfAngle);

			// Normalize axis
			Vector3D normalizedAxis = axis.normalized();

			// Construct quaternion representing the rotation
			Quaternion rotationQuaternion(
				normalizedAxis.x * sinHalfAngle,
				normalizedAxis.y * sinHalfAngle,
				normalizedAxis.z * sinHalfAngle,
				cosHalfAngle
			);
			return rotationQuaternion.normalized();
		}


		real toAxisAngle(Vector3D& axis) const {
			// Ensure the quaternion is normalized
			Quaternion q = normalized();

			// Calculate the angle
			real angle = real(2.0) * acos(q.r);

			// Calculate the axis
			// Assuming quaternion is normalized, otherwise, calculate the square root of (1 - r^2)
			real s = realSqrt(real(1.0) - q.r * q.r);
			if (s < real(0.001)) {
				// If the angle is very close to 0, the axis can be anything
				axis.x = real(1.0);
				axis.y = axis.z = real(0.0);
			}
			else {
				axis.x = q.i / s;
				axis.y = q.j / s;
				axis.z = q.k / s;
			}

			return angle;
		}


		Quaternion conjugated() const {
			return Quaternion(r, -i, -j, -k);
		}

		void display() const {
			std::cout << "r: " << r << ", i: " << i << ", j: " << j << "k" << k << "\n";
		}
	};
}

#endif