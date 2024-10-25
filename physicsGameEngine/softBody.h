
#ifndef SOFT_BODY_H
#define SOFT_BODY_H

#include "particleSpringDamper.h"

namespace pe {

	class SoftBody {

	public:

		std::vector<Particle> particles;
		std::vector<ParticleSpringDamper> springs;
		std::vector<std::pair<int, int>> springPairs;

		SoftBody(
			const std::vector<Vector3D>& particleCoordinates,
			real mass,
			real damping,
			const std::vector<std::pair<int, int>>& springPairs,
			const std::vector<real> springStrengths
		) : springPairs{springPairs} {

			if (springPairs.size() != springStrengths.size()) {
				throw std::invalid_argument(
					"Spring pairs and strengths must have the same size"
				);
			}

			particles.resize(particleCoordinates.size());
			for (int i = 0; i < particles.size(); i++) {
				particles[i].setMass(mass);
				particles[i].position = particleCoordinates[i];
				particles[i].damping = damping;
			}

			springs.resize(springPairs.size());
			for (int i = 0; i < springs.size(); i++) {
				springs[i] = ParticleSpringDamper(
					&particles[springPairs[i].second],
					springStrengths[i],
					1.0,
					(particles[springPairs[i].second].position - 
					particles[springPairs[i].first].position).magnitude()
				);
			}
		}


		/*
			Applies a force on all of the particles in the body, such as
			gravity for instance.
		*/
		void applyForce(const ParticleForceGenerator& force, real duration) {
			for (int i = 0; i < particles.size(); i++) {
				force.updateForce(&particles[i], duration);
			}
		}


		// Applies all the spring forces
		void applySpringForces(real duration) {
			for (int i = 0; i < springPairs.size(); i++) {
				springs[i].updateForce(&particles[springPairs[i].first], duration);
			}
		}


		// Integrates all of the particles
		void integrate(real duration) {
			for (int i = 0; i < particles.size(); i++) {
				particles[i].verletIntegrate(duration);
			}
		}

	};

}

#endif