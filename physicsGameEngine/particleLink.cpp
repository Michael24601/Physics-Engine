// Source file for the particle link class

#include "particleLink.h"

using namespace pe;

real ParticleLink::currentLength() const {
	/*
		The current length is the magnitude of the vector from the first
		object to the second.
	*/
	Vector3D vector = particle[0]->position - particle[1]->position;
	return vector.magnitude();
}