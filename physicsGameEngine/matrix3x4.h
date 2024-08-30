/*
	Header file for class that represents a 3 by 4 matrix. We create a
	3 by 4 matrix class instead of a generalized matrix class for the same
	reasons outlined in the 3 by 3 matrix class (optimization).

	We use this matrix to rotate then translate a vector in space, if we
	consider the first 3 columns to represent a normal 3 by 3 matrix's
	rotation and the 4th column to represent the translation vector. The
	on the vector is the same had we first multiplied by the 3 by 3 rotation
	vector then followed it by an addition of the 4th column as a
	translation. However, a 3 by 4 matrix can't multiply a 3D vector, so we 
	instead assume it is a 4 by 4 matrix where the last row is always made
	up of 0 0 0 1, and the we consider the all 3D vectors to be 4D vectors
	where the last coordinate is 1. Using this technique, all matrix by 
	vector multiplications will yield a vector which has had a rotation and
	translation applied sequentially, and a 4th coordinate that is always 1.
	Because the last row of both structures is always the same, we don't
	need to store it, and continue to work with 3 by 4 matrices and 3D
	vectors (the matrix is called a homogeneous matrix). Moreover, to
	stack the rotation and translation trandformation, we can multiply the
	two 4 by 4 matrices where the 4th row is 0 0 0 1, which produces a 
	4 by 4 matrix with the last row being 0 0 0 1, meaning we can again
	discard it and consider only the 3 by 4 matrix as a result.

	Moreover, we can get the inverse of this matrix if we assume it has
	a 4th row being 0 0 0 1 (if the matrix is invertible) since the
	result is also a matrix with a 0 0 0 1 4th row, meaning we can again
	omit it and consider the result to be a 3 by 4 inverse matrix.

	The matrix works on the vector representing the local coordinates of the
	object (in relation to the origin of the body). To apply the
	transformation and get the new location of the object in absolute (not 
	local) coordinates, we simply multiply the relative position vector by
	the matrix. On the other hand, if we transform an object and want to
	find its relative position to the origin of the body, we multiply the
	absolute position vector by the inverse transformation matrix.
*/

#ifndef MATRIX_3_BY_4_H
#define MATRIX_3_BY_4_H

#include "accuracy.h"
#include "vector3D.h"
#include "quaternion.h"
#include "matrix3x3.h"

namespace pe {

	class Matrix3x4 {

	public:

		/*
			The coefficients of the matrix are kept in a 1D array where
			data[0] is the top left element and data[1] is the one to its
			right and data[4] is underneath it.
		*/
		real data[12];

		// The identity of the matrix (no transformation)
		static const Matrix3x4 IDENTITY;

		// No-arg constructor
		Matrix3x4() {
			data[0] = data[1] = data[2] = data[3] = data[4] = data[5]
				= data[6] = data[7] = data[8] = data[9] = data[10]
				= data[11] = 0;
		}

		// Argumented constructor 
		Matrix3x4(real d0, real d1, real d2, real d3, real d4, real d5,
			real d6, real d7, real d8, real d9, real d10, real d11) {
			data[0] = d0;	data[1] = d1;	data[2] = d2;
			data[3] = d3;	data[4] = d4;	data[5] = d5;
			data[6] = d6;	data[7] = d7;	data[8] = d8;
			data[9] = d9;	data[10] = d10;	data[11] = d11;
		}

		Matrix3x4(const Matrix3x3& rotation, const Vector3D& translation) {

			// Rotation
			data[0] = rotation.data[0];
			data[1] = rotation.data[1];	
			data[2] = rotation.data[2];
			data[4] = rotation.data[3];
			data[5] = rotation.data[4];	
			data[6] = rotation.data[5];
			data[8] = rotation.data[6];
			data[9] = rotation.data[7];	
			data[10] = rotation.data[8];

			// Translation
			data[3] = translation.x;	
			data[7] = translation.y;	
			data[11] = translation.z;
		}

