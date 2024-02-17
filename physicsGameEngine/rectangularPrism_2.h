
#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "polyhedron_2.h"

namespace pe {

	class RectangularPrism : public Polyhedron {

	public:

		real width;
		real height;
		real depth;

		RectangularPrism(
			RigidBody* body,
			real width,
			real height,
			real depth,
			real mass,
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(mass / 12.0)* (height* height + depth * depth), 0, 0,
					0, (mass / 12.0)* (width* width + depth * depth), 0,
					0, 0, (mass / 12.0)* (width* width + height * height)
				),
				std::vector<Vector3D>{
					Vector3D(-width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, depth / 2)
				},
				std::vector<std::vector<int>>{
					std::vector<int>{ 0, 1, 2, 3 },
					std::vector<int>{ 7, 6, 5, 4 },
					std::vector<int>{ 0, 3, 7, 4 },
					std::vector<int>{ 1, 5, 6, 2 },
					std::vector<int>{ 0, 4, 5, 1 },
					std::vector<int>{ 3, 2, 6, 7 }
				},
				std::vector<std::pair<int, int>>{
					std::make_pair(0, 1),
					std::make_pair(1, 2),
					std::make_pair(2, 3),
					std::make_pair(3, 0),
					std::make_pair(4, 5),
					std::make_pair(5, 6),
					std::make_pair(6, 7),
					std::make_pair(7, 4),
					std::make_pair(0, 4),
					std::make_pair(1, 5),
					std::make_pair(2, 6),
					std::make_pair(3, 7),
				}
			),
			width{ width }, height{ height }, depth{ depth } {}
	};
}


#endif