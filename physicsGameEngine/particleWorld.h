/*
	Header file for the Particle World, which keeps track of all the
	particles and updating their positions, collisions, and accumulated
	forces.

	When the class is used properly, the game loop should look like this:

	while (!gameOver) {
		world.startFrame();

		// Add any necessary forces or torque

		world.runPhysics(duration);

		// Changes the graphics based on the physics
		runGraphicsUpdate(duration);
	}
*/

#ifndef PARTICLE_WORLD_H
#define PARTICLE_WORLD_H

#include "particleContact.h"
#include "particleContactResolver.h"
#include "particleContactGenerator.h"
#include "particleForceRegistry.h"
#include "particleForceGenerator.h"

namespace pe {

	class ParticleWorld {

	private:

		struct ParticleRegistration;
		typedef ParticleRegistration* ParticleRegistrationPtr;

		/*
			Struct that represents a node in the linked list holding all of
			the particles of the world.
		*/
		struct ParticleRegistration {
			// Particle in the node
			Particle* particle;
			// Next particle in the node
			ParticleRegistrationPtr next;
		};

		// Pointer to the first particle in the linked list
		ParticleRegistrationPtr firstParticle;

		// Registry that records the forces associated with each particle
		ParticleForceRegistry registry;

		// Resolver of the contacts
		ParticleContactResolver resolver;

		struct ContactGeneratorRegistration;
		typedef ContactGeneratorRegistration* ContactGeneratorRegistrationPtr;

		/*
			Struct that represents a node in the linked list holding all of
			the contact generators of the world, which create the necessary
			contacts.
		*/
		struct ContactGeneratorRegistration
		{
			ParticleContactGenerator* generator;
			ContactGeneratorRegistrationPtr next;
		};

		// Pointer to the first contact generator in the linked list
		ContactGeneratorRegistrationPtr firstContactGenerator;

		/*
			Holds the list of contacts. It is an array instead of a linked
			list because we can limit the number of contacts generated in
			the generator class.
		*/
		ParticleContact* contacts;

		/*
			Maximum number of contacts allowed to be resolved each frame 
			(as in, the size of the contacts array).
		*/
		unsigned int maxNumberOfContacts;

		/*
			The maximum number of iterations that can be used when resolving
			contacts. If the value is 0 (by default), twice the number of
			contacts (not maxNumberOfContacts) is used.
		*/
		unsigned int maxIterations;

	public:

		/*
			Argumnented constructor, which allows the user to specify the 
			maximum number of contacts and the number of the contact
			resolution iterations (which, if not specified, is assumed to
			be twice the number of contacts per frame). 
		*/
		ParticleWorld(unsigned int maxNumberOfContacts, unsigned int
			maxIterations = 0) : maxIterations{maxIterations} {
			contacts = new ParticleContact[maxNumberOfContacts];
		}

		/*
			Readies the world for the start of a frame. Each particle is
			cleared of its accumulated forces, and is ready to recevive
			new ones before being integrated.
		*/
		void startFrame();

		/*
			Calls all of the contact generators in the linked and fills
			the generated contacts in the contacts array. Returns the sum
			of all the generated contacts.
		*/
		unsigned int generateContacts();

		/*
			Integrates all of the particles in the linked list during the
			duration given (likely the frame length). 
		*/
		void integrate(real duration);

		/*
			Runs the physics of the entire world, combining the effects of
			the other functions:
			- First the force generators fill the particle's accumulated
			force vector.
			- Then, the particles are integrated (moved) during the
			duration of the frame.
			- Then the contacts are generated according to the geometry of
			the world by the generators, and are then resolved by the
			particle contact classes. 
		*/
		void runPhysics(real duration);
	};
}

#endif