		// Copy constructor
		Matrix3x4(const Matrix3x4& matrix) {
			// Uses overloaded = operator
			*this = matrix;
		}

		// Overloaded = operator
		Matrix3x4& operator=(const Matrix3x4& matrix) {
			for (int i = 0; i < 12; i++) {
				data[i] = matrix.data[i];
			}
			return *this;
		}

		/*
			Returns the determinant of the matrix assuming that there is a
			4th row 0 0 0 1.
		*/
		real getDeterminant() const {
			return -data[8] * data[5] * data[2] +
				data[4] * data[9] * data[2] +
				data[8] * data[1] * data[6] -
				data[0] * data[9] * data[6] -
				data[4] * data[1] * data[10] +
				data[0] * data[5] * data[10];
		}

		/*
			Returns the inverse of the matrix if it exists, or the matrix
			itself if it doesn't, assuming there is a 4th row 0 0 0 1. Since
			the inverse will also have a 0 0 0 1 4th row, we can omit it and
			return a 3 by 4 matrix.
		*/
		Matrix3x4 inverse() const {
			real determinant = getDeterminant();

			// If the matrix isn't invertible, it is returned as is
			if (determinant != (real)0.0f) {

				real coefficient = 1 / determinant;
				Matrix3x4 inverse;

				inverse.data[0] = (-data[9] * data[6] + data[5] * data[10])
					* coefficient;
				inverse.data[4] = (data[8] * data[6] - data[4] * data[10])
					* coefficient;
				inverse.data[8] = (-data[8] * data[5] + data[4] * data[9])
					* coefficient;
				inverse.data[1] = (data[9] * data[2] - data[1] * data[10]) 
					* coefficient;
				inverse.data[5] = (-data[8] * data[2] + data[0] * data[10]) 
					* coefficient;
				inverse.data[9] = (data[8] * data[1] - data[0] * data[9]) 
					* coefficient;
				inverse.data[2] = (-data[5] * data[2] + data[1] * data[6])
					* coefficient;
				inverse.data[6] = (data[4] * data[2] - data[0] * data[6]) 
					* coefficient;
				inverse.data[10] = (-data[4] * data[1] + data[0] * data[5]) 
					* coefficient;
				inverse.data[3] = (data[9] * data[6] * data[3]
					- data[5] * data[10] * data[3]
					- data[9] * data[2] * data[7]
					+ data[1] * data[10] * data[7]
					+ data[5] * data[2] * data[11]
					- data[1] * data[6] * data[11]) * coefficient;
				inverse.data[7] = (-data[8] * data[6] * data[3]
					+ data[4] * data[10] * data[3]
					+ data[8] * data[2] * data[7]
					- data[0] * data[10] * data[7]
					- data[4] * data[2] * data[11]
					+ data[0] * data[6] * data[11]) * coefficient;
				inverse.data[11] = (data[8] * data[5] * data[3]
					- data[4] * data[9] * data[3]
					- data[8] * data[1] * data[7]
					+ data[0] * data[9] * data[7]
					+ data[4] * data[1] * data[11]
					- data[0] * data[5] * data[11]) * coefficient;

				return inverse;
			}
			else {
				return *this;
			}
		}

		/*
			Sets this matrix to be the inverse of the given matrix if the
			matrix is non - singular.Does nothing otherwise.
		*/
		void setInverse(const Matrix3x4& matrix) {
			// Utilizes the inverse method and overloaded = operator
			*this = matrix.inverse();
		}

		// Inverts the matrix if it invertible, otherwise leaves it as is
		void invert() {
			setInverse(*this);
		}

