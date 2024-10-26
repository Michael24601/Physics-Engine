
#ifndef CUBOID_H
#define CUBOID_H

#include "mesh.h"

namespace pe {

	class Cuboid : public Mesh {

	private:

		static std::vector<Vector3D> generateVertices(
			real width, 
			real height, 
			real depth
		) {
			return std::vector<Vector3D>{
				Vector3D(-width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, depth / 2)
			};
		}


		static std::vector<std::pair<int, int>> generateEdges() {

			std::vector<std::pair<int, int>> edges{
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
				 std::make_pair(3, 7)
			};

			return edges;
		}


		// All vertices are in counter-clockwise order
		static std::vector<std::vector<int>> generateFaces() {

			std::vector<std::vector<int>> faces{
				std::vector<int>{ 0, 1, 2, 3 },
				std::vector<int>{ 7, 6, 5, 4 },
				std::vector<int>{ 0, 3, 7, 4 },
				std::vector<int>{ 1, 5, 6, 2 },
				std::vector<int>{ 0, 4, 5, 1 },
				std::vector<int>{ 3, 2, 6, 7 }
			};

			return faces;
		}


		/*
			Since we know how a rectangular prism looks, and the kinds of
			faces it has, we can set each face's uv coordinates this way.
		*/
		void setUVCoordinates(real cornerX = 1, real cornerY = 1) {
			std::vector<Vector2D> textureCoordinates{
				Vector2D(0, 0),
				Vector2D(0, cornerY),
				Vector2D(cornerX, cornerY),
				Vector2D(cornerX, 0)
			};
			for (int i = 0; i < getFaceCount(); i++) {
				setFaceTextureCoordinates(i, textureCoordinates);
			}
		}


	public:

		const real width;
		const real height;
		const real depth;

		Cuboid(real width, real height, real depth) :
			width{ width }, height{ height }, depth{ depth },
			Mesh(
				generateVertices(width, height, depth), 
				generateFaces(), 
				generateEdges()
			) {

			setUVCoordinates();
		}

	};
}


#endif
