// Source file for the vector3D class

#include "vector3D.h"
#include "matrix3x4.h"

using namespace pe;

Vector3D pe::localToWorld(const Vector3D& relativePosition, const
	Matrix3x4& transformation) {
	return transformation.transform(relativePosition);
}


Vector3D pe::worldToLocal(const Vector3D & absolutePosition, const 
	Matrix3x4& transformation) {
	return transformation.inverseTransform(absolutePosition);
}


Vector3D pe::localToWorldDirection(const Vector3D& relativePosition, const
	Matrix3x4& transformation) {
	return transformation.transformDirection(relativePosition);
}


Vector3D pe::worldToLocalDirection(const Vector3D& absolutePosition, const
	Matrix3x4& transformation) {
	return transformation.inverseTransformDirection(absolutePosition);
}


void pe::makeOrthonormalBasis(const Vector3D& x, Vector3D* y, Vector3D* z) {
	*z = x.vectorProduct(*y);

	// We then check if we need to change the suggested y or not.
	if (z->magnitudeSquared() == 0.0) {
		return;
	}

	(*y) = (*z).vectorProduct(x);
	// Normalizes the vectors
	y->normalize();
	z->normalize();
}


std::ostream& pe::operator<<(std::ostream& out, const Vector3D& vector) {
	out << "x: " << vector.x << ", y: " << vector.y << ", z: " 
		<< vector.z << "\n";
	return out;
}
