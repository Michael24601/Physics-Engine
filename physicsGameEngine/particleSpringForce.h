/*
	Header file for class representing a spring force between two
	objects connected by the spring. An instance of this class represents
	the force acting on one of the objects. The spring's force is excerted
	both when compressed or elongated. 

	One problem with spring forces when implemented like this is that
	with a very stiff spring (large k), and a small frame length
	(low fps), the spring can act unrealistically. In reality, when a
	very stiff spring is compressed due to the object's acceleration,
	it almost immediatly slows down due to the spring's force. But
	because programtically we are working with frames, we have to wait
	the frame's length in time for the spring to start resisting
	compression. For a very stiff spring, that can be enough time for the
	object to have gathered enough velocity to oscillate further than it
	had the time before. This goes on forever, with the overall enegry
	and elongation increasing instead of diminishing. It is therefore
	advisable to not use a large spring coefficient, espcially with low
	framerates.

	One solution is faking stiff springs, an approach that uses the 
	differential equation for the position of the spring endpoint to 
	predict its position, but the approach isn't robust enough for 
	general use. In general, it is best to avoid stiff springs.
*/

#ifndef PARTICLE_SPRING_FORCE_H
#define PARTICLE_SPRING_FORCE_H

#include "particleForceGenerator.h"

namespace pe {

	class ParticleSpringForce : public ParticleForceGenerator {

	public:

		// Other particle connected to the spring (force not acting on it)
		Particle* otherParticle;

		// Spring constant k
		real springConstant;

		// Resting length of the spring l0
		real restingLength;

	public:

		ParticleSpringForce(Particle* otherParticle, real springConstant, 
			real restingLength) : otherParticle{otherParticle}, 
			springConstant{ springConstant }, restingLength{restingLength} {};

		/*
			Follows the formula of force = -k * (| d | -l0) * d where d is 
			the vector (position - otherObjecsPosition)
		*/
		void updateForce(Particle* particle, real duration) const override ;
	};
}

#endif