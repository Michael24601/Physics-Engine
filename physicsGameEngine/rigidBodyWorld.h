/*
	Header file for the Rigid Body World, which keeps track of all the
	which works in the same way as the particle world class but with rigid
	bodies.

	When the class is used properly, the game loop should look like this:

	while (!gameOver) {
		world.startFrame();

		// Add any necessary forces or torque

		world.runPhysics(duration);

		// Changes the graphics based on the physics
		runGraphicsUpdate(duration);
	}
*/

#ifndef RIGID_BODY_WORLD_H
#define RIGID_BODY_WORLD_H

#include "rigidBody.h"

namespace pe {

	class RigidBodyWorld {

	private:

		struct RigidBodyRegistration;
		typedef RigidBodyRegistration* RigidBodyRegistrationPtr;

		/*
			Struct that represents a node in the linked list holding all of
			the bodies of the world.
		*/
		struct RigidBodyRegistration {
			// Particle in the node
			RigidBody* rigidBody;
			// Next particle in the node
			RigidBodyRegistrationPtr next;
		};

		// Pointer to the first particle in the linked list
		RigidBodyRegistrationPtr firstRigidBody;


		/*
			Clears the force and torque accumulators from all the rigid
			bodies in the world in order to prepare them for the start
			of the frame where force and torque are added accordingly.
			Also calculates any new data based on the orientation and
			position of the object (like the inertia tensor and transform
			matrix.
		*/
		void startFrame();

		/*
			Once the startFrame function is called, and any needed forces
			are added afterwards (or before the startFrame is called,
			it doesn't matetr)
		*/
		void runPhysics(real duration);

	};
}

#endif