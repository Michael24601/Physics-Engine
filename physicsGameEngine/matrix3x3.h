/*
	Header file for class representing a 3 by 3 matrix. For the purposes
	of this engine, having a class for a 3 by 3 matrix (and a 3 by 4 matrix)
	is better than having a general n by m matrix class as it can be more
	easily optomized.

	3 by 3 matrices can be used for rotations, where each column represents
	the change of basis in one of the three directions x, y and z. Moreover,
	the multiplication of two matrices stacks rotations. To apply a rotation
	to a vector.

	The matrix works on the vector representing the local coordinates of the
	object (in relation to the origin of the body). To apply the rotation
	and get the new location of the object in absolute (not local)
	coordinates, we simply multiply the relative position vector by the 
	matrix. On the other hand, if we transform an object and want to find
	its relative position to the origin of the body, we multiply the 
	absolute position vector by the inverse rotation matrix.
*/

#ifndef MATRIX_3_BY_3_H
#define MATRIX_3_BY_3_H

#include "accuracy.h"
#include "vector3D.h"
#include "quaternion.h"

namespace pe {

	class Matrix3x3 {

	public:

		/*
			The coefficients of the matrix are kept in a 1D array where
			data[0] is the top right element and data[1] is the one to its
			right and data[3] is underneath it.
		*/
		real data[9];

		// No-arg constructor
		Matrix3x3(){
			data[0] = data[1] = data[2] = data[3] = data[4] = data[5]
				= data[6] = data[7] = data[8] = 0;
		}

		// Argumented constructor 
		Matrix3x3(real d0, real d1, real d2, real d3, real d4, real d5,
			real d6, real d7, real d8) {
			data[0] = d0;	data[1] = d1;	data[2] = d2;
			data[3] = d3;	data[4] = d4;	data[5] = d5;
			data[6] = d6;	data[7] = d7;	data[8] = d8;
		}

		// Copy constructor
		Matrix3x3(const Matrix3x3& matrix) {
			// Uses overloaded = operator
			*this = matrix;
		}

		
		/*
			Setter for the matrix.
			Takes in 3 vectors, whose components are then set in as the 
			columns of the matrix.
		*/
		void setComponents(
			const Vector3D& first,
			const Vector3D& second,
			const Vector3D& third
		) {
			data[0] = first.x;
			data[1] = second.x;
			data[2] = third.x;
			data[3] = first.y;
			data[4] = second.y;
			data[5] = third.y;
			data[6] = first.z;
			data[7] = second.z;
			data[8] = third.z;
		}

		// Overloaded = operator
		Matrix3x3& operator=(const Matrix3x3& matrix) {
			for (int i = 0; i < 9; i++) {
				data[i] = matrix.data[i];
			}
			return *this;
		}

		// Returns the determinant of the matrix
		real getDeterminant() const {
			return data[0] * data[4] * data[8]
				+ data[3] * data[7] * data[2]
				+ data[6] * data[1] * data[5]
				- data[0] * data[7] * data[5]
				- data[6] * data[4] * data[2]
				- data[3] * data[1] * data[8];
		}

