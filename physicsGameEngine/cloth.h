
#ifndef CLOTH_H
#define CLOTH_H

#include "softObject.h"
#include "particleDirectForce.h"

namespace pe {

	class Cloth: public SoftObject {

	private:

		static SoftObject generateSoftObject(
			int columnDensity, int rowDensity,
			real height, real width,
			const Vector3D& direction1,
			const Vector3D& direction2,
			const Vector3D& origin,
			real mass,
			real damping,
			real dampingCoefficient,
			real structuralStifness,
			real shearStifness,
			real bendStiffness
		);

	public:

		// Number of particles in each row and column
		std::pair<int, int> sideDensity;
		
		// The length of each row and column
		std::pair<real, real> sideLength;

		/*
			The direction vector of each row and column
			(must not be colinear).
			They must also be normalized, but the constructor normalizes
			them anyway.
		*/
		std::pair<Vector3D, Vector3D> sideDirection;

		// The origin, kind of the lower left corner
		Vector3D origin;

		// Map that quickly returns the neighbors of a particle in the grid
		std::vector<std::vector<int>> particleNeighbors;

		/*
			The rendering components.
		*/
		RenderComponent faceRenderer;
		RenderComponent edgeRenderer;

		Cloth(
			int columnDensity, int rowDensity,
			real height, real width,
			const Vector3D& sideDirection1,
			const Vector3D& sideDirection2,
			const Vector3D& origin,
			real mass,
			real damping,
			real dampingCoefficient,
			real structuralStifness,
			real shearStifness,
			real bendStiffness
		);


		/*
			Applies laplacian smoothing in order to make the cloth behave
			better and look smoother.
			Done after the spring forces and other forces have been applied
			and the particles have been integrated, but before the mesh
			vertices have been moved to match the particle grid position.
		*/
		void applyLaplacianSmoothing(int iterations, real factor);


		/*
			Applies wind force to the cloth.
			It can do that by adding forces to particles based on the normal
			vector direction of the faces the particle is part of, compared
			to the wind direction.
		*/
		void applyWindForce(const Vector3D& force, real deltaT);

	};
}

#endif