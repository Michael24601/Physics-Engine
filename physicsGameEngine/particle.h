/*
	Header file for particle class, which represents objects that have
	no direction, like bullets, projectiles, fireworks and light 
	particles...
*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>
#include "accuracy.h"
#include <assert.h>
#include "vector3D.h"

namespace pe {

	// Point-mass (no direction)
	class Particle {

	public:

		// Position of particle
		Vector3D position;

		// Velocity of particle
		Vector3D velocity;

		/*
			Acceleration of Particle. The position and velocity of the 
			particle can't be changed directly, as per the second law
			of newton. However, acceleration is changed using forces, 
			which in turn changes the velocity and position.
		*/
		Vector3D acceleration;

		/*
			Basic drag force baked into the object particle, which 
			eliminates added energy (caused by instability). Other drag 
			forces may be added later on the object. 0 drag means there is
			no velocity, 1 drag means the velocity doesn't decrease. A value
			in between means the drag slows down the particle. Usually, 
			since other drag forces can be added later, this value is used
			to only offset numerical instabilities, and should hover around 
			a value slightly less than 1.
		*/
		real damping;

		/*
			The mass of the object needs to be non-zero, otherwise the
			acceleration equation a = f*1/m leads to a division by 0. 
			Moreover, we need a way to represent objects with infinite 
			mass (like walls). One solution is to store 1/m of the object,
			which is 0 when the object weighs infinity, and which can't
			lead to a 0 mass.
		*/
		real inverseMass;

		// Sum of forces acting on the particle
		Vector3D accumulatedForce;

		/*
			Non-awake particles don't move, and remain in place, even
			when forces are applied to them. We use this boolean so that
			external functions, such as distance constraints and collision,
			which may move the particles in order to resolve some equation,
			know that this particle shouldn't move. Likewise for the
			integration in the main game loop.
		*/
		bool isAwake;


		Particle() : isAwake{ true } {}


		/*
			Used to set mass directly unless we need to set an infinite
			mass, in which case it can be set directly.
		*/
		void setMass(real mass);


		// Returns mass if inverse is not 0, otherwise 0 is returned
		real getMass() const;


		// Checks if mass is finite
		bool hasFiniteMass() const {
			return inverseMass != 0;
		}


		/*
			Updates positionand velocity using duration(time between 
			frames). Uses a linear approximation of the integral, so
			may not be perfectly accurate.
			The approximation used here is Euler integration.
		*/
		void integrate(real duration);


		/*
			Updates positionand velocity using duration(time between
			frames). Uses a linear approximation of the integral, so
			may not be perfectly accurate.
			The approximation used here is the verlet integration,
			and is usually better when many particles are involved,
			and connected, such as in cloth simulation.
		*/
		void verletIntegrate(real duration);


		void addForce(const Vector3D& force);


		// Removes all forces each frame before reapplying them
		void clearAccumulatedForce();


		// Sets the wake status
		void setAwake(bool isAwake) {
			this->isAwake = isAwake;
		}
	};
}

#endif