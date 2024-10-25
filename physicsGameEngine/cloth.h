
#ifndef CLOTH_H
#define CLOTH_H

#include "softObject.h"
#include "particleDistanceConstraint.h"

namespace pe {

	class Cloth: public SoftObject {

	private:


		static SoftObject generateSoftObject(
			const std::pair<int, int>& sideDensity,
			const std::pair<real, real>& sideLength,
			const std::pair<Vector3D, Vector3D>& sideDirection,
			const Vector3D& origin,
			real mass,
			real damping,
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
			They must also be normalized, but the
			constructor normalizes them anyway.
		*/
		std::pair<Vector3D, Vector3D> sideDirection;

		// The origin, kind of the lower left corner
		Vector3D origin;

		// Map that quickly returns the neighbors of a particle in the grid
		std::vector<std::vector<int>> particleNeighbors;

		Cloth(
			const std::pair<int, int>& sideDensity,
			const std::pair<real, real>& sideLength,
			const std::pair<Vector3D, Vector3D>& sideDirection,
			const Vector3D& origin,
			real mass,
			real damping,
			real structuralStifness,
			real shearStifness,
			real bendStiffness
		);


		void applyLaplacianSmoothing(int iterations, real factor);

	};
}

#endif