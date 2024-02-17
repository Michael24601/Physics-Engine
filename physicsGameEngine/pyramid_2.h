

#ifndef PYRAMID_H
#define PYRAMID_H

#include "polyhedron_2.h"

namespace pe {

	class Pyramid : public Polyhedron {

	public:

		// Side length
		real side;
		real height;

		Pyramid(
			RigidBody* body, 
			real side, 
			real height, 
			real mass,
			Vector3D position) : 
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(mass / 10.0)* (3 * side * side + height * height), 0, 0,
					0, (mass / 10.0)* (3 * side * side + height * height), 0,
					0, 0, (mass / 5.0)* side* side
				),
				std::vector<Vector3D>{
					Vector3D(0, 3 * height / 4.0, 0),
					Vector3D(-side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, side / 2),
					Vector3D(-side / 2, -height / 4.0, side / 2),
				},
				std::vector<std::vector<int>>{
					std::vector<int>{1, 2, 3, 4},
					std::vector<int>{0, 1, 4},
					std::vector<int>{0, 4, 3},
					std::vector<int>{0, 3, 2},
					std::vector<int>{0, 2, 1}
				},
				std::vector<std::pair<int, int>>{
					std::make_pair(1, 2),
					std::make_pair(2, 3),
					std::make_pair(3, 4),
					std::make_pair(4, 1),
					std::make_pair(0, 1),
					std::make_pair(0, 2),
					std::make_pair(0, 3),
					std::make_pair(0, 4)
				}
			),
			side{ side }, height{ height } {}

	};
}

#endif

