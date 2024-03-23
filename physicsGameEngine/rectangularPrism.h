
#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "polyhedron.h"
#include "breakable.h"
#include "util.h"

namespace pe {

	class RectangularPrism : public Polyhedron, public Breakable {

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


		void breakObject(
			std::vector<Polyhedron*>& polyhedra,
			const Vector3D& contactNormal,
			real deltaT
		) {

			// Point around which the object will break
			Vector3D point(width / 10.0, -height / 8.0, depth / 12.0);

			/*
				This will divide the prism into 8 other polyhedrons, each of
				which will have a coner that is the breakage point.
			*/
			for (int i = 0; i < 8; i++) {
				/*
					Of the 8 rigid bodies, each will have with width to left
					of the point, or to its right, the height above the
					point, or below it, the depth in front of the point,
					or behind it; which in turn gives us 2^3 = 8
					configurations.
				*/
				real width (i % 2 == 0 ? this->width / 2 + point.x : this->width / 2 - point.x);
				real height = (i % 4 <= 1 ? this->height / 2 + point.y : this->height / 2 - point.y);
				real depth = (i % 8 <= 3 ? this->depth / 2 + point.z : this->depth / 2 - point.z);

				/*
					The offset is the position of this new polyhedron relative
					to the centre of gravity of this polyhedron (which is at
					the oririn (0, 0, 0) in local coordinates.
					Depending on which sides of the point we took depth, width
					and height-wise, we can shift the centre half of those
					dimensions either in the positive or negative direction.
				*/
				Vector3D offset;
				offset.x = (i % 2 == 0 ? width / 2 : -width / 2);
				offset.y = (i % 4 <= 1 ? height / 2 : -height / 2);
				offset.z = (i % 8 <= 3 ? depth / 2 : -depth / 2);

				// Mass is proportional to volume
				real mass = (width * height * depth * this->body->getMass()) / 
					(this->width * this->height * this->depth);

				Vector3D position = this->body->transformMatrix.transform(offset);

				RectangularPrism* prism = new RectangularPrism(
					width, height, depth, mass, position, new RigidBody()
				);

				// We can add for each fracture a force in its direction
				Vector3D force = offset;
				force += contactNormal * offset.magnitude();
				prism->body->addForce(force * (1.0/deltaT));

				prism->body->orientation = this->body->orientation;
				prism->body->linearVelocity = this->body->linearVelocity;
				prism->body->angularVelocity = this->body->angularVelocity;
				prism->body->linearDamping = this->body->linearDamping;
				prism->body->angularDamping = this->body->angularDamping;
				prism->body->forceAccumulator = this->body->forceAccumulator;
				prism->body->torqueAccumulator = this->body->torqueAccumulator;

				polyhedra.push_back(prism);
			}
		}

	};
}


#endif