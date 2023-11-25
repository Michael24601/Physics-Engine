/*
	Header file for class representing a cable force. A cable force
	is a force that prevents object from being further away from each
	other than its length, but lets them get arbitrarily close.

	The class applies the cable force to one of the bodies directly
	(in the update force function from the interface), but also holds a
	pointer to the other body it's supposed to be applying the force on,
	in order to use its poisition in calculating the force. It does not
	however, modify or apply a force on the body in any way, and a seperate
	force object is needed for that.

	Note that it is better to simulate cables and rods as contacts
	(collisions), not forces directly, so this is just temporary.
*/

#ifndef RIGID_BODY_CABLE_FORCE_H
#define RIGID_BODY_CABLE_FORCE_H

#include "rigidBodyForceGenerator.h"

namespace pe {

	class RigidBodyCableForce : public RigidBodyForceGenerator {

	private:

		/*
			The local(relative) coordinates of the spring's connection to
			the first body.
		*/
		Vector3D connectionPoint;

		/*
			The local(relative) coordinates of the spring's connection to
			the second body.
		*/
		Vector3D otherConnectionPoint;

		// The second body the spring acts on
		RigidBody* otherBody;

		// Default length of spring
		real length;

		// Bounciness of force when the object hits the cable length limit
		real restitutionCoefficient;

	public:

		RigidBodyCableForce(const Vector3D& connectionPoint,
			RigidBody* otherBody, const Vector3D& otherConnectionPoint,
			real length, real restitutionCoefficient);

		virtual void updateForce(RigidBody* body, real duration);
	};
}

#endif