/*
	Particle Contact interface. Like the particle force generator class,
	this interface aims at generalizing the action of generating a contact.
	Unlike the particle contact class, which resolves a contact, this
	class determines when a contact should be generated to be resolved.
*/

#ifndef PARTICLE_CONTACT_GENERATOR_H
#define PARTICLE_CONTACT_GENERATOR_H

#include "particleContact.h"

namespace pe {

	class ParticleContactGenerator {

	public:

		/*
			Fills the contact pointer parameter with an array of generated
			contacts, up to the given limit in the second parameter. The
			pointer should point to the first available index in the array,
			not the overall first index. The number of filled contacts is
			returned by the function. The virtual function should be
			overwritten by subclasses. 
		*/
		virtual unsigned int addContact(ParticleContact* contact, unsigned
			int limit) = 0;

	};
}

#endif