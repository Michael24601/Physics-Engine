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

