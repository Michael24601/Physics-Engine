
#include "rigidBodyCableForce.h"

using namespace pe;


RigidBodyCableForce::RigidBodyCableForce(const Vector3D& connectionPoint,
	RigidBody* otherBody, const Vector3D& otherConnectionPoint,
	real length, real restitutionCoefficient) : 
	connectionPoint{ connectionPoint }, otherBody{ otherBody }, 
	otherConnectionPoint{ otherConnectionPoint }, length{ length }, 
	restitutionCoefficient{restitutionCoefficient} {}


void RigidBodyCableForce::updateForce(RigidBody* body, real duration) const {
	// Calculates the two ends in world space.
	/*
		UNCLEAR WHY BUT USING body->getPointInWorldCoordinates(connectionPoint)
		gives an error, so instead the transform matrix is used directly.
	*/
	Vector3D pointOfApplication =
		body->transformMatrix.transform(connectionPoint);
	Vector3D otherPointOfApplication =
		otherBody->transformMatrix.transform(otherConnectionPoint);
	/*
		Calculates the directiomn vector of the spring pull force.
		It should be normalized before the real calculated magnitude is
		multiplied by it since this step only concerns its direction.
	*/
	Vector3D force = pointOfApplication - otherPointOfApplication;

	// Magnitude of the spring force
	real currentLength = force.magnitude();

	// Checks if the cable has extended beyond its allowed length.
	if (currentLength > length) {
		// Calculates the amount by which to shorten the cable.
		real excessLength = currentLength - length;
		// Normalizes the direction vector
		force.normalize();
		force *= excessLength * (-1 / duration) * restitutionCoefficient;
		body->addForceAtLocalPoint(force, connectionPoint);
	}

	// Otherwise we apply no force, since max has not been reached
}