/*
	Note that integrartion of particles, and the choice of integrator,
	is not done inside of this class, but outside by the user of the class
	and its children.
*/

#ifndef CLOTH_H
#define CLOTH_H

#include "particle.h"
#include "curvedFace.h"
#include "particleBungeeForce.h"
#include "particleSpringForce.h"
#include "particleSpringDamper.h"
#include "particleDistanceConstraint.h"
#include "particleAngleConstraint.h"
#include "edge.h"
#include <vector>

namespace pe {

	class Cloth {

	private:

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
			Returns a grid of particles between the top left and bottom
			right corners, such as there are the specified number of
			particles per column and row.
		*/
		static std::vector<Vector3D> returnParticleGrid(
			int columnSize,
			int rowSize,
			Vector3D topLeft,
			Vector3D bottomRight
		);


		// The egdes just correspond to the associations
		void setEdges();


		void setFaces();


		/*
			Creates the forces and constraints.
			Called in the subclass when the edges and faces have been set.
		*/
		void setForces();


		void setConstraints();


		void calculateMeshNormals();


	public:

		/*
			Number of particles in the columns and rows.
			Their product will be the total number of particles.
		*/
		int columnSize;
		int rowSize;

		/*
			Bungee force, where each force applies from one particle onto
			the other (two sided).
		*/
		struct SpringForce {
			ParticleSpringDamper force1;
			ParticleSpringDamper force2;
		};

		std::vector<SpringForce> forces;
		real ropeStrength;
		
		std::vector<Particle> particles;

		// The faces and edges needs to draw the particle mesh
		std::vector<CurvedFace*> faces;
		std::vector<Edge*> edges;

		/*
			If the mesh needs to be connected with forces like spring
			forces, or cables, or rods, this needs to be initialized
			in the constructor of the class extending this one.
		*/
		Cloth(
			Vector3D topLeft,
			Vector3D bottomRight,
			int rowSize,
			int columnSize,
			real mass,
			real damping,
			real ropeStrength
		);


		/*
			Applies angle and distance constraints on the particles of the
			mesh in order to add stability to the structure by preventing
			the cloth from being too deformed.
		*/
		void applyConstraints();


		/*
			Updates the mesh (after integration), updating the face data
		*/
		void update();

	};
}


#endif