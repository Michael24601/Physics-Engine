/*
	Header file for class representing an oriented bounding box.
*/

#ifndef ORIENTED_BOUNDING_BOX_H
#define ORIENTED_BOUNDING_BOX_H

#include "boundingBox.h"
#include <Eigen/Dense>

namespace pe {

	class OrientedBoundingBox : public BoundingBox {

	public:

		OrientedBoundingBox(
			Polyhedron* polyhedron
		) : BoundingBox(polyhedron) {

			/*
				Calculates the offset, halfsize, and initial orientation (needs
				to be called only once, at the start, sinde oriented bounding
				boxes rotate and move with the polyhedron, and can use the updated
				polyhedron transform matrix and the afore mentioned values to
				stay accurate frame by frame).

				This method (using Principal Component Analysis), is an
				approximation, but can be calculated quite fast.
			*/

			if (vertices.empty()) {
				return;
			}

			int size = vertices.size();

			// Convert vector of points to Eigen matrix
			Eigen::MatrixXd data(size, 3);
			for (size_t i = 0; i < size; ++i) {
				data.row(i) = Eigen::Vector3d(
					vertices[i].x,
					vertices[i].y,
					vertices[i].z
				);
			}

			// Mean of the points
			Eigen::Vector3d mean = data.colwise().mean();
			// Center the data
			Eigen::MatrixXd centered = data.rowwise() - mean.transpose();
			// Covariance matrix of the points
			Eigen::Matrix3d covariance = (centered.transpose() * centered)
				/ double(size - 1);

			// Eigen decomposition
			Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(covariance);
			if (solver.info() != Eigen::Success) {
				throw std::runtime_error("Eigen decomposition failed");
			}

			// Eigenvalues are in ascending order
			Eigen::Vector3d eigenValues = solver.eigenvalues();
			Eigen::Matrix3d eigenVectors = solver.eigenvectors();

			// The offset is just the mean of the points
			baseOffset = Vector3D(mean.x(), mean.y(), mean.z());

			// The transpose of the eigenVector matrix is the rotation matrix.
			Eigen::Matrix3d mat = eigenVectors.transpose();
			baseOrientation = Matrix3x3(
				mat(0, 0), mat(0, 1), mat(0, 2),
				mat(1, 0), mat(1, 1), mat(1, 2),
				mat(2, 0), mat(2, 1), mat(2, 2)
			);

			// The half-sizes are the square root of eigenvalues
			Eigen::Vector3d hs = eigenValues.cwiseSqrt();
			halfsize = Vector3D(hs.x(), hs.y(), hs.z());

			// We call the update function to initialize the transform matrix
			update();
		}


		void update() override {
			/*
				Because it can be oriented, the OBB transform matrix can be
				updated by simply combining its base parameters with the
				transform matrix of the polyhedron at this point.
			*/
			transformMatrix = Matrix3x4(baseOrientation, baseOffset);
			transformMatrix.combineMatrix(polyhedron->getTransformMatrix());
		}
	};
}

#endif