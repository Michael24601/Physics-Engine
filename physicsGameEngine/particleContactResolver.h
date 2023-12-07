// Header file for class that resolves all particle contacts each frame

#ifndef PARTICLE_CONTACT_RESOLEVR
#define PARTICLE_CONTACT_RESOLVER

#include <algorithm>
#include <vector>
#include "particleContact.h"

namespace pe {

	class ParticleContactResolver {

	private:

		/*
			When a collision is resolved, it may cause a previously resolved
			collision to again enter into a collision. So we may have to
			repeat the previous collisions every time a collision is 
			resolved. This value specifes how many times we are allowed to
			do that. Usually, the same number of iterations as contacts
			works for most situations, but a general rule of thumb is that
			twice the number of contacts is safer.
		*/
		unsigned int iterations;

	public:

		// No-arg constructor
		ParticleContactResolver() : iterations{} {};

		// Setter for the number of allowed iterations
		void setIterations(unsigned int iterations) {
			this->iterations = iterations;
		}

		/*
			Resolves an array of contacts. Because a contact being resolved
			can affect other contacts, we always start with the most severe
			collisions. Moreover, the function deals with interpenetrations
			after dealing with each collision.
		*/
		void resolveContacts(
			std::vector<ParticleContact> particleContactArray,
			real duration
		) {
			int numberOfContacts = particleContactArray.size();
			if (numberOfContacts > 0) {

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

				while (iterationCount < iterations && iterationCount < numberOfContacts) {

					// Resolves the most severe collision first
					auto mostSevereCollision = *std::min_element(particleContactArray.begin(),
						particleContactArray.end(), [](const
							ParticleContact& first, const ParticleContact& second) ->
						bool { return first.calculateSeperationVecolity()
						< second.calculateSeperationVecolity();
						});

					mostSevereCollision.resolveContact(duration);
					iterationCount++;
				}
			}
		}
	};
}

#endif