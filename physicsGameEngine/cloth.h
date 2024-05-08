
#ifndef CLOTH_H
#define CLOTH_H

#include "mesh.h"
#include "particleBungeeForce.h"
#include "particleSpringForce.h"
#include "particleSpringDamper.h"
#include "particleDistanceConstraint.h"
#include <vector>

namespace pe {

	class Cloth: public Mesh {

	private:

		// Distance constraints to maintain resting lengths
		std::vector<ParticleDistanceConstraint> distanceConstraints;

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
		std::vector<Edge*> calculateEdges();


		std::vector<CurvedFace*> calculateFaces();


		/*
			Creates the forces and constraints.
			Called in the subclass when the edges and faces have been set.
		*/
		void setForces();


		void setConstraints();


		std::vector<std::vector<Vector3D>> calculateMeshNormals() override;


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

	};
}

#endif