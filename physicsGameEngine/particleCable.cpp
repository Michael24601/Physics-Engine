// Source file for the particle cable class

#include "particleCable.h"

using namespace pe;

unsigned int ParticleCable::fillContact(ParticleContact* contact, unsigned
	int limit) const {
	real currentCableLength = currentLength();

	// If the objects exceed the cable limit
	if (currentCableLength >= maxLength) {
		contact->particle[0] = particle[0];
		contact->particle[1] = particle[1];

		/*
			Because the objects are both particles, the normal is simply
			the vector connecting them, normalized.

			The link between the two is modeled using the impulse and
			interpenetration of a contact.
		*/
		Vector3D normal = particle[1]->position - particle[0]->position;
		normal.normalize();
		contact->contactNormal = normal;
		contact->interpenetration = currentCableLength - maxLength;
		contact->restitutionCoefficient = restitutionCoefficient;

		return 1;

	}
	// Else no contacts are used since the objects are still close
	else {
		return 0;
	}
}