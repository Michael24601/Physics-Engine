/*
	Note that integrartion of particles, and the choice of integrator,
	is not done inside of this class, but outside by the user of the class
	and its children.
*/

#ifndef PARTICLE_MESH_H
#define PARTICLE_MESH_H

#include "particle.h"
#include <vector>

namespace pe {

	/*
		A struct representing a face.
		We use pointers to the real particles in the mesh to associate
		particles in the face.
		Note that the particles must be defined in counter-clockwise order.
	*/
	struct MeshFace {
		std::vector<Particle*> particles;

		MeshFace() {}

		MeshFace(const std::vector<Particle*>& particles) : 
			particles(particles) {}


		// Returns the positions in clockwise or counter-clockwise order
		std::vector<Vector3D> getVertices(order order) const {
			std::vector<Vector3D> positions;
			if (order == order::COUNTER_CLOCKWISE) {
				for (const Particle* particle : particles) {
					positions.push_back(particle->position);
				}
			}
			else if (order == order::CLOCKWISE) {
				for (auto it = particles.rbegin(); it != particles.rend(); 
					++it) {
					positions.push_back((*it)->position);
				}
			}
			return positions;
		}


		// This function assumes we have at least 3 vertices
		Vector3D getNormal(order order) const {
			Vector3D firstVertex = particles[0]->position;
			Vector3D secondVertex = particles[1]->position;
			Vector3D thirdVertex = particles[2]->position;

			Vector3D AB = secondVertex - firstVertex;
			Vector3D AC = thirdVertex - firstVertex;
			Vector3D normal = AB.vectorProduct(AC);
			normal.normalize();
			// If clockwise, we reverse it
			if (order == order::CLOCKWISE) {
				normal *= -1;
			}
			return normal;
		}


		Vector3D getCentroid() const {
			Vector3D sum;
			// Calculates the sum of vertex positions
			for (const Particle* particle : particles) {
				sum += particle->position;
			}
			// Calculates the centroid as the average of vertex positions
			return sum * (1.0f / static_cast<real>(particles.size()));
		}
	};


	/*
		A struct representing the edge of a polyhedron.
	*/
	struct MeshEdge {
		std::pair<Particle*, Particle*> particles;

		MeshEdge() {}

		MeshEdge(Particle* first, Particle* second) {
			particles = std::make_pair(first, second);
		}
	};


	class ParticleMesh {
	public:
		
		std::vector<Particle> particles;

		// The faces and edges needs to draw the particle mesh
		std::vector<MeshFace> faces;
		std::vector<MeshEdge> edges;

		/*
			If the mesh needs to be connected with forces like spring
			forces, or cables, or rods, this needs to be initialized
			in the constructor of the class extending this one.
		*/
		ParticleMesh(
			std::vector<Vector3D> particlePositions,
			std::vector<real> mass,
			std::vector<real> damping
		) {
			/*
				Positions(in global coordinates, as we are working with
				particles, and there are no transformations.
			*/
			particles.resize(particlePositions.size());
			for (int i = 0; i < particles.size(); i++) {
				particles[i].position = particlePositions[i];
				particles[i].setMass(mass[i]);
				particles[i].damping = damping[i];
			}
		}

		/*
			Sets the edge connections of the particles in the mesh.
			Needs to be overriden in such a way as to return an array of
			Edge objects based on which particles the edges connect in the
			mesh.
		*/
		virtual void setEdges() = 0;


		/*
			Sets the face associations.
			Everything said about the edges applies here.
		*/
		virtual void setFaces() = 0;
	};
}


#endif