/*
	Class that holds the relevant information and processing for a contact
	between two particles, or a particle and an immovable object.

	The class asseses if a contact will happen (if the velocities are
	closing), and calculates the resulting velocities after the contact 
	using the conservation of momentum formula and the closing velocity
	formula.

	The contact is resolved using impulses instead of forces, where an
	impulse changes the velocity directly instead of changing the 
	acceleration. Moreover, the sum of impulses is proportional to the
	change in velocity, not the overall velocity. Impulses are also applied
	only instantaneously, and don't linger as forces do for several frames.
*/

#ifndef PARTICLE_CONTACT_H
#define PARTICLE_CONTACT_H

#include "particle.h"

namespace pe {

	class ParticleContact {

	public:

		/*
			Pointer to the two particles in contact (if the second object
			is immovable, or not a particle, the second pointer is kept
			NULL).
		*/
		Particle* particle[2];

		/*
			Normal vector in the direction of object 1 to object 2. The
			velocities are considered only in the direction of this vector.
			For particles, it can be inferred from their positions, but the
			vector has to be made explicit for more complex objects.
		*/
		Vector3D contactNormal;
		/*
			Coefficient of restitution, which determine how quickly the two
			objects bounce off of each other, where 1 has them bounce back
			with the same overall speed of the colission, and 0 having them
			be stuck together.
		*/
		real restitutionCoefficient;

		/*
			The amount one object is inside another. The burden of
			calculating the value doesn't lie in the physics engine, 
			but because, depending on the restitution value, the objects
			might not move after the collision, the objects have to be 
			moved just enough to eliminate interpenetration.
		*/
		real interpenetration;

		// Calculates the necessary impulses in case of a contact
		void resolveVelocity(real duration);

		/*
			Resolves interpenetration if it still exists after resolving the
			collision velocities. The amount each particle is moved is 
			proportional to their mass, since it makes little sense to move
			a wall as much as the object colliding with it in order to
			remove their interpenetration.
		*/
		void resolveInterpenetration(real duration);

		// Calculates the seperation velocity from the closing velocity
		real calculateSeperationVecolity() const;

		// Resolves the contact
		void resolveContact(real duration);

	};		
}

#endif