		/*
			Same as mutliplying a 3 by 3 matrix by a 3D vector but we assume
			the vector is 4D and the matric is a 4 by 4 matrix (without
			storing or bothering to calculate those coefficients). So while
			in theory we are returning a 4D vector, the Vector3D class will
			have to do.
		*/
		Vector3D operator* (const Vector3D& vector) const {
			/*
				The difference is the addition of the fourth column's extra
				coefficient to each row, representing the transaltion.
			*/
			return Vector3D(
				data[0] * vector.x + data[1] * vector.y + data[2] * vector.z
				+ data[3], data[4] * vector.x + data[5] * vector.y
				+ data[6] * vector.z + data[7], data[8] * vector.x
				+ data[9] * vector.y + data[10] * vector.z + data[11]
			);
		}

		/*
			Multiplies two 3 by 4 matrices to stack their rotations and
			translations. Again we assume they are 4 by 4 matrices with the
			last row being 0 0 0 1. This produces a 4 by 4 matrix where the
			last row is 0 0 0 1, so again we can ignore it, and return a
			3 by 4 matrix. 
		*/
		Matrix3x4 operator* (const Matrix3x4& r) const {
			return Matrix3x4(
				r.data[0] * data[0] + r.data[4] * data[1] + r.data[8] * data[2],
				r.data[1] * data[0] + r.data[5] * data[1] + r.data[9] * data[2],
				r.data[2] * data[0] + r.data[6] * data[1] + r.data[10] * data[2],
				r.data[3] * data[0] + r.data[7] * data[1] + r.data[11] * data[2]
				+ data[3],
				r.data[0] * data[4] + r.data[4] * data[5] + r.data[8] * data[6],
				r.data[1] * data[4] + r.data[5] * data[5] + r.data[9] * data[6],
				r.data[2] * data[4] + r.data[6] * data[5] + r.data[10] * data[6],
				r.data[3] * data[4] + r.data[7] * data[5] + r.data[11] * data[6]
				+ data[7],
				r.data[0] * data[8] + r.data[4] * data[9] + r.data[8] * data[10],
				r.data[1] * data[8] + r.data[5] * data[9] + r.data[9] * data[10],
				r.data[2] * data[8] + r.data[6] * data[9] + r.data[10] * data[10],
				r.data[3] * data[8] + r.data[7] * data[9] + r.data[11] * data[10]
				+ data[11]
			);
		}

		/*
			Sets the first 3 columns of the 3 by 4 matrix to be the rotation
			matrix of the given quaternion, and sets the last column to be
			the translation vector that moves the object.
		*/
		void setOrientationAndPosition(const Quaternion& q, const Vector3D& 
			position) {
			/*
				Uses the formula that transforms an axisand angle into a
				rotation matrix.
			*/
			data[0] = 1 - (2 * q.j * q.j + 2 * q.k * q.k);
			data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
			data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
			data[3] = position.x;
			data[4] = 2 * q.i * q.j - 2 * q.k * q.r;
			data[5] = 1 - (2 * q.i * q.i + 2 * q.k * q.k);
			data[6] = 2 * q.j * q.k + 2 * q.i * q.r;
			data[7] = position.y;
			data[8] = 2 * q.i * q.k + 2 * q.j * q.r;
			data[9] = 2 * q.j * q.k - 2 * q.i * q.r;
			data[10] = 1 - (2 * q.i * q.i + 2 * q.j * q.j);
			data[11] = position.z;
		}

		/*
			Wrapper function for the multiplication of a vector by a matrix.
			Used because, when converting from local to world coordinates
			after performing a transformation, the process isn't always as
			simple as multiplying by a matrix, so a transform function is
			used to wrap the process. So using a wrapper here standardizes
			the process.
		*/
		Vector3D transform(const Vector3D& vector) const {
			// Applies the rotation by multiplying the vector by the matrix
			return (*this) * vector;
		}

		/*
			Applies inverse transformation of the calling object's matrix
			on a vector. Instead of first getting the inverse then
			multiplying the vector by the matrix, this function does this in
			one operation. The function should be used for performance when
			converting from local to world coordinates after transformation,
			instead of using the inverse and operator* functions.
		*/
		Vector3D inverseTransform(const Vector3D& vector) const {
			// First applies the translation
			Vector3D translated = vector;
			translated.x -= data[3];
			translated.y -= data[7];
			translated.z -= data[11];

			// Then applies the rotation and returns the result
			return Vector3D(
				translated.x * data[0] +
				translated.y * data[4] +
				translated.z * data[8],
				translated.x * data[1] +
				translated.y * data[5] +
				translated.z * data[9],
				translated.x * data[2] +
				translated.y * data[6] +
				translated.z * data[10]
			);
		}

