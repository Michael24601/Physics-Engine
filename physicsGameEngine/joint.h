
#ifndef JOINT_H
#define JOINT_H

#include "contact.h"

namespace pe {

	class Joint {

	public:
		/**
		* Holds the two rigid bodies that are connected by this joint.
		*/
		RigidBody* body[2];
		/**
		* Holds the relative location of the connection for each
		* body, given in local coordinates.
		*/
		Vector3D position[2];

		/**
		* Holds the maximum displacement at the joint before the
		* joint is considered to be violated. This is normally a
		* small, epsilon value. It can be larger, however, in which
		* case the joint will behave as if an inelastic cable joined
		* the bodies at their joint locations.
		*/
		real error;


		Joint(
			RigidBody* body1, 
			RigidBody* body2, 
			const Vector3D& connectionPoint1,
			const Vector3D& connectionPoint2, 
			real err
		)

			: error(err) {
			body[0] = body1;
			body[1] = body2;
			position[0] = connectionPoint1;
			position[1] = connectionPoint2;

			// Calculate the world positions of the local points
			Vector3D worldPoint1 = body1->getPointInWorldCoordinates(connectionPoint1);
			Vector3D worldPoint2 = body2->getPointInWorldCoordinates(connectionPoint2);

			// We initially move the bodies so that they are connected
			Vector3D direction = worldPoint2 - worldPoint1;
			body1->position += direction * 0.5;
			body2->position += direction * -0.5;
		}

		/**
		* Generates the contacts required to restore the joint if it
		* has been violated.
		*/
		unsigned int addContact(
			std::vector<Contact>& contacts
		) const {

			// Calculate the position of each connection point in world coordinates
			Vector3D a_pos_world = body[0]->getPointInWorldCoordinates(position[0]);
			Vector3D b_pos_world = body[1]->getPointInWorldCoordinates(position[1]);

			// Calculate the length of the joint
			Vector3D a_to_b = b_pos_world - a_pos_world;
			Vector3D normal = a_to_b;
			normal.normalize();
			real length = a_to_b.magnitude();

			// Check if it is violated
			if (realAbs(length) > error){

				Contact contact;
				contact.body[0] = body[0];
				contact.body[1] = body[1];
				contact.contactNormal = normal;
				contact.contactPoint = (a_pos_world + b_pos_world) * 0.5f;
				contact.penetration = length - error;
				contact.friction = 1.0f;
				contact.restitution = 0;

				contacts.push_back(contact);

				return 1;
			}

			return 0;
		}
	};
}

#endif