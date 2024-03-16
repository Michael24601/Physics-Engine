/*
	Note that integrartion of particles, and the choice of integrator,
	is not done inside of this class, but outside by the user of the class
	and its children.
*/

#ifndef PARTICLE_MESH_H
#define PARTICLE_MESH_H

#include "particle.h"
#include "curvedFace.h"
#include "particleBungeeForce.h"
#include "particleDistanceConstraint.h"
#include "particleAngleConstraint.h"
#include "edge.h"
#include <vector>

namespace pe {

	class ParticleMesh {

	protected:

		/*
			Vertices corresponding to the location of the particles.
			The local ones correspond to the position of the particles
			at the creation of the cloth.
		*/
		std::vector<Vector3D> localVertices;
		std::vector<Vector3D> globalVertices;

		// Distance constraints to maintain resting lengths
		std::vector<ParticleDistanceConstraint> distanceConstraints;

		// Angle constraints to maintain bending behavior
		std::vector<ParticleAngleConstraint> angleConstraints;

		/*
			The egdes store the assoications between edges in a certain
			way that can be useful if we want unique assoications.
			But sometimes we need to have access to all particles
			connected to another. 
			For that we have this, where edgeAssociations[i] contains
			all associated particle indexes to particle[i].
		*/
		std::vector<std::vector<int>> edgeAssociations;


		std::vector<std::vector<int>> getEdgeAssociations() const {
			std::vector<std::vector<int>> associations(particles.size());
			for (Edge* edge : edges) {
				associations[edge->getIndex(0)].push_back(edge->getIndex(1));
				associations[edge->getIndex(1)].push_back(edge->getIndex(0));
			}
			return associations;
		}


	public:

		/*
			Bungee force, where each force applies from one particle onto
			the other (two sided).
		*/
		struct BungeeForce {
			ParticleBungeeForce force1;
			ParticleBungeeForce force2;
		};

		std::vector<BungeeForce> forces;
		real ropeStrength;

		/*
			Creates the forces and constraints.
			Called in the subclass when the edges and faces have been set.
		*/
		void setForces() {

			for (Edge* edge: edges) {
				Vector3D distanceVector = localVertices[edge->getIndex(0)] - 
					localVertices[edge->getIndex(1)];
				real distance = distanceVector.magnitude();

				// Adds the bungee force for each edge twice (one from each particle)
				BungeeForce force{
					ParticleBungeeForce(
						&particles[edge->getIndex(1)],
						ropeStrength,
						distance
					),
					ParticleBungeeForce(
						&particles[edge->getIndex(0)],
						ropeStrength,
						distance
					),
				};
				forces.push_back(force);
			}
		}


		void setConstraints() {

			for (Edge* edge : edges) {
				Vector3D distanceVector = localVertices[edge->getIndex(0)] -
					localVertices[edge->getIndex(1)];
				real restLength = distanceVector.magnitude();

				// Adds the distance constraint for each edge as well
				Particle* particle1 = &particles[edge->getIndex(0)];
				Particle* particle2 = &particles[edge->getIndex(1)];

				ParticleDistanceConstraint distanceConstraint(
					particle1,
					particle2,
					restLength
				);
				distanceConstraints.push_back(distanceConstraint);
			}

			// Adding angle constraints for each particle triplet in a face
			for (CurvedFace* face : faces) {
				/*
					Each triplet in the facem means each angle formed by 3
					consecutive particles.
				*/
				for (int i = 0; i < 4; i++) {

					Particle* particle1 = &particles[face->getIndex(i)];
					Particle* particle2 = &particles[face->getIndex((i + 1) % 4)];
					Particle* particle3 = &particles[face->getIndex((i + 2) % 4)];

					// Rest angle is 90 degrees as the particles are in a grid
					real restAngle = PI;

					ParticleAngleConstraint angleConstraint(
						particle1,
						particle2,
						particle3,
						restAngle,
						(real)1.0
					);

					angleConstraints.push_back(angleConstraint);
				}
			}
		}


		Vector3D calculateMeshVertexNormal(
			const std::vector<Particle>& particles,
			const std::vector<std::vector<int>>& connections,
			int particleIndex
		) const {

			Vector3D particlePosition = particles[particleIndex].position;
			Vector3D normal;

			// For each beighboring vector
			for (int i = 0; i < connections[particleIndex].size(); i++) {

				// First we retrieve the position of the neighbor
				int neighborIndex = connections[particleIndex][i];
				Vector3D neighborPosition = particles[neighborIndex].position;

				/*
					Then, we retrieve this vector, which is the position of
					the neighbor in comparison to this particle's position.
					Since particles move, we use the original local 
					coordinates, which tell us where teh neighbor is supposed
					to be.
					We then rotate it by 90 degrees and normalize it.
				*/
				Vector3D neighborVector = localVertices[neighborIndex] - 
					localVertices[particleIndex];
				neighborVector.rotate90Degrees();
				neighborVector.normalize();

				// We then add to the normal this cross product
				normal += (neighborPosition - particlePosition).vectorProduct(
					neighborVector
				);
			}

			// Normalize the average vector to get the normal
			return normal.normalized();
		}


		std::vector<Vector3D> calculateMeshNormals() const {
			std::vector<Vector3D> normals;
			for (int i = 0; i < particles.size(); i++) {
				normals.push_back(
					calculateMeshVertexNormal(
						particles,
						edgeAssociations,
						i
					)
				);
			}
			return normals;
		}


	public:
		
		std::vector<Particle> particles;

		// The faces and edges needs to draw the particle mesh
		std::vector<CurvedFace*> faces;
		std::vector<Edge*> edges;

		/*
			If the mesh needs to be connected with forces like spring
			forces, or cables, or rods, this needs to be initialized
			in the constructor of the class extending this one.
		*/
		ParticleMesh(
			std::vector<Vector3D> particlePositions,
			std::vector<real> mass,
			std::vector<real> damping,
			real ropeStrength
		) : ropeStrength{ ropeStrength } {

			localVertices = particlePositions;
			/*
				Initially, the global vertices were the same as the
				original local ones.
			*/
			globalVertices = particlePositions;

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


			/*
				Because it is unsafe to set the edges and faces of the mesh
				in the constructor using pure virtual functions not yet
				defined, we expect the constructors of the classes extending
				ParticleMesh to call setEdges and setFaces after implimenting
				them.
			*/
		}

		/*
			Sets the edge connections of the particles in the mesh.
			Needs to be overriden in such a way as to return an array of
			Edge objects based on which particles the edges connect in the
			mesh.

			Extending this function in the subclass makes more sense than
			having the parent class expect a vector of edges after testing.
		*/
		virtual void setEdges() = 0;


		/*
			Sets the face associations.
			Everything said about the edges applies here.
		*/
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


		/*
			Updates the mesh (after integration), updating the face data
		*/
		void update() {

			/*
				First the global vertices need to be updated to be the same
				as the particle's as there is not transform matrix.
			*/
			for (int i = 0; i < particles.size(); i++) {
				globalVertices[i] = particles[i].position;
			}

			/*
				The normals can be calculated as such.
			*/
			std::vector<Vector3D> normals = calculateMeshNormals();

			for (CurvedFace* face : faces) {

				/*
					Since we don't have a transform matrix, we recalculate the
					values using the formulas. The normals have to be set from
					the outside, and the rest the face can calculate.
				*/
				for (int i = 0; i < face->getVertexNumber(); i++) {
					face->setNormal(i, normals[face->getIndex(i)]);
				}

				face->update();
			}
		}


	};
}


#endif