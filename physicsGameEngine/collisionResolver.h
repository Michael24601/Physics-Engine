
#ifndef COLLISION_RESOLVER_H
#define COLLISION_RESOLVER_H

#include "contact.h"

namespace pe{

	class CollisionResolver {

	private:

		/*
			Prepares the contacts for processing. That means tha internals
			are all calculated.
		*/
		void prepareContacts(
			Contact* contactArray, 
			unsigned int contactNumber,
			real duration
		);


		/*
			Resolves all the interpenetrations.
			Because resolving one interpenetration may cause another to
			dissapear or a new one to appear, this function finds the worst
			interpenetration, resolves it, updates all the penetrations,
			and then repeats the process.
		*/
		void adjustPositions(
			Contact* c,
			unsigned numContacts,
			real duration
		);


		/*
			Resolves all velocities.
		*/
		void adjustVelocities(
			Contact* c,
			unsigned numContacts,
			real duration
		);


		/*
			Number of iterations to perform when resolving velocity.
			Since resolving one velocity may cause other collisions in the
			same frame, we need a limit on the number of allowed iterations.
		*/
		unsigned int velocityIterations;

		/*
			Number of iterations to perform when resolving penetration.
			Since resolving one penetration may cause other interpenetrations
			in the same frame, we need a limit on the number of allowed
			iterations.
		*/
		unsigned int positionIterations;


		/*
			To avoid instability, any velocity smaller than this value is
			too small to consider, and is considered to be 0. By default
			it should be about 0.01.
		*/
		real velocityEpsilon;

		/*
			To avoid instability, any penetration smaller than this value 
			is too small to consider, and is considered to be 0. By default
			it should be about 0.01.
		*/
		real positionEpsilon;


	public:

		/*
			The number of velocity iterations used the last time the resolve
			velocities function was called.
		*/
		unsigned int velocityIterationsUsed;

		/*
			The number of position iterations used the last time the resolve
			contacts function was called.
		*/
		unsigned int positionIterationsUsed;


		/*
			Takes the maximum number of allowed velocity and position
			iterations each frame for all the collisions we have.
			By default, the epsilon values we have are 0.01.
		*/
		CollisionResolver(
			unsigned int velocityIterations,
			unsigned int positionIterations,
			real velocityEpsilon = (real)0.01,
			real positionEpsilon = (real)0.01
		);


		/*
			Resolves both the interpenetration and velocity changes (makes
			bodies rebound and moves them so they no longer inter-penetrate).

			The two are independent, so we resolve all interpenetrations
			first, then all contacts (velocity changes).
			Note that the contacts are resolved in order of most severe,
			where the most severe is the one with the largest desired delta
			velocity.
		*/
		void resolveContacts(
			Contact* contactArray,
			unsigned int contactNumber,
			real duration
		);
	};
}

#endif