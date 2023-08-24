/*
	Header file for the particle cable class, which represents a cable
	connecting two particle. This cable can be bouncy or not depending
	on its restitution value. The objects can get closer to each other
	than the length of the cable, but no further.
*/

#ifndef PARTICLE_CABLE_H
#define PARTICLE_CABLE_H

#include "particleLink.h"

namespace pe {

	class ParticleCable : public ParticleLink {

	public:

		// Maximum length that the objects can be apart
		real maxLength;

		/*
			The restitution coefficient, which determines how bouncy the
			cable is.
		*/
		real restitutionCoefficient;

		virtual unsigned int fillContact(ParticleContact* contact, unsigned
			int limit) const;
	};
}

#endif