// Source file for the particle contact resolver class

#include "particleContactResolver.h"

using namespace pe;

void ParticleContactResolver::resolveContacts(ParticleContact*
	particleContactArray, unsigned int numberOfContacts, real duration) {

	int iterationCount = 0;

	/*
		First the collisions are resolved, the ones with the smallest
		(most severe) seperating velocity first. Note that as a result,
		there is no guarantee all contacts will be resolved. But this is
		still the most realistic way to do it, as more severe contacts 
		should realistically be resolved first.

		Note that the interpenetration of any objects affected by the
		collision needs to be recalculated by the collision detection
		algorithm each iteration as well (after each change).
	*/
	while (iterationCount < iterations) {

		// Resolves the most severe collision first
		auto mostSevereCollision = *std::min_element(particleContactArray,
			particleContactArray + numberOfContacts, [](const 
				ParticleContact& first, const ParticleContact& second) -> 
			bool { return first.calculateSeperationVecolity() 
						< second.calculateSeperationVecolity(); 
			});

		mostSevereCollision.resolveContact(duration);
		iterationCount++;
	}
}