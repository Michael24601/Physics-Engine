
#include "contact.h"

using namespace pe;

void Contact::calculateContactBasis() {

	Vector3D contactTangent[2];

	// First we check if the z axis is closer to the x axis or y axis
	if (realAbs(contactNormal.x) > realAbs(contactNormal.y))
	{
		/*
			Instead of normalizing the vectors later, we can apply a
			scaling factor, which ensures the vectors end up normalized
			in a more performant way.
		*/
		const real s = (real)1.0f / realSqrt(contactNormal.z * contactNormal.z
			+ contactNormal.x * contactNormal.x);

		// The new x axis at a right angle to the world y axis
		contactTangent[0].x = contactNormal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -contactNormal.x * s;
		// The new y axis at right angles to the new x and z axes
		contactTangent[1].x = contactNormal.y * contactTangent[0].x;
		contactTangent[1].y = contactNormal.z * contactTangent[0].x -
			contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
	}
	else {
		// Scaling factor, like the one above
		const real s = (real)1.0 / realSqrt(contactNormal.z * contactNormal.z
			+ contactNormal.y * contactNormal.y);

		// The new x axis at a right angle to the world x axis.
		contactTangent[0].x = 0;
		contactTangent[0].y = -contactNormal.z * s;
		contactTangent[0].z = contactNormal.y * s;

		// The new y axis at right angles to the new x and z axes
		contactTangent[1].x = contactNormal.y * contactTangent[0].z -
			contactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = contactNormal.x * contactTangent[0].y;
	}

	/*
		Then a matrix is created from the new basis vectors, forming a
		trasnform matrix from contact to world coordinates.
	*/
	contactToWorld.setComponents(
		contactNormal,
		contactTangent[0],
		contactTangent[1]
	);
}


void Contact::calculateInternals(real duration) {

	/*
		If one of the bodies is null (the contact is happening with an
		immovable object like a wall or floor), then we need the second
		body object to be the one that's null.
	*/
	if (!body[0]) {
		swapBodies();
	}
	assert(body[0]);

	// First we calculate the contact basis
	calculateContactBasis();

	// We then calculate the local positions of the contact
	relativeContactPosition[0] = contactPoint - body[0]->position;
	if (body[1]) {
		relativeContactPosition[1] = contactPoint - body[1]->position;
	}


	/*
		We also need to find the closing (contact) velocity in contact
		coordinates.
	*/
	contactVelocity = calculateLocalVelocity(0, duration);
	if (body[1]) {
		contactVelocity -= calculateLocalVelocity(1, duration);
	}


	// We also need the desired change in velocity, in world coordinates
	calculateDesiredDeltaVelocity(duration);
}


void Contact::swapBodies() {
	contactNormal *= -1;
	RigidBody* temp = body[0];
	body[0] = body[1];
	body[1] = temp;
}



