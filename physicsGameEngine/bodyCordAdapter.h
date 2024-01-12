
/*
	Connects a body visually to an anchor point using a cord particle mesh,
	while under the hood, using a spring for the connection (for stability).
*/

#ifndef BODY_CORD_ADAPTER_H
#define BODY_CORD_ADAPTER_H

#include "cord.h"
#include "rigidBody.h"
#include "particleSpringForce.h"
#include "rigidBodySpringForce.h"

namespace pe {

	class BodyCordAdapter {

	public:

		RigidBody* body;
		Cord* cord;
		Vector3D connectionPoint;

		/* 
			Dummy body used to simulate a connection between the cord and
			the body.
		*/
		RigidBody dummyBody;

		RigidBodySpringForce rigidBodySpringForce;

		BodyCordAdapter(
			RigidBody* body, 
			Cord* cord, 
			const Vector3D& connectionPoint,
			real length,
			real strength,
			const Vector3D& anchor
		) : 
			body{ body }, 
			cord{ cord }, 
			connectionPoint{connectionPoint},
			rigidBodySpringForce(
				connectionPoint,
				&dummyBody,
				Vector3D(0, 0, 0),
				strength,
				length
			){

			/*
				Positions the dummy body to pull the body in that direction.
			*/
			dummyBody.position = anchor;
			// Needs to be calculated only once initially, since it won't move
			dummyBody.calculateDerivedData();
		}


		/*
			Repositions the end of the rope so that it always follows the 
		*/
		void reposition() {
			/*
				Repositions the end of the cord so that it connects to the
				end of the body.
			*/
			Vector3D position = body->transformMatrix.transform(connectionPoint);
			cord->particles[cord->particleNumber - 1].position = position;

			cord->particles[0].position = dummyBody.position;
		}


		/*
			Updates (adds) forces to the body and last particle of the cord
			to pull them in the correct direction.
		*/
		void updateForce(real duration) {
			// The spring forces attaching the cord particles
			for (auto& force : cord->forces) {
				force.force1.updateForce(force.force2.otherParticle, duration);
				force.force2.updateForce(force.force1.otherParticle, duration);
			}

			// The force keeping the cube attached to the anchor
			rigidBodySpringForce.updateForce(body, duration);
		}
	};
}

#endif