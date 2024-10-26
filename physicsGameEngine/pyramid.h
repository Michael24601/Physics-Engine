
#ifndef PYRAMID_H
#define PYRAMID_H

#include "mesh.h"

namespace pe {

	class Pyramid : public Mesh {

	private:


		static std::vector<Vector3D> generateVertices(
			real side, 
			real height
		) {
			return std::vector<Vector3D>{
				Vector3D(0, 3 * height / 4.0, 0),
					Vector3D(-side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, side / 2),
					Vector3D(-side / 2, -height / 4.0, side / 2),
			};
		}


		static std::vector<std::pair<int, int>> generateEdges() {

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

			return edges;
		}


		// All vertices are in counter-clockwise order
		static std::vector<std::vector<int>> generateFaces() {

			std::vector<std::vector<int>> faces{
				std::vector<int>{ 1, 2, 3, 4 },
				std::vector<int>{0, 1, 4},
				std::vector<int>{0, 4, 3},
				std::vector<int>{0, 3, 2},
				std::vector<int>{0, 2, 1}
			};

			return faces;
		}


	public:

		const real side;
		const real height;

		Pyramid(real side, real height) :
			side{ side }, height{ height },
			Mesh(
				generateVertices(side, height),
				generateFaces(), 
				generateEdges()
			){}

	};
}

#endif

