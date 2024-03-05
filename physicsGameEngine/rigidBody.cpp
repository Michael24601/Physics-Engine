// Source file of the rigid body class

#include "rigidBody.h"

using namespace pe;

/*
	Creates a transform matrix from the orientationand position of the
	object.
	The value transformMatrix is sent and filled with the result.
*/
static inline void calculateTransformMatrix(Matrix3x4& transformMatrix,
	const Vector3D& position, const Quaternion& orientation) {
	transformMatrix.data[0] = 1 - 2 * orientation.j * orientation.j -
		2 * orientation.k * orientation.k;
	transformMatrix.data[1] = 2 * orientation.i * orientation.j -
		2 * orientation.r * orientation.k;
	transformMatrix.data[2] = 2 * orientation.i * orientation.k +
		2 * orientation.r * orientation.j;
	transformMatrix.data[3] = position.x;
	transformMatrix.data[4] = 2 * orientation.i * orientation.j +
		2 * orientation.r * orientation.k;
	transformMatrix.data[5] = 1 - 2 * orientation.i * orientation.i -
		2 * orientation.k * orientation.k;
	transformMatrix.data[6] = 2 * orientation.j * orientation.k -
		2 * orientation.r * orientation.i;
	transformMatrix.data[7] = position.y;
	transformMatrix.data[8] = 2 * orientation.i * orientation.k -
		2 * orientation.r * orientation.j;
	transformMatrix.data[9] = 2 * orientation.j * orientation.k +
		2 * orientation.r * orientation.i;
	transformMatrix.data[10] = 1 - 2 * orientation.i * orientation.i -
		2 * orientation.j * orientation.j;
	transformMatrix.data[11] = position.z;
}


/*
	Calculates the world coordinate version of the inertia tensor
	from the local version. Recalculated at each frame.
	The value inverseInertiaTensorWorld is sent and filled with the result.
*/
static inline void transformInertiaTensor(
	Matrix3x3& inverseInertiaTensorWorld, const Quaternion& q,
	const Matrix3x3& inverseInertiaTensor, const Matrix3x4& rotationMatrix) {

	real t4 = rotationMatrix.data[0] * inverseInertiaTensor.data[0] +
		rotationMatrix.data[1] * inverseInertiaTensor.data[3] +
		rotationMatrix.data[2] * inverseInertiaTensor.data[6];
	real t9 = rotationMatrix.data[0] * inverseInertiaTensor.data[1] +
		rotationMatrix.data[1] * inverseInertiaTensor.data[4] +
		rotationMatrix.data[2] * inverseInertiaTensor.data[7];
	real t14 = rotationMatrix.data[0] * inverseInertiaTensor.data[2] +
		rotationMatrix.data[1] * inverseInertiaTensor.data[5] +
		rotationMatrix.data[2] * inverseInertiaTensor.data[8];
	real t28 = rotationMatrix.data[4] * inverseInertiaTensor.data[0] +
		rotationMatrix.data[5] * inverseInertiaTensor.data[3] +
		rotationMatrix.data[6] * inverseInertiaTensor.data[6];
	real t33 = rotationMatrix.data[4] * inverseInertiaTensor.data[1] +
		rotationMatrix.data[5] * inverseInertiaTensor.data[4] +
		rotationMatrix.data[6] * inverseInertiaTensor.data[7];
	real t38 = rotationMatrix.data[4] * inverseInertiaTensor.data[2] +
		rotationMatrix.data[5] * inverseInertiaTensor.data[5] +
		rotationMatrix.data[6] * inverseInertiaTensor.data[8];

	real t52 = rotationMatrix.data[8] * inverseInertiaTensor.data[0] +
		rotationMatrix.data[9] * inverseInertiaTensor.data[3] +
		rotationMatrix.data[10] * inverseInertiaTensor.data[6];
	real t57 = rotationMatrix.data[8] * inverseInertiaTensor.data[1] +
		rotationMatrix.data[9] * inverseInertiaTensor.data[4] +
		rotationMatrix.data[10] * inverseInertiaTensor.data[7];
	real t62 = rotationMatrix.data[8] * inverseInertiaTensor.data[2] +
		rotationMatrix.data[9] * inverseInertiaTensor.data[5] +
		rotationMatrix.data[10] * inverseInertiaTensor.data[8];
	inverseInertiaTensorWorld.data[0] = t4 * rotationMatrix.data[0] +
		t9 * rotationMatrix.data[1] +
		t14 * rotationMatrix.data[2];
	inverseInertiaTensorWorld.data[1] = t4 * rotationMatrix.data[4] +
		t9 * rotationMatrix.data[5] +
		t14 * rotationMatrix.data[6];
	inverseInertiaTensorWorld.data[2] = t4 * rotationMatrix.data[8] +
		t9 * rotationMatrix.data[9] +
		t14 * rotationMatrix.data[10];
	inverseInertiaTensorWorld.data[3] = t28 * rotationMatrix.data[0] +
		t33 * rotationMatrix.data[1] +
		t38 * rotationMatrix.data[2];
	inverseInertiaTensorWorld.data[4] = t28 * rotationMatrix.data[4] +
		t33 * rotationMatrix.data[5] +
		t38 * rotationMatrix.data[6];
	inverseInertiaTensorWorld.data[5] = t28 * rotationMatrix.data[8] +
		t33 * rotationMatrix.data[9] +
		t38 * rotationMatrix.data[10];
	inverseInertiaTensorWorld.data[6] = t52 * rotationMatrix.data[0] +
		t57 * rotationMatrix.data[1] +
		t62 * rotationMatrix.data[2];
	inverseInertiaTensorWorld.data[7] = t52 * rotationMatrix.data[4] +
		t57 * rotationMatrix.data[5] +
		t62 * rotationMatrix.data[6];
	inverseInertiaTensorWorld.data[8] = t52 * rotationMatrix.data[8] +
		t57 * rotationMatrix.data[9] +
		t62 * rotationMatrix.data[10];
}


