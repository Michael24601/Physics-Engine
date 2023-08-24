
#include "rigidBodyGravity.h"

using namespace pe;

void RigidBodyGravity::updateForce(RigidBody* body, real duration) {
	if (body->hasFiniteMass()) {
		body->addForce(gravity * body->getMass());
	}
}