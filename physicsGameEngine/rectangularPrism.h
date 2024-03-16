
#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "polyhedron.h"

namespace pe {

	class RectangularPrism : public Polyhedron {

	private:

		static std::vector<Edge*> generateEdges(
			std::vector<Vector3D>& localVertices,
			std::vector<Vector3D>& globalVertices
		) {
			std::vector<Edge*> edges(12);

			// Define the edges of the pyramid
			edges[0] = new Edge(&localVertices, &globalVertices, 0, 1);
			edges[1] = new Edge(&localVertices, &globalVertices, 1, 2);
			edges[2] = new Edge(&localVertices, &globalVertices, 2, 3);
			edges[3] = new Edge(&localVertices, &globalVertices, 3, 0);
			edges[4] = new Edge(&localVertices, &globalVertices, 4, 5);
			edges[5] = new Edge(&localVertices, &globalVertices, 5, 6);
			edges[6] = new Edge(&localVertices, &globalVertices, 6, 7);
			edges[7] = new Edge(&localVertices, &globalVertices, 7, 4);
			edges[8] = new Edge(&localVertices, &globalVertices, 0, 4);
			edges[9] = new Edge(&localVertices, &globalVertices, 1, 5);
			edges[10] = new Edge(&localVertices, &globalVertices, 2, 6);
			edges[11] = new Edge(&localVertices, &globalVertices, 3, 7);

			//std::cout << "a\n";

			return edges;
		}


		// All vertices are in clockwise order
		static std::vector<Face*> generateFaces(
			std::vector<Vector3D>& localVertices,
			std::vector<Vector3D>& globalVertices
		) {
			std::vector<Face*> faces(6);

			std::vector<std::vector<int>> indexes{
				std::vector<int>{ 0, 1, 2, 3 },
				std::vector<int>{ 7, 6, 5, 4 },
				std::vector<int>{ 0, 3, 7, 4 },
				std::vector<int>{ 1, 5, 6, 2 },
				std::vector<int>{ 0, 4, 5, 1 },
				std::vector<int>{ 3, 2, 6, 7 }
			};

			for (int i = 0; i < indexes.size(); i++) {
				faces[i] = new Face(
					&localVertices,
					&globalVertices,
					indexes[i]
				);
			}

			return faces;
		}

	public:

		real width;
		real height;
		real depth;

		RectangularPrism(
			real width,
			real height,
			real depth,
			real mass,
			Vector3D position,
			RigidBody* body) :
			Polyhedron(
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
				body
			),
			width{ width }, height{ height }, depth{ depth }{

			setFaces(generateFaces(localVertices, globalVertices));
			setEdges(generateEdges(localVertices, globalVertices));

			setUVCoordinates();
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
			for (Face* face : faces) {
				face->setTextureCoordinates(textureCoordinates);
			}
		}
	};
}


#endif