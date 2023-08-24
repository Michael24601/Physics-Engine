
#include "rigidBodySpringForce.h"
#include <iostream>

using namespace pe;

RigidBodySpringForce::RigidBodySpringForce(const Vector3D& connectionPoint,
	RigidBody* otherBody, const Vector3D& otherConnectionPoint,
	real springConstant, real restLength) : connectionPoint{connectionPoint},
	otherBody{ otherBody }, otherConnectionPoint{otherConnectionPoint},
	springConstant{ springConstant }, restLength{restLength} {}


void RigidBodySpringForce::updateForce(RigidBody* body, real duration) {
	// Calculate the two ends in world space.
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
	real magnitude = force.magnitude();
	magnitude = realAbs(magnitude - restLength);
	magnitude *= springConstant;

	// Applies the force 
	force.normalize();
	force *= -magnitude;

	/*
		We could also have used addForce with applicationPoint in world
		coordinates.
	*/
	body->addForceAtLocalPoint(force, connectionPoint);
}