		/*
			Returns the inverse of the matrix if it exists, or the matrix
			itself if it doesn't.
		*/
		Matrix3x3 inverse() const {
			real determinant = getDeterminant();

			// If the matrix isn't invertible, it is returned as is
			if (determinant != (real)0.0f) {

				real coefficient = 1 / determinant;
				Matrix3x3 inverse;

				inverse.data[0] = (data[4] * data[8] - data[5] * data[7])
					* coefficient;
				inverse.data[1] = -(data[1] * data[8] - data[2] * data[7])
					* coefficient;
				inverse.data[2] = (data[1] * data[5] - data[2] * data[4])
					* coefficient;
				inverse.data[3] = -(data[3] * data[8] - data[5] * data[6])
					* coefficient;
				inverse.data[4] = (data[0] * data[8] - data[0] * data[4])
					* coefficient;
				inverse.data[5] = -(data[0] * data[5] - data[2] * data[3])
					* coefficient;
				inverse.data[6] = (data[3] * data[7] - data[4] * data[6])
					* coefficient;
				inverse.data[7] = -(data[0] * data[7] - data[1] * data[6])
					* coefficient;
				inverse.data[8] = (data[0] * data[4] - data[1] * data[3])
					* coefficient;

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
		void setInverse(const Matrix3x3& matrix) {
			// Utilizes the inverse method and overloaded = operator
			*this = matrix.inverse();
		}

		// Inverts the matrix if it invertible, otherwise leaves it as is
		void invert() {
			setInverse(*this);
		}

		/*
			Returns the transpose of this matrix, which represents the
			inverse of a rotation matrix only. Since a 3 by 4 matrix does
			not just represent a rotation, we won't use its transpose as a
			replacement for its inverse. We also can't calculate the
			transpose of a homogeneous matrix, as the last column will
			become the 4th row, which may have non-zero elements, which
			can't be represented in a 3 by 4 matrix by assuming it's 4 by 4.
		*/
		Matrix3x3 transpose() const {
			return Matrix3x3(data[0], data[3], data[6], data[1], data[4],
				data[7], data[2], data[5], data[8]);
		}

		// Sets this matrix as the transpose of the given parameter
		void setTranspose(const Matrix3x3& matrix) {
			*this = matrix.transpose();
		}

		/*
			Multiplying a vector by a matrix (to its left) is like
			having it multiplied by a 1D matrix, so it returns a 1D
			matrix of 3 rows, in other words, a Vector3D object.
		*/
		Vector3D operator* (const Vector3D& vector) const {
			return Vector3D(
				data[0] * vector.x + data[1] * vector.y + data[2] * vector.z,
				data[3] * vector.x + data[4] * vector.y + data[5] * vector.z,
				data[6] * vector.x + data[7] * vector.y + data[8] * vector.z
			);
		}

		/*
			Multiplies a matrix by a scaler.
		*/
		Matrix3x3 operator* (real scaler) const {
			return Matrix3x3(
				data[0] * scaler,
				data[1] * scaler,
				data[2] * scaler,
				data[3] * scaler,
				data[4] * scaler,
				data[5] * scaler,
				data[6] * scaler,
				data[7] * scaler,
				data[8] * scaler
			);
		}


		/*
			Adds two matrices.
		*/
		Matrix3x3 operator+ (const Matrix3x3& matrix) const {
			return Matrix3x3(
				data[0] + matrix.data[0],
				data[1] + matrix.data[1],
				data[2] + matrix.data[2],
				data[3] + matrix.data[3],
				data[4] + matrix.data[4],
				data[5] + matrix.data[5],
				data[6] + matrix.data[6],
				data[7] + matrix.data[7],
				data[8] + matrix.data[8]
			);
		}


		/*
			Multiplies a matrix by a scaler and applies the result
		*/
		void operator*= (real scaler) {
			data[0] *= scaler;
			data[1] *= scaler;
			data[2] *= scaler;
			data[3] *= scaler;
			data[4] *= scaler;
			data[5] *= scaler;
			data[6] *= scaler;
			data[7] *= scaler;
			data[8] *= scaler;
		}

		// Multiplies two 3 by 3 matrices to stack their rotations
		Matrix3x3 operator* (const Matrix3x3& r) const {
			return Matrix3x3(
				data[0] * r.data[0] + data[1] * r.data[3] + data[2] * r.data[6],
				data[0] * r.data[1] + data[1] * r.data[4] + data[2] * r.data[7],
				data[0] * r.data[2] + data[1] * r.data[5] + data[2] * r.data[8],
				data[3] * r.data[0] + data[4] * r.data[3] + data[5] * r.data[6],
				data[3] * r.data[1] + data[4] * r.data[4] + data[5] * r.data[7],
				data[3] * r.data[2] + data[4] * r.data[5] + data[5] * r.data[8],
				data[6] * r.data[0] + data[7] * r.data[3] + data[8] * r.data[6],
				data[6] * r.data[1] + data[7] * r.data[4] + data[8] * r.data[7],
				data[6] * r.data[2] + data[7] * r.data[5] + data[8] * r.data[8]
			);
		}

		// Sets the matrix to be the rotation matrix of the given quaternion
		void setOrientation(const Quaternion& q)
		{
			/*
				The formula used here is the same as the one that transforms
				an axis and angle into the matrix.
			*/
			data[0] = 1 - (2 * q.j * q.j + 2 * q.k * q.k);
			data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
			data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
			data[3] = 2 * q.i * q.j - 2 * q.k * q.r;
			data[4] = 1 - (2 * q.i * q.i + 2 * q.k * q.k);
			data[5] = 2 * q.j * q.k + 2 * q.i * q.r;
			data[6] = 2 * q.i * q.k + 2 * q.j * q.r;
			data[7] = 2 * q.j * q.k - 2 * q.i * q.r;
			data[8] = 1 - (2 * q.i * q.i + 2 * q.j * q.j);
		}


		/*
			Function that sets the elements of the matrix to be those of a
			skew matrix. A skew matrix is used in the calculation of
			rotational dynamics; specifically, relating angular velocity
			with angular displacement, something that will be useful in the
			collision resolution module.
		*/
		void setSkewSymmetric(const Vector3D vector){
			data[0] = data[4] = data[8] = 0;
			data[1] = -vector.z;
			data[2] = vector.y;
			data[3] = vector.z;
			data[5] = -vector.x;
			data[6] = -vector.y;
			data[7] = vector.x;
		}

		/*
			Wrapper function for the multiplication of a vector by a matrix.
		*/
		Vector3D transform(const Vector3D& vector) const {
			// Applies the rotation by multiplying the vector by the matrix
			return (*this) * vector;
		}
	};
}

#endif