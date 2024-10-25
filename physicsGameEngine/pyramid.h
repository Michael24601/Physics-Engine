

#ifndef PYRAMID_H
#define PYRAMID_H

#include "rigidObject.h"
#include "orientedBoundingBox.h"

namespace pe {

	class Pyramid : public RigidObject<OrientedBoundingBox> {

	private:

		// Side length
		real side;
		real height;

	public:

		static Mesh* generateMesh(real side, real height) {

			std::vector<Vector3D> vertices {
				Vector3D(0, 3 * height / 4.0, 0),
				Vector3D(-side / 2, -height / 4.0, -side / 2),
				Vector3D(side / 2, -height / 4.0, -side / 2),
				Vector3D(side / 2, -height / 4.0, side / 2),
				Vector3D(-side / 2, -height / 4.0, side / 2),
			};

			std::vector<std::pair<int, int>> edges{
				std::make_pair(1, 2),
				std::make_pair(2, 3),
				std::make_pair(3, 4),
				std::make_pair(4, 1),
				std::make_pair(0, 1),
				std::make_pair(0, 2),
				std::make_pair(0, 3),
				std::make_pair(0, 4)
			};

			std::vector<std::vector<int>> faces{
				std::vector<int>{ 1, 2, 3, 4 },
				std::vector<int>{0, 1, 4},
				std::vector<int>{0, 4, 3},
				std::vector<int>{0, 3, 2},
				std::vector<int>{0, 2, 1}
			};

			Mesh* mesh = new Mesh(vertices, faces, edges);

			return mesh;
		}


		Pyramid(
			real side, real height,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : RigidObject<OrientedBoundingBox>(
			generateMesh(side, height),
			new Renderer(mesh, GL_STATIC_DRAW, false),
			/*
				The centroid of the pyramid is one fourth of the way
				up from the base. The bounding box is centred halfway
				between the base and apex of the pyramid, so we have
				to shift it by a quarter of the height.
				The halfsize of the bounding box is half the side,
				half the height, then half a side.
			*/
			new OrientedBoundingBox(
				Vector3D(side/2.0, height/2.0, side/2.0), 
				Vector3D(0, height/4.0, 0)
			),
			position,
			orientation,
			mass,
			Matrix3x3(
				(mass / 10.0)* (3 * side * side + height * height), 0, 0,
				0, (mass / 10.0)* (3 * side * side + height * height), 0,
				0, 0, (mass / 5.0)* side* side
			)
		), side{ side }, height{ height} {}


		~Pyramid() {
			delete mesh;
			delete renderer;
			delete boundingVolume;
		}

	};
}

#endif

