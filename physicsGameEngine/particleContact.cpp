// Source file for the Particle Contact class

#include "particleContact.h"

using namespace pe;

void ParticleContact::resolveContact(real duration) {
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

/* 
	The seperation velocity is the opposite of the closing velocity,
	which is the difference of both object's velocities in the direction
	of the contact normal.
*/
real ParticleContact::calculateSeperationVecolity() const {
	Vector3D relativeVelocity = particle[0]->velocity;
	/* 
		If the second object is a movable particle, its velocity is 
		considered.
	*/

	try{
		if (particle[1] != NULL) {
			relativeVelocity -= particle[1]->velocity;
		}
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Error: Out of range exception caught: " 
			<< e.what() << "\n";
	}
	catch (const std::exception& e) {
		// Handle other exceptions if necessary
		std::cerr << "Error: " << e.what() << "\n";
	}

	return contactNormal.scalarProduct(relativeVelocity);
}

void ParticleContact::resolveVelocity(real duration) {
	real seperationVelocity = calculateSeperationVecolity();

	/*
		If the seperating velocity is positive, then the objects
		are not getting closer, so there is no need for an impulse
	*/
	if (seperationVelocity > 0){
		return;
	}

	// The new seperation velocity depends
	real newSeperationVelocity = -seperationVelocity *
		restitutionCoefficient;

	/*
		This piece of code checks if the collision during the frame was
		caused solely by the acceleration in the last frame. If that is
		the case, we don't want to create a noticeable impulse, but just
		enough of an impulse to negate the velocity. This happens when
		objects are in resting contact, like an object on the floor.
	*/
	Vector3D causedVelocity = particle[0]->acceleration;
	if (particle[1] != NULL) {
		causedVelocity -= particle[1]->acceleration;
	}
	real causedSeperationVelocity = causedVelocity.scalarProduct(
		contactNormal * duration);

	// Removes the seperation velocity
	if (causedSeperationVelocity < 0)
	{
		newSeperationVelocity += restitutionCoefficient 
			* causedSeperationVelocity;
		// Make sure not too much was removed
		if (newSeperationVelocity < 0)
		{
			newSeperationVelocity = 0;
		}
	}

	// The impulese's value depends on the total mass and change in velocity
	real changeInVelocity = newSeperationVelocity - seperationVelocity;
	real totalInverseMass = particle[0]->inverseMass;
	if (particle[1] != NULL) {
		totalInverseMass += particle[1]->inverseMass;
	}

	// If both objects are immovable
	if (totalInverseMass <= 0) {
		return;
	}

	real impulse = changeInVelocity / totalInverseMass;
	Vector3D impulseVector = contactNormal * impulse;

	// The impulse immediatly affects each particle based on their mass
	if(particle[0]->isAwake)
		particle[0]->velocity += impulseVector * particle[0]->inverseMass;
	if (particle[1] != NULL && particle[1]->isAwake) {
		/*
			Same formula, opposite direction, since the contact normal is
			calculated relative to the first particle (object 1 to 2).
		*/
		particle[1]->velocity += impulseVector * -particle[1]->inverseMass;
	}
}

void ParticleContact::resolveInterpenetration(real duration) {

	// If the two objects are still within each other afer the collision
	if (interpenetration > 0) {
		real totalInverseMass = particle[0]->inverseMass;
		if (particle[1] != NULL) {
			totalInverseMass += particle[1]->inverseMass;
		}

		// If either object has finite mass (meaning it can move)
		if (totalInverseMass > 0) {
			Vector3D moveVector = contactNormal * (interpenetration 
				/ totalInverseMass);

			// Then each particle is moved based on its mass
			if(particle[0]->isAwake)
				particle[0]->position += moveVector * particle[0]->inverseMass;

			if (particle[1] != NULL && particle[0]->isAwake) {
				/*
					A minus sign is added as the contact normal is realtive
					to the first object.
				*/
				particle[1]->position += moveVector 
					* -particle[1]->inverseMass;
			}
		}
	}
}