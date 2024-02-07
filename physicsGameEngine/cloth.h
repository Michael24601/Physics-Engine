
#ifndef CLOTH_H
#define CLOTH_H

#include "particleMesh.h"
#include "particleBungeeForce.h"
#include "particleSpringForce.h"
#include "particleAngleConstraint.h"
#include "particleDistanceConstraint.h"
#include <assert.h>

namespace pe {

	class Cloth : public ParticleMesh {

	protected:

		/*
			Bungee force, where each force applies from one particle onto
			the other (two sided).
		*/
		struct BungeeForce {
			ParticleBungeeForce force1;
			ParticleBungeeForce force2;
		};

		// Distance constraints to maintain resting lengths
		std::vector<ParticleDistanceConstraint> distanceConstraints;

		// Angle constraints to maintain bending behavior
		std::vector<ParticleAngleConstraint> angleConstraints;


		/*
			Creates the forces and constraints.
			Called in the subclass when the edges and faces have been set.
		*/
		void setForces() {

			for (const MeshEdge& edge : edges) {

				Vector3D distanceVector = edge.particles.first->position -
					edge.particles.second->position;
				real distance = distanceVector.magnitude();

				// Adds the bungee force for each edge
				BungeeForce force{
					ParticleBungeeForce(edge.particles.first, ropeStrength,
						distance),
					ParticleBungeeForce(edge.particles.second, ropeStrength,
						distance),
				};
				forces.push_back(force);

				// Adds the distance constraint for each edge as well
				Particle* particle1 = edge.particles.first;
				Particle* particle2 = edge.particles.second;
				real restLength = distance;

				ParticleDistanceConstraint distanceConstraint(
					particle1,
					particle2,
					restLength
				);
				distanceConstraints.push_back(distanceConstraint);
			}
		}


		void setConstraints() {
			// Adding angle constraints for each particle triplet in a face
			for (const MeshFace& face : faces) {
				/*
					Each triplet in the facem means each angle formed by 3
					consecutive particles.
				*/
				for (int i = 0; i < 4; i++) {

					Particle* particle1 = face.particles[i];
					Particle* particle2 = face.particles[(i + 1) % 4];
					Particle* particle3 = face.particles[(i + 2) % 4];

					// Rest angle is 90 degrees as the particles are in a grid
					real restAngle = PI;

					ParticleAngleConstraint angleConstraint(
						particle1,
						particle2,
						particle3,
						restAngle,
						(real)0.1
					);

					angleConstraints.push_back(angleConstraint);
				}
			}
		}

	public:

		std::vector<BungeeForce> forces;
		real ropeStrength;

		Cloth(
			const std::vector<Vector3D>& particles,
			std::vector<real> mass,
			std::vector<real> damping,
			real ropeStrength
		) :
			ParticleMesh(
				particles,
				mass,
				damping
			),
			ropeStrength{ ropeStrength } {}

		virtual void setEdges() = 0;


		virtual void setFaces() = 0;


		/*
			Applies angle and distance constraints on the particles of the
			mesh in order to add stability to the structure by preventing
			the cloth from being too deformed.
		*/
		void applyConstraints() {
			// Apply distance constraints
			for (ParticleDistanceConstraint& constraint : distanceConstraints) {
				constraint.applyConstraint();
			}

			// Apply angle constraints
			for (ParticleAngleConstraint& constraint : angleConstraints) {
				constraint.applyConstraint();
			}
		}

	};
}

#endif