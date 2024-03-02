
#include "collisionResolver.h"

using namespace pe;


CollisionResolver::CollisionResolver(
	unsigned int velocityIterations,
	unsigned int positionIterations,
	real velocityEpsilon,
	real positionEpsilon
) : velocityIterations{velocityIterations},
positionIterations{ positionIterations }, 
velocityEpsilon{ velocityEpsilon },
positionEpsilon{ positionEpsilon },
positionIterationsUsed{},
velocityIterationsUsed{} {}


void CollisionResolver::prepareContacts(
	Contact* contacts,
	unsigned contactNumber,
	real duration
) {
	for (int i = 0; i < contactNumber; i++) {
		// Calculate the internal contact data (inertia, basis, etc).
		contacts[i].calculateInternals(duration);
	}
}


void CollisionResolver::adjustPositions(
	Contact* c,
	unsigned numContacts,
	real duration
){
	unsigned int i, index;
	Vector3D velocityChange[2], rotationChange[2];
	real rotationAmount[2];
	real max;
	Vector3D cp;

	// Iteratively resolve interpenetration in order of severity.
	positionIterationsUsed = 0;
	while (positionIterationsUsed < positionIterations){

		// Find biggest penetration.
		max = positionEpsilon;
		index = numContacts;
		for (i = 0; i < numContacts; i++) {
			if (c[i].penetration > max)
			{
				max = c[i].penetration;
				index = i;
			}
		}
		if (index == numContacts) break;
		// Match the awake state at the contact.
		//c[index].matchAwakeState();
		// Resolve the penetration.
		c[index].applyPositionChange(
            velocityChange,
			rotationChange,
			max
		);
        //-positionEpsilon);
		// Again this action may have changed the penetration of other
		// bodies, so we update contacts.
		for (i = 0; i < numContacts; i++){

			if (c[i].body[0]){

				if (c[i].body[0] == c[index].body[0]){

					cp = rotationChange[0].vectorProduct(c[i].
						relativeContactPosition[0]);
					cp += velocityChange[0];
					c[i].penetration -=
						rotationAmount[0] * cp.scalarProduct(c[i].
							contactNormal);
				}
				else if (c[i].body[0] == c[index].body[1]){

					cp = rotationChange[1].vectorProduct(c[i].
						relativeContactPosition[0]);
					cp += velocityChange[1];
					c[i].penetration -=
						rotationAmount[1] * cp.scalarProduct(c[i].
							contactNormal);
				}
			}
			if (c[i].body[1]){

				if (c[i].body[1] == c[index].body[0]){

					cp = rotationChange[0].vectorProduct(c[i].
						relativeContactPosition[1]);
					cp += velocityChange[0];
					c[i].penetration +=
						rotationAmount[0] * cp.scalarProduct(c[i].
							contactNormal);
				}
				else if (c[i].body[1] == c[index].body[1]){

					cp = rotationChange[1].vectorProduct(c[i].
						relativeContactPosition[1]);
					cp += velocityChange[1];
					c[i].penetration +=
						rotationAmount[1] * cp.scalarProduct(c[i].
							contactNormal);
				}
			}
		}
		positionIterationsUsed++;
	}
}


void CollisionResolver::adjustVelocities(
	Contact* c,
	unsigned numContacts,
	real duration
){
	Vector3D velocityChange[2], rotationChange[2];
	Vector3D deltaVel;

	// iteratively handle impacts in order of severity.
	velocityIterationsUsed = 0;
	while (velocityIterationsUsed < velocityIterations)
	{
		// Find contact with maximum magnitude of probable velocity change.
		real max = velocityEpsilon;
		unsigned int index = numContacts;
		for (unsigned int i = 0; i < numContacts; i++)
		{
			if (c[i].desiredDeltaVelocity > max)
			{
				max = c[i].desiredDeltaVelocity;
				index = i;
			}
		}
		if (index == numContacts) break;


		// Do the resolution on the contact that came out top.
		c[index].applyVelocityChange(velocityChange, rotationChange);

		// With the change in velocity of the two bodies, the update of
		// contact velocities means that some of the relative closing
		// velocities need recomputing.
		for (unsigned int i = 0; i < numContacts; i++)
		{
			// Check each body in the contact
			for (unsigned int b = 0; b < 2; b++) if (c[i].body[b])
			{
				// Check for a match with each body in the newly
				// resolved contact
				for (unsigned int d = 0; d < 2; d++)
				{
					if (c[i].body[b] == c[index].body[d])
					{
						deltaVel = velocityChange[d] +
							rotationChange[d].vectorProduct(
								c[i].relativeContactPosition[b]);

						// The sign of the change is negative if we're dealing
						// with the second body in a contact.
						Matrix3x3 inverse = c[i].contactToWorld.transpose();
						c[i].contactVelocity +=
							inverse.transform(deltaVel)
							* (b ? -1 : 1);
						c[i].calculateDesiredDeltaVelocity(duration);
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}


void CollisionResolver::resolveContacts(
	Contact* contacts,
	unsigned int contactNumber,
	real duration
) {
	if (contactNumber == 0) return;

	prepareContacts(contacts, contactNumber, duration);

	// Resolved interpenetartion
	adjustPositions(contacts, contactNumber, duration);

	// Resolves contacts, strating with the most severe
	adjustVelocities(contacts, contactNumber, duration);
}