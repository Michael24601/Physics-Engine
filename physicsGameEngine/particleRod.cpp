// Source file for the particle rod class

#include "particleRod.h"

using namespace pe;

unsigned int ParticleRod::fillContact(ParticleContact* contact, unsigned
	int limit) const {
	real currentRodLength = currentLength();

	// If the two objects deviate from the length of the rod
	if (currentRodLength != length) {
		contact->particle[0] = particle[0];
		contact->particle[1] = particle[1];

		/*
			The normal is the difference in the position of the two objects
			(normalized) since they are both particle.
		*/
		Vector3D normal = particle[1]->position - particle[0]->position;
		normal.normalize();

		/*
			The contact normal reverses signs depending on whether the
			objects are extending or compressing. Same for the
			interpenetration.
		*/
		if (currentRodLength > length) {
			contact->contactNormal = normal;
			contact->interpenetration = currentRodLength - length;
		}
		else {
			contact->contactNormal = normal * -1;
			contact->interpenetration = length - currentRodLength;
		}

		// Because this is a rod, there is no bounciness, so resitution is 0
		contact->restitutionCoefficient = 0;

		return 1;
	}
	// Else no impulses are needed to properly distance them 
	else {
		return 0;
	}
}