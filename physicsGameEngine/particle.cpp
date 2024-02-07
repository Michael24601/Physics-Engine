// Particle class source file

#include "particle.h"

using namespace pe;

void Particle::setMass(real mass) {
	assert(mass > 0.0);
	inverseMass = ((real)1.0) / mass;
}

real Particle::getMass() const {
	if (inverseMass > 0) {
		return 1 / inverseMass;
	}
	else {
		return REAL_MAX;
	}
}

void Particle::integrate(real duration) {
	if (inverseMass > 0) {
		assert(duration > 0.0);
		/*
			First, the position is updated using the formula
			p = p0 + v0 * t + a * t^2/2, where we can ignore the acceleration
			bit as it won't affect much, so we have p = p0 + v0 * t.
		*/
		position.linearCombination(velocity, duration);

		/*
			Calculates new acceleration using a = f * 1 / m (plus whatever we
			decide is the value of acceleration at this point).
		*/
		Vector3D newAcceleration = acceleration;
		newAcceleration.linearCombination(accumulatedForce, inverseMass);

		// Update velocity using v = v0 + a * t
		velocity.linearCombination(newAcceleration, duration);

		/*
			Adds drag effect using v = v * d^t, instead of v = v * d, in 
			order to make drag proportional to the duration.
		*/
		velocity *= realPow(damping, duration);

		// Removes all forces once movement has been updates
		clearAccumulatedForce();
	}
}


void Particle::verletIntegrate(real duration) {
	if (inverseMass > 0) {

		// Converts the time so it is calibrated to be the same as Euler
		duration *= 5;

		// Saves the current position
		Vector3D currentPosition = position;

		Vector3D newAcceleration = acceleration;
		newAcceleration.linearCombination(accumulatedForce, inverseMass);

		// Update the position using Verlet integration
		position += (velocity * duration) 
			+ (newAcceleration * duration * duration * 0.5);

		// Update velocity using the difference in positions
		velocity = (position - currentPosition) * (duration);

		// Apply damping to velocity
		velocity *= realPow(damping, duration);

		// Clear accumulated force
		clearAccumulatedForce();
	}
}


void Particle::addForce(const Vector3D& force) {
	accumulatedForce += force;
}

void Particle::clearAccumulatedForce() {
	accumulatedForce.clear();
}