void RigidBody::calculateDerivedData() {
	/*
		Since a quaternion has 4 degrees of freedom, we need to normalize
		it or it might not represent a rotation. We don't know how the
		torque will affect the orientation, so we normalize it every frame.
	*/
	orientation.normalize();

	// Calculates the transform matrix for the body
	calculateTransformMatrix(transformMatrix, position, orientation);

	// Calculates the Inertia Tensor in world coordinates
	transformInertiaTensor(inverseInertiaTensorWorld, orientation,
		inverseInertiaTensor, transformMatrix);

}

void RigidBody::setInertiaTensor(const Matrix3x3& inertiaTensor) {
	inverseInertiaTensor.setInverse(inertiaTensor);
}


void RigidBody::addForce(const Vector3D& force) {
	forceAccumulator += force;
}


void RigidBody::clearAccumulators() {
	forceAccumulator.clear();
	torqueAccumulator.clear();
}


void RigidBody::integrate(real duration) {

	// If the body is not awake, we don't integrate it
	if (!isAwake) {
		return;
	}

	/*
		Clears the last frame acceleration valueand simply places the
		constant non-changing acceleration in it. Then adds the
		force accumulator to it.
	*/
	lastFrameAcceleration = acceleration;
	lastFrameAcceleration.linearCombination(forceAccumulator, inverseMass);

	/*
		Calculate angular acceleration from torque inputs, which is only a
		local variable.
	*/
	Vector3D angularAcceleration =
		inverseInertiaTensorWorld.transform(torqueAccumulator);

	// Adjusts the velocities of the body
	linearVelocity.linearCombination(lastFrameAcceleration, duration);
	angularVelocity.linearCombination(angularAcceleration, duration);

	// Adds damping
	linearVelocity *= realPow(linearDamping, duration);
	angularVelocity *= realPow(angularDamping, duration);

	// Adjusts the position of the body and orientation
	position.linearCombination(linearVelocity, duration);
	// Update angular position.
	orientation.addScaledVector(angularVelocity, duration);

	// Recalculates derived data like the inertia tensor
	calculateDerivedData();

	// Accumulators cleared each frame
	clearAccumulators();
}


void RigidBody::addForce(const Vector3D& force, const Vector3D& point) {
	/*
		The formula which converts force to torque needs a vector relating
		the relative distance of the origin and the point of application,
		so we calculate it.
	*/
	Vector3D d = point - position;

	// The force is immediatly added to the force accumulator
	forceAccumulator += force;
	// For the torque accumulator, we use the vector product on the force
	torqueAccumulator += d.vectorProduct(force);
}


void RigidBody::addTorque(const Vector3D& torque) {
	torqueAccumulator += torque;
}


void RigidBody::addForceAtLocalPoint(const Vector3D& force,
	const Vector3D& point) {
	// We first convert to world coordinates then use the addForce function
	Vector3D worldPoint = getPointInWorldCoordinates(point);
	addForce(force, worldPoint);

}


void RigidBody::setMass(real mass) {
	assert(mass > 0.0);
	inverseMass = ((real)1.0) / mass;
}


real RigidBody::getMass() const {
	if (inverseMass > 0) {
		return 1 / inverseMass;
	}
	else {
		return REAL_MAX;
	}
}


bool RigidBody::hasFiniteMass() const {
	return inverseMass != 0;
}


Vector3D RigidBody::getPointInWorldCoordinates(const Vector3D& point) const {
	return transformMatrix.transform(point);
}


Vector3D RigidBody::getPointInLocalCoordinates(const Vector3D& point) const {
	return transformMatrix.inverseTransform(point);
}


void RigidBody::setAwake(bool isAwake){
	this->isAwake = isAwake;
}