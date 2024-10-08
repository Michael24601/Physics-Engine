/*
	Header file for the general polyhedron class.
*/

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "vector2D.h"
#include "rigidBody.h"
#include "BVHSphere.h"
#include <vector>
#include <algorithm>

#include "face.h"
#include "curvedFace.h"
#include "edge.h"

namespace pe {

	class Polyhedron {

	public:

		/*
			Checks if point is inside a face.
		*/
		static bool isPointInsideFace(
			const Face* face,
			const Vector3D& point,
			real rayLength
		);

		/*
			The vertices, faces, edges, bounding volumes... are all in
			local (relative) coordinates.
		*/
		std::vector<Vector3D> localVertices;
		RigidBody* body;

		std::vector<Face*> faces;
		std::vector<Edge*> edges;

		Polyhedron(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		);

		
		~Polyhedron();
		

		/*
			In most well known polyhedrons, such as rectangular prisms,
			spheres, and pyramids, the centre of gravity is well known,
			and can be set explicitely using the dimensions of the shape,
			if it is known.
			However, if it is not know (if the shape is not well known),
			we can use this static function to set the centre of gravity
			of the shape (around which, the rotations and relative positions
			are set). This is distinct from the geometric centre, which is
			the centre of the box that contains the shape.
		*/
		static Vector3D calculateCentreOfGravity(
			const std::vector<Vector3D>& vertices
		);


		/*
			One design issue is wether we create setters for the face
			and edge associations, or we create pure virtual functions
			that need to be implemented in subclasses which force the
			edges and faces to be set.
			We go with the former approach as it provides the flexibility
			of instantiating a polyhedron object without having to have
			a suitable subclass.
		*/
		void setFaces(std::vector<Face*> faces);

		/*
			We can usually get the edges from the faces, without having
			to define them explicitely; however this creates duplicated
			edges, which slow the engine down, both in the graphics
			module and the collision detection system.
			So for simple shapes, where we can define unique edges,
			we use this function.
		*/
		void setEdges(std::vector<Edge*> edges);


		Vector3D getAxis(int index) const;


		Vector3D getCentre() const;


		Vector3D getFaceNormal(int index) const;


		const Matrix3x4& getTransformMatrix() const;

		/*
			Note that because local coordinates are stored and used in the
			Polyhedron class, the given point is assumed to be global
			and is transformed into a local point.
		*/
		bool isPointInsidePolyhedron(
			const Vector3D& point
		) const ;


	};
}

#endif