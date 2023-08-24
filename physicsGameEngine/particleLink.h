/*
	Header file for interface that represents two particles linked together.
	This class is meant to be extended by whichever class represents the
	actual link, which can range from cables, rods, etc... Particle contacts
	are used to simulate the connection.
*/

#ifndef PARTICLE_LINK_H
#define PARTICLE_LINK_H

#include "particleContact.h"

namespace pe {

	class ParticleLink {

	protected:

		// Returns the current length of the link, which may be changing
		real currentLength() const;

	public:

		// Particles connected to each other
		Particle* particle[2];

		/*
			Function that creates the particle contacts necessary to resolve
			the connection in case the objects get too close/far from one
			another. The contact pointer is filled with an array of the 
			necessary contacts. The number of contacts is returned by the
			function. The limit parameter limits the number of contacts that
			can be generated.
		*/
		virtual unsigned int fillContact(ParticleContact* contact,
			unsigned int limit) const = 0;

	};
}

#endif

