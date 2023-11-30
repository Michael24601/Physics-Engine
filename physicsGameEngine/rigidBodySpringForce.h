/*
	Header file for class representing a spring force between two
	objects connected by the spring. An instance of this class represents
	the force acting on one of the objects. The spring's force is excerted
	both when compressed or elongated.
	
	The limitations of workings of this force are the same as the spring
	force used between particles.

	The class applies a spring force to one of the bodies directly
	(in the update force function from the interface), but also holds a
	pointer to the other body it's supposed to be applying the force on,
	in order to use its poisition in calculating the force. It does not
	however, modify or apply a force on the body in any way, and a seperate
	force object is needed for that.
*/

#ifndef RIGID_BODY_SPRING_FORCE_H
#define RIGID_BODY_SPRING_FORCE_H

#include "accuracy.h"
#include "vector3D.h"
#include "rigidBodyForceGenerator.h"

namespace pe {

	class RigidBodySpringForce : public RigidBodyForceGenerator {
	
	public:

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

		// Spring constant k, which determines stifness
		real springConstant;

		// Default length of spring
		real restLength;

	public:

		RigidBodySpringForce(const Vector3D& connectionPoint,
			RigidBody* otherBody, const Vector3D& otherConnectionPoint,
			real springConstant, real restLength);

		virtual void updateForce(RigidBody* body, real duration);
	};
}

#endif