		/*
			When transforming a vector using a matrix, multiplying the vector
			by the matrix works with distance vectors, but with direction
			representing vectors, the result is not always another direction
			vector, so we use a specialized function for direction vectors of
			magnitude 1.
		*/
		Vector3D transformDirection(const Vector3D& vector) const {
			// Ensures the vector remains a direction vector of magnitude 1
			return Vector3D(
				vector.x * data[0] +
				vector.y * data[1] +
				vector.z * data[2],
				vector.x * data[4] +
				vector.y * data[5] +
				vector.z * data[6],
				vector.x * data[8] +
				vector.y * data[9] +
				vector.z * data[10]
			);
		}

		/*
			Tranformation function for direction vectors, when converting
			from absolute to relative coordinates, which would usually
			require an inverse matrix multiplication, but which is tweaked
			here because we are working with direction vectors.
		*/
		Vector3D inverseTransformDirection(const Vector3D& vector) const {
			// Ensures the vector remains a direction vector of magnitude 1
			return Vector3D(
				vector.x * data[0] +
				vector.y * data[4] +
				vector.z * data[8],
				vector.x * data[1] +
				vector.y * data[5] +
				vector.z * data[9],
				vector.x * data[2] +
				vector.y * data[6] +
				vector.z * data[10]
			);
		}

		/*
			Returns the first, second, third, or fourth column vector,
			starting with i = 0.
		*/
		Vector3D getColumnVector(int i) const {
			return Vector3D(data[i], data[i + 4], data[i + 8]);
		}


		// Function that returns the transaltion part of the matrix
		Vector3D getTranslation() const {
			return Vector3D(
				data[3],
				data[7],
				data[11]
			);
		}

		Matrix3x3 getRotation() const {
			return Matrix3x3(
				data[0], data[1], data[2],
				data[4], data[5], data[6],
				data[8], data[9], data[10]
			);
		}

		/*
			Adds a rotation to the rotation matrix part of the homogeneous
			matrix.
		*/
		void addRotation(const Matrix3x3& rotation) {
			setRotation(rotation * getRotation());
		}


		void addTranslation(const Vector3D& translation) {
			setTranslation(translation + getTranslation());
		}


		// Function that sets the translation part of the matrix
		void setTranslation(const Vector3D& translation) {
			data[3] = translation.x;
			data[7] = translation.y;
			data[11] = translation.z;
		}


		// Function that sets the rotation part of the matrix
		void setRotation(const Matrix3x3& rotation) {
			data[0] = rotation.data[0];
			data[1] = rotation.data[1];
			data[2] = rotation.data[2];
			data[4] = rotation.data[3];
			data[5] = rotation.data[4];
			data[6] = rotation.data[5];
			data[8] = rotation.data[6];
			data[9] = rotation.data[7];
			data[10] = rotation.data[8];
		}


		/*
			Function that combines the effects of two 3 by 4 matrices.
			The way to transform a Matrix3x4 B by another Matrix3x4 A is to
			first rotate the translation aspect of B by A's rotation, then
			add B's translation to it. The rotation of B is also multiplied
			by that of A.
			This is the equivalent of multiplying two homogeneous Matrix4x4
			where the last row is [0 0 0 1]. It achieves the effect of
			combining their transformations.
		*/
		void combineMatrix(const Matrix3x4& newMatrix) {
			setTranslation(
				newMatrix.getRotation().transform(getTranslation())
			);
			addTranslation(newMatrix.getTranslation());
			addRotation(newMatrix.getRotation());
		}
	};
}

#endif