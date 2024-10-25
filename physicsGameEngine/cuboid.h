
#ifndef CUBOID_H
#define CUBOID_H

#include "rigidObject.h"
#include "orientedBoundingBox.h"

namespace pe {

	class Cuboid: public RigidObject<OrientedBoundingBox> {

	private:

		real width;
		real height;
		real depth;

	public:

		static Mesh* generateMesh(
			real width, real height, real depth
		) {

			std::vector<Vector3D> vertices{
				Vector3D(-width / 2, -height / 2, -depth / 2),
				Vector3D(width / 2, -height / 2, -depth / 2),
				Vector3D(width / 2, -height / 2, depth / 2),
				Vector3D(-width / 2, -height / 2, depth / 2),
				Vector3D(-width / 2, height / 2, -depth / 2),
				Vector3D(width / 2, height / 2, -depth / 2),
				Vector3D(width / 2, height / 2, depth / 2),
				Vector3D(-width / 2, height / 2, depth / 2)
			};

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

			std::vector<std::vector<int>> faces{
				std::vector<int>{ 0, 1, 2, 3 },
				std::vector<int>{ 7, 6, 5, 4 },
				std::vector<int>{ 0, 3, 7, 4 },
				std::vector<int>{ 1, 5, 6, 2 },
				std::vector<int>{ 0, 4, 5, 1 },
				std::vector<int>{ 3, 2, 6, 7 }
			};

			Mesh* mesh = new Mesh(vertices, faces, edges);

			std::vector<Vector2D> textureCoordinates{
				Vector2D(0, 0),
				Vector2D(0, 1),
				Vector2D(1, 1),
				Vector2D(1, 0)
			};

			for (int i = 0; i < mesh->getFaceCount(); i++) {
				mesh->setFaceTextureCoordinates(i, textureCoordinates);
			}

			return mesh;
		}


		Cuboid(
			real width, real height, real depth,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : RigidObject<OrientedBoundingBox>(
			generateMesh(width, height, depth),
			new Renderer(mesh, GL_STATIC_DRAW, false),
			new OrientedBoundingBox(Vector3D(width/2.0, height/2.0, depth/2.0)),
			position,
			orientation,
			mass,
			Matrix3x3(
				(mass / 12.0)* (height* height + depth * depth), 0, 0,
				0, (mass / 12.0)* (width* width + depth * depth), 0,
				0, 0, (mass / 12.0)* (width* width + height * height)
			)
		), width{ width }, height{ height }, depth{depth} {}


		~Cuboid() {
			delete mesh;
			delete renderer;
			delete boundingVolume;
		}

	};
}


#endif