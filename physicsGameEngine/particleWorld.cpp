// Source file for the particle world class

#include "particleWorld.h"

using namespace pe;

void ParticleWorld::startFrame() {
	ParticleRegistrationPtr ptr = firstParticle;
	// Loops over the particles
	while (ptr) {
		// Clears the accumulators of all the particles
		ptr->particle->clearAccumulatedForce();
		ptr = ptr->next;
	}
}


unsigned int ParticleWorld::generateContacts() {
	unsigned int limit = maxNumberOfContacts;

	/*
		Points to the next available index in the contacts array, starting
		with 0.
	*/
	ParticleContact* nextContact = contacts;

	// Pointer that iterates over the contact generators
	ContactGeneratorRegistrationPtr ptr = firstContactGenerator;

	while (ptr) {
		unsigned int generatedContactsNumber = ptr->generator->addContact(
			nextContact, limit);
		// Lowers the limit by the number of contacts already generated
		limit -= generatedContactsNumber;
		/*
			The next available contact is after a gap the size of the number
			of generated contacts.
		*/
		nextContact += generatedContactsNumber;

		/*
			When the number of generated contacts hits the limit, no more
			contacts are generated.
		*/
		if (limit <= 0) {
			break;
		}

		ptr = ptr->next;
	}

	// The number of contacts generated (can be up to maxNumberOfContacts)
	return maxNumberOfContacts - limit;
}


void ParticleWorld::integrate(real duration) {
	ParticleRegistrationPtr ptr = firstParticle;
	while (ptr) {
		// Integrates each particle in turn (and clears their accumulator)
		ptr->particle->integrate(duration);
		ptr = ptr->next;
	}
}


void ParticleWorld::runPhysics(real duration) {
	// First the forces are applied to the particles
	registry.updateAll(duration);

	// Then the objects are integrated (moved)
	integrate(duration);

	// The contacts are then generated and the count is kept record of
	unsigned int numberOfContacts = generateContacts();

	// If there are contacts to be resolved
	if (numberOfContacts) {
		// If the max number of iterations has been set and is not 0
		if (maxIterations) {
			resolver.setIterations(maxIterations);
		}
		// Else we assume we should use twice the number of contacts
		else{
			resolver.setIterations(numberOfContacts * 2);
		}
		// resolver.resolveContacts(contacts, numberOfContacts, duration);
	}
}