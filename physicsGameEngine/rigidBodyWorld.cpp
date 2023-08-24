// Source file for the Rigid Body World class

#include "rigidBodyWorld.h"

using namespace pe;

void RigidBodyWorld::startFrame() {
	RigidBodyRegistrationPtr ptr = firstRigidBody;
	// Loops over the particles
	while (ptr) {
		// Clears the accumulators of all the particles
		ptr->rigidBody->clearAccumulators();
		// Calculates the new data
		ptr->rigidBody->calculateDerivedData();
		ptr = ptr->next;
	}
}

void RigidBodyWorld::runPhysics(real duration)
{
	// First apply the force generators, then integrate the objects
	RigidBodyRegistrationPtr ptr = firstRigidBody;
	while (ptr){
		// Remove all forces from the accumulator
		ptr->rigidBody->integrate(duration);
		ptr = ptr->next;
	}
}