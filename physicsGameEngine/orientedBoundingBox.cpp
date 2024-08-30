
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

	// Converting vector of points to Eigen matrix
	Eigen::MatrixXd data(size, 3);
	for (size_t i = 0; i < size; ++i) {
		data.row(i) = Eigen::Vector3d(
			vertices[i].x,
			vertices[i].y,
			vertices[i].z
		);
	}

	Eigen::MatrixXd centered = data.rowwise() - data.colwise().mean();
	Eigen::MatrixXd covariance = (centered.transpose() * centered) / double(size);

	// Computing eigenvalues and eigenvectors
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(covariance);
	if (eigensolver.info() != Eigen::Success) {
		std::cerr << "Eigen decomposition failed." << std::endl;
		return;
	}

	Eigen::VectorXd eigenvalues = eigensolver.eigenvalues();
	Eigen::MatrixXd eigenvectors = eigensolver.eigenvectors();

	/*
		We can then use the eigenvectors to find the base orientation of
		the OBB (the orientation of the OBB when the object is still in
		local coordinates.
	*/
	baseOrientation = Matrix3x3(
		eigenvectors(0, 0), eigenvectors(0, 1), eigenvectors(0, 2),
		eigenvectors(1, 0), eigenvectors(1, 1), eigenvectors(1, 2),
		eigenvectors(2, 0), eigenvectors(2, 1), eigenvectors(2, 2)
	);

	// Centroid of the bounding box
	Eigen::Vector3d center = data.colwise().mean();

	/*
		Here we rotate data to align with the coordinate axes and then
		get the min.max values.
	*/
	Eigen::MatrixXd alignedData = (data.rowwise() - center.transpose()) *
		eigenvectors;
	Eigen::Vector3d minValues = alignedData.colwise().minCoeff();
	Eigen::Vector3d maxValues = alignedData.colwise().maxCoeff();

	// The half-size calculation
	Eigen::Vector3d hs = (maxValues - minValues) * 0.5;
	halfsize = Vector3D(hs.x(), hs.y(), hs.z());

	// The offset from the object's local origin (centroid)
	Eigen::Vector3d o = eigenvectors * center;
	baseOffset = Vector3D(o.x(), o.y(), o.z());

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