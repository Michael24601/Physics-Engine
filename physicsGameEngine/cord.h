
#ifndef CORD_H
#define CORD_H

#include "particleMesh.h"
#include "particleBungeeForce.h"
#include <assert.h>

namespace pe {

	class Cord : public ParticleMesh {

	private:

		/*
			Returns a segment of particles between two coordinates
			(connected in a straight line). There must be a least two
			particles.
		*/
		std::vector<Vector3D> returnParticleSegment(
			int particleNumber,
			Vector3D firstCoordinate,
			Vector3D bottomCoordinate
		) {
			std::vector<Vector3D> positions;
			
			// The direction vector between the two coordinates
			Vector3D direction = bottomCoordinate - firstCoordinate;
			real stepSize = direction.magnitude() / (particleNumber-1);

			// Normalize the direction vector
			direction.normalize();
			for (int i = 0; i < particleNumber; i++) {
				Vector3D currentPosition = firstCoordinate + direction * (i * stepSize);
				positions.push_back(currentPosition);
			}

			return positions;
		}

		/*
			Bungee force, where each force applies from one particle onto
			the other (two sided).
		*/
		struct BungeeForce {
			ParticleBungeeForce force1;
			ParticleBungeeForce force2;
		};

	public:

		/*
			Number of particles in the cord.
		*/
		int particleNumber;
		
		// Cables connecting the particles
		std::vector<BungeeForce> forces;

		real totalRestingLength;

		Cord(
			// The mass and damping are the same for all particles here
			real mass,
			real damping,
			real strength,
			int particleNumber,
			Vector3D firstCoordinate,
			Vector3D bottomCoordinate
		) :
			ParticleMesh(
				returnParticleSegment(
					particleNumber,
					firstCoordinate,
					bottomCoordinate
				),
				// Number of particles is row - 1 is the number of edges
				std::vector<real>(particleNumber, mass),
				std::vector<real>(particleNumber, damping)
			),
			particleNumber{ particleNumber } {

			setEdges();
			setFaces();

			Vector3D direction = bottomCoordinate - firstCoordinate;
			real distance = direction.magnitude() / (particleNumber - 1);
			for (const MeshEdge& edge : edges) {
				// Creates a spring for each edge
				BungeeForce force{
					ParticleBungeeForce(edge.particles.second, strength, distance),
					ParticleBungeeForce(edge.particles.first, strength, distance)
				};
				forces.push_back(force);
			}

			totalRestingLength = distance * (particleNumber - 1);
		}

		virtual void setEdges() {
			for (int i = 0; i < particleNumber - 1; i++) {
				edges.push_back(MeshEdge(&particles[i],
					&particles[i+1]));
			}
		}


		virtual void setFaces() {
			// Since it's a cord, there are no faces.
		}
	};
}

#endif