void Contact::applyPositionChange(
	Vector3D linearChange[2],
	Vector3D angularChange[2],
	real penetration
) {
	const real angularLimit = (real)0.2f;
	real angularMove[2];
	real linearMove[2];

	real totalInertia = 0;
	real linearInertia[2];
	real angularInertia[2];

	// We need to work out the inertia of each object in the direction
	// of the contact normal, due to angular inertia only.
	for (unsigned i = 0; i < 2; i++) if (body[i])
	{
		Matrix3x3 inverseInertiaTensor = body[i]->inverseInertiaTensor;

		// Use the same procedure as for calculating frictionless
		// velocity change to work out the angular inertia.
		Vector3D angularInertiaWorld =
			relativeContactPosition[i].vectorProduct(contactNormal);
		angularInertiaWorld =
			inverseInertiaTensor.transform(angularInertiaWorld);
		angularInertiaWorld =
			angularInertiaWorld.vectorProduct(relativeContactPosition[i]);
		angularInertia[i] =
			angularInertiaWorld.scalarProduct(contactNormal);

		// The linear component is simply the inverse mass
		linearInertia[i] = body[i]->inverseMass;

		// Keep track of the total inertia from all components
		totalInertia += linearInertia[i] + angularInertia[i];

		// We break the loop here so that the totalInertia value is
		// completely calculated (by both iterations) before
		// continuing.
	}

	// Loop through again calculating and applying the changes
	for (unsigned i = 0; i < 2; i++) if (body[i])
	{
		// The linear and angular movements required are in proportion to
		// the two inverse inertias.
		real sign = (i == 0) ? 1 : -1;
		angularMove[i] =
			sign * penetration * (angularInertia[i] / totalInertia);
		linearMove[i] =
			sign * penetration * (linearInertia[i] / totalInertia);

		// To avoid angular projections that are too great (when mass is large
		// but inertia tensor is small) limit the angular move.
		Vector3D projection = relativeContactPosition[i];
		projection.linearCombination(
			contactNormal,
			-relativeContactPosition[i].scalarProduct(contactNormal)
		);

		// Use the small angle approximation for the sine of the angle (i.e.
		// the magnitude would be sine(angularLimit) * projection.magnitude
		// but we approximate sine(angularLimit) to angularLimit).
		real maxMagnitude = angularLimit * projection.magnitude();

		if (angularMove[i] < -maxMagnitude)
		{
			real totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if (angularMove[i] > maxMagnitude)
		{
			real totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}

		// We have the linear amount of movement required by turning
		// the rigid body (in angularMove[i]). We now need to
		// calculate the desired rotation to achieve that.
		if (angularMove[i] == 0) {
			// Easy case - no angular movement means no rotation.
			angularChange[i].clear();
		}
		else {
			// Work out the direction we'd like to rotate in.
			Vector3D targetAngularDirection =
				relativeContactPosition[i].vectorProduct(contactNormal);

			Matrix3x3 inverseInertiaTensor = body[i]->inverseInertiaTensor;

			// Work out the direction we'd need to rotate to achieve that
			angularChange[i] =
				inverseInertiaTensor.transform(targetAngularDirection) *
				(angularMove[i] / angularInertia[i]);
		}

		// Velocity change is easier - it is just the linear movement
		// along the contact normal.
		linearChange[i] = contactNormal * linearMove[i];

		// Now we can start to apply the values we've calculated.
		// Apply the linear movement
		Vector3D pos = body[i]->position;
		pos.linearCombination(contactNormal, linearMove[i]);
		body[i]->position = pos;

		// And the change in orientation
		Quaternion q = body[i]->orientation;
		q.addScaledVector(angularChange[i], ((real)1.0));
		body[i]->orientation = q;

		if (!body[i]->isAwake) {
			body[i]->calculateDerivedData();
		}
	}
}


Vector3D Contact::calculateLocalVelocity(
	unsigned int bodyIndex,
	real duration
) {
	RigidBody* thisBody = body[bodyIndex];
	// Work out the velocity of the contact point.
	Vector3D velocity = thisBody->angularVelocity.vectorProduct(
		relativeContactPosition[bodyIndex]
	);
	velocity += thisBody->linearVelocity;

	// We then turn the velocity into contact coordinates

	// Because contactToWorld is a rotation, its inverse is the transpose 
	Matrix3x3 inverse = contactToWorld.transpose();
	Vector3D contactVelocity = inverse.transform(velocity);

	return contactVelocity;
}


inline Vector3D Contact::calculateFrictionlessImpulse(
	Matrix3x3* inverseInertiaTensor
) {
	Vector3D impulseContact;

	// Build a vector that shows the change in velocity in
	// world space for a unit impulse in the direction of the contact
	// normal.
	Vector3D deltaVelWorld = relativeContactPosition[0].vectorProduct(contactNormal);
	deltaVelWorld = inverseInertiaTensor[0].transform(deltaVelWorld);
	deltaVelWorld = deltaVelWorld.vectorProduct(relativeContactPosition[0]);

	// Work out the change in velocity in contact coordiantes.
	real deltaVelocity = deltaVelWorld.scalarProduct(contactNormal);

	// Add the linear component of velocity change
	deltaVelocity += body[0]->inverseMass;

	// Check if we need to the second body's data
	if (body[1])
	{
		// Go through the same transformation sequence again
		Vector3D deltaVelWorld = relativeContactPosition[1].vectorProduct(contactNormal);
		deltaVelWorld = inverseInertiaTensor[1].transform(deltaVelWorld);
		deltaVelWorld = deltaVelWorld.vectorProduct(relativeContactPosition[1]);

		// Add the change in velocity due to rotation
		deltaVelocity += deltaVelWorld.scalarProduct(contactNormal);

		// Add the change in velocity due to linear motion
		deltaVelocity += body[1]->inverseMass;
	}

	// Calculate the required size of the impulse
	impulseContact.x = desiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0;
	impulseContact.z = 0;
	return impulseContact;
}


inline Vector3D Contact::calculateFrictionImpulse(
	Matrix3x3* inverseInertiaTensor
) {
	Vector3D impulseContact;
	real inverseMass = body[0]->inverseMass;

	// The equivalent of a cross product in matrices is multiplication
	// by a skew symmetric matrix - we build the matrix for converting
	// between linear and angular quantities.
	Matrix3x3 impulseToTorque;
	impulseToTorque.setSkewSymmetric(relativeContactPosition[0]);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	Matrix3x3 deltaVelWorld = impulseToTorque;
	deltaVelWorld = deltaVelWorld * inverseInertiaTensor[0];
	deltaVelWorld = deltaVelWorld * impulseToTorque;
	deltaVelWorld *= -1;

	// Check if we need to add body two's data
	if (body[1])
	{
		// Set the cross product matrix
		impulseToTorque.setSkewSymmetric(relativeContactPosition[1]);

		// Calculate the velocity change matrix
		Matrix3x3 deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 = deltaVelWorld2 * inverseInertiaTensor[1];
		deltaVelWorld2 = deltaVelWorld2 * impulseToTorque;
		deltaVelWorld2 *= -1;

		// Add to the total delta velocity.
		deltaVelWorld = deltaVelWorld + deltaVelWorld2;

		// Add to the inverse mass
		inverseMass += body[1]->inverseMass;
	}

	// Do a change of basis to convert into contact coordinates.
	Matrix3x3 deltaVelocity = contactToWorld.transpose();
	deltaVelocity = deltaVelocity * deltaVelWorld;
	deltaVelocity = deltaVelocity * contactToWorld;

	// Add in the linear velocity change
	deltaVelocity.data[0] += inverseMass;
	deltaVelocity.data[4] += inverseMass;
	deltaVelocity.data[8] += inverseMass;

	// Invert to get the impulse needed per unit velocity
	Matrix3x3 impulseMatrix = deltaVelocity.inverse();

	// Find the target velocities to kill
	Vector3D velKill(desiredDeltaVelocity,
		-contactVelocity.y,
		-contactVelocity.z);

	// Find the impulse to kill target velocities
	impulseContact = impulseMatrix.transform(velKill);

	// Check for exceeding friction
	real planarImpulse = realSqrt(
		impulseContact.y * impulseContact.y +
		impulseContact.z * impulseContact.z
	);

	if (planarImpulse > impulseContact.x * friction) {
		// We need to use dynamic friction
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;

		impulseContact.x = deltaVelocity.data[0] +
			deltaVelocity.data[1] * friction * impulseContact.y +
			deltaVelocity.data[2] * friction * impulseContact.z;
		impulseContact.x = desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= friction * impulseContact.x;
		impulseContact.z *= friction * impulseContact.x;
	}
	return impulseContact;
}


void Contact::applyVelocityChange(
	Vector3D velocityChange[2],
	Vector3D rotationChange[2]
) {
	// Get hold of the inverse mass and inverse inertia tensor, both in
	// world coordinates.
	Matrix3x3 inverseInertiaTensor[2];
	inverseInertiaTensor[0] = body[0]->inverseInertiaTensor;
	if (body[1])
		inverseInertiaTensor[1] = body[1]->inverseInertiaTensor;

	// We will calculate the impulse for each contact axis
	Vector3D impulseContact;

	if (friction == (real)0.0)
	{
		// Use the short format for frictionless contacts
		impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	}
	else
	{
		// Otherwise we may have impulses that aren't in the direction of the
		// contact, so we need the more complex version.
		impulseContact = calculateFrictionImpulse(inverseInertiaTensor);
	}

	// Convert impulse to world coordinates
	Vector3D impulse = contactToWorld.transform(impulseContact);

	// Split in the impulse into linear and rotational components
	Vector3D impulsiveTorque = relativeContactPosition[0].vectorProduct(impulse);
	rotationChange[0] = inverseInertiaTensor[0].transform(impulsiveTorque);
	velocityChange[0].clear();
	velocityChange[0].linearCombination(impulse, body[0]->inverseMass);

	body[0]->linearVelocity += velocityChange[0];
	body[0]->angularVelocity += rotationChange[0];

	if (body[1]) {

		// Work out body one's linear and angular changes
		Vector3D impulsiveTorque = impulse.vectorProduct(relativeContactPosition[1]);
		rotationChange[1] = inverseInertiaTensor[1].transform(impulsiveTorque);
		velocityChange[1].clear();
		velocityChange[1].linearCombination(impulse, -body[1]->inverseMass);

		body[1]->linearVelocity += velocityChange[1];
		body[1]->angularVelocity += rotationChange[1];
	}
}



void Contact::calculateDesiredDeltaVelocity(real duration) {

	const static real velocityLimit = (real)1.0f;

	// Calculate the acceleration induced velocity accumulated this frame
	real velocityFromAcc = 0;

	/*
		This part here in this function is done so that the velocity
		built up from last frame's acceleration does not cause instability
		when we have resting contacts (object at rest, not moving).
		The instability occurs if the velocity built up each frame when the
		object slightly clips into another causes a larger than expected
		impulse, so we get rid of it.
	*/
	if (body[0]->isAwake) {
		velocityFromAcc +=
			(body[0]->lastFrameAcceleration.scalarProduct(contactNormal)) * duration;
	}
	if (body[1] && body[1]->isAwake) {
		velocityFromAcc -=
			(body[1]->lastFrameAcceleration.scalarProduct(contactNormal)) * duration;
	}

	/*
		In the same vein, when the velocity is very slow, indicating that
		the object is at rest, we limit the restitutuion so that the resting
		contact impulse does not cause bouncing.
	*/
	real thisRestitution = restitution;
	if (realAbs(contactVelocity.x) < velocityLimit) {
		thisRestitution = 0;
	}

	// Combine the bounce velocity with the removed
	// acceleration velocity.
	desiredDeltaVelocity =
		-contactVelocity.x
		- thisRestitution * (contactVelocity.x - velocityFromAcc);
}


void Contact::matchAwakeState(){
	// Collisions with the world never cause a body to wake up.
	if (!body[1]) return;

	bool body0awake = body[0]->isAwake;
	bool body1awake = body[1]->isAwake;

	// Wake up only the sleeping one
	if (body0awake ^ body1awake) {
		if (body0awake) body[1]->setAwake(true);
		else body[0]->setAwake(true);
	}
}