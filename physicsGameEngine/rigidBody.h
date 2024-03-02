/*
	Class representing a rigid body. Works the same way the particle class
	does, but with some extra data like torque (rotation equivalent of a
	force), orientation, and their associated member functions.

	Objects like cubes, spheres, and pyramids that we want to simulate
	extend this class to add their own shape specific characteristics.
*/

#ifndef RIGID_BODY
#define RIGID_BODY

#include <assert.h>
#include "accuracy.h"
#include "vector3D.h"
#include "matrix3x4.h"
#include "matrix3x3.h"

namespace pe {

	class RigidBody {

	private:

	public:

		/*
			The mass of the object needs to be non-zero, otherwise the
			acceleration equation a = f*1/m leads to a division by 0.
			Moreover, we need a way to represent objects with infinite
			mass (like walls). One solution is to store 1/m of the object,
			which is 0 when the object weighs infinity, and which can't
			lead to a 0 mass.
		*/
		real inverseMass;

		/*
			Inertia is to rotation what mass is to translation.
			It has to do with the ease of rotating a body along some axis.
			It is represented by a tensor, a generelization of a matrix,
			which in this case, is a 3 by 3 matrix.
			The inertia tensor represents the inertia of the body for any
			axis it may be rotating around.
			The shape of the body influences how the matrix is calculated,
			so this value is only set when the class is extended.
			This value also holds the inverse of the tensor, as the inverse
			is used in the equation rotational acceleration = torque *
			inverse inertia tensor.
			We can get the inertia tensor by summing masses at different
			distances from an axis of rotation.
			Since the inertia tensor is given in local coordinates (relative
			to the object), it doesn't change each time the object moves,
			and does not need updating.
			Note that when torque is applied, the object doesn't always
			rotate in the same axis as the torque.
		*/
		Matrix3x3 inverseInertiaTensor;

		/*
			Since torque and rotational acceleration are given in terms of
			world coordinates, so instead of recalculating the inertia
			tensor at each frame, we instead transform our local inertia
			tensor into a world coordinate-inertia tensor using a function,
			and recalculating the value at each frame.
		*/
		Matrix3x3 inverseInertiaTensorWorld;

		/*
			Holds the position of the origin of the rigid body.
			With specific shapes, the position of other points are saved
			in relation to the origin.
		*/
		Vector3D position;


		/*
			To avoid gimbal locks, we use quaternions instead of Euler
			angles (3D vectors) to save the orientation of the object.
		*/
		Quaternion orientation;

		
		/*
			Holds the linear veolicty of the body.
		*/
		Vector3D linearVelocity;

		/*
			Holds the angular velocity of the object.
			With rotations, unlike orientations, gimbal lock is not an
			issue, so we use a 3D vector to hold the value of rotations.
			This is also sometimes called the rotation of the body.
		 */
		Vector3D angularVelocity;

		/*
			Holds the constant non-changing linear acceleration of the body.
			This acceleration does not change and is not removed.
			An example can be gravity. During integration, any new
			acceleration is added to it (though the variable itself is not
			changed,
		*/
		Vector3D acceleration;

		/*
			Holds the linear acceleartion of the last frame, after having
			applied the forces. This value is changed each frame, and
			is only a memebr variable in order to allow us to determine
			the acceleration at the last frame.
		*/
		Vector3D lastFrameAcceleration;

		/*
			Matrix used in transforming local coordinates (relative to body
			origin), to world coordinates, and vice-versa using its inverse.
			Updated once per frame to keep up with the orientation and
			position of the object.
		*/
		Matrix3x4 transformMatrix;

		/*
			Force accumulators work for rigid bodies in the same way that
			they did for particle. The net result of applying all the forces
			is the same as applying a single force that is their sum.
		*/
		Vector3D forceAccumulator;

		/*
			Torque accumulators work in the same way, by summing the
			different torques applied to th body. However, linear force
			can also contriibute to torque. So using a formula that relates
			the two (torque = force x vector of relative position of the
			point of application from the origin, where x is the vector
			product), forces that don't pass through the origin contribute
			to the torque.
		*/
		Vector3D torqueAccumulator;

		/*
			Through numerical instability, energy may be added to the rigid
			body, so we include a damping value that dampens the
			acceleration of the object. This exsists for linear and angular
			motion.
		*/ 
		real linearDamping;
		real angularDamping;

	public:


		bool isAwake;


		RigidBody() : isAwake{true} {};

		/*
			Calculates internal data of the object based on its state.
			Used to update things like the transform matrix based on the
			current orientation and orientation of the data.
		*/
		void calculateDerivedData();

		void setInertiaTensor(const Matrix3x3& inertiaTensor);

		/*
			Add a force to the object without an application point,
			which we assume means the force is added to the origin. We
			automatically discard the possibility that the force can
			contribute to the overall torque, so we only add it to the
			forc accumulator.
		*/
		void addForce(const Vector3D& force);

		// Clears the accumulators each frame so they can be refilled
		void clearAccumulators();

		/*
			Rotates and translates the object according to the rotational
			and linear accelerations that we get from the force and torque
			accumulators.
		*/
		void integrate(real duration);

		/*
			Adds a force to the body, applied at a point, which may
			contribute to the torque applied on the object.
			Note that the force direction and point of application
			are both given in terms of world coordinates, not local
			coordinates.
		*/
		void addForce(const Vector3D& force, const Vector3D& point);

		/*
			Adds torque to the body, which only contributes to the torque
			accumulator. Torque is not added at any specific point.
		*/
		void addTorque(const Vector3D& torque);

		/*
			Adds a force at a point given relative to the origin of the
			body. The function thus converts it to world coordinates
			based on teh object's location before adding the force to
			the torque accumulator.
			The direction however is still assumed ot be given in world
			coordinates.
		*/
		void addForceAtLocalPoint(const Vector3D& force,
			const Vector3D& point);

		/*
			Used to set mass directly unless we need to set an infinite
			mass, in which case it can be set directly.
		*/
		void setMass(real mass);

		// Returns mass if inverse is not 0, otherwise 0 is returned
		real getMass() const;

		// Checks if mass is finite
		bool hasFiniteMass() const;

		/*
			Assumes the sent point is in local coordinates and returns it
			in world coordinates using the transform matrix.
		*/
		Vector3D getPointInWorldCoordinates(const Vector3D& point) const;

		/*
			Assumes the sent point is in world coordinates and returns it
			in local coordinates using the transform matrix.
		*/
		Vector3D getPointInLocalCoordinates(const Vector3D& point) const;


		// Sets the wake status
		void setAwake(bool isAwake);

	};
}

#endif