
#include "orientedBoundingBox.h";

using namespace pe;


OrientedBoundingBox::OrientedBoundingBox(Polyhedron* polyhedron) :
BoundingBox(polyhedron) {

	/*
		Calculates the offset, halfsize, and initial orientation (needs
		to be called only once, at the start, sinde oriented bounding
		boxes rotate and move with the polyhedron, and can use the updated
		polyhedron transform matrix and the afore mentioned values to
		stay accurate frame by frame).

		This method (using Principal Component Analysis), is an
		approximation, but can be calculated quite fast.
	*/

	std::vector<Vector3D>& vertices = polyhedron->localVertices;

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

	// Extract eigenvalues and eigenvectors
	Eigen::Vector3d eigenValues = solver.eigenvalues();
	Eigen::Matrix3d eigenVectors = solver.eigenvectors();

	// Pair the eigenvalues with their corresponding eigenvectors
	std::vector<std::pair<double, Eigen::Vector3d>> eigenPairs;
	for (int i = 0; i < 3; ++i) {
		eigenPairs.emplace_back(eigenValues[i], eigenVectors.col(i));
	}

	// Sort pairs by eigenvalue in descending order to match expected axis order
	std::sort(eigenPairs.begin(), eigenPairs.end(), [](const auto& a, const auto& b) {
		return a.first > b.first;  // Descending order
		});

	// Construct the orientation matrix and halfsize vector
	Eigen::Matrix3d orientationMatrix;
	Eigen::Vector3d halfsizeVector;
	for (int i = 0; i < 3; ++i) {
		orientationMatrix.col(i) = eigenPairs[i].second;
		halfsizeVector[i] = std::sqrt(std::abs(eigenPairs[i].first));
	}

	// Set the base orientation and halfsize
	baseOrientation = Matrix3x3(
		orientationMatrix(0, 0), orientationMatrix(0, 1), orientationMatrix(0, 2),
		orientationMatrix(1, 0), orientationMatrix(1, 1), orientationMatrix(1, 2),
		orientationMatrix(2, 0), orientationMatrix(2, 1), orientationMatrix(2, 2)
	);

	halfsize = Vector3D(halfsizeVector.x(), halfsizeVector.y(), halfsizeVector.z());

	// We call the update function to initialize the transform matrix
	update(polyhedron);
}

void OrientedBoundingBox::update(Polyhedron* polyhedron) {
	/*
		Because it can be oriented, the OBB transform matrix can be
		updated by simply combining its base parameters with the
		transform matrix of the polyhedron at this point.
	*/
	transformMatrix = Matrix3x4(baseOrientation, baseOffset);
	transformMatrix.combineMatrix(polyhedron->getTransformMatrix());
}