/*
	Header file for class that represents a rod connecting two particles.
	The rod allows the objetcs to neither get closer or go further away.
*/

#ifndef PARTICLE_ROD_H
#define PARTICLE_ROD_H

#include "particleLink.h"

namespace pe {

	class ParticleRod : public ParticleLink {

	public:

		// Length of the rod
		real length;

		virtual unsigned int fillContact(ParticleContact* contact, unsigned
			int limit) const;
	};
}

#endif