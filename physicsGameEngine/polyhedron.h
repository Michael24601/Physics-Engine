/*
	Header file for the general polyhedron class.
*/

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>
#include <eigen/dense>

#include "face.h"
#include "curvedFace.h"
#include "edge.h"

namespace pe {

	class Polyhedron {

	public:

		/*
			For any shape, there will be a smallest collision box that can
			contain the entire shape and is aligned with the coordinate system
			axes. This shape may not be centred at the centre of gravity however
			(the geometric centre may not be the centre of gravity,
			which will be closer to areas of the shape with more vertices.
			This is why we need an offset.
			We could instead find the maximum x, y, z, of any vertices,
			and use that as the halfsize from the centre of gravity,
			but then it wouldn't be the smallest collision box anymore.
			(the centre of gravity in a pyramid for example, is closer to
			the base; but the collision box must contain all vertices;
			so if the halfsize were to be calculated from the centre of
			gravity, the box would not be minimal. There would be a large
			gap below the base of the pyramid).

			This type of bounding box is called an AABB, and because it is
			axis aligned, it needs to be recalculated from the global
			vertices each frame, since it can't be rotated by the transform
			matrix lest it no longer is axis aligned.

			So this function expects the global coordinates if we need the
			AABB to be relevant to the object this frame.
		*/
		static void calculateAxisAlignedBoundingBox(
			const std::vector<Vector3D>& vertices,
			Vector3D& halfsize,
			Vector3D& offset
		);


		/*
			The same applies here, but the bounding box can be oriented.
			This makes it more flexible. It also means that because there's
			no fixed orientation, it doesn't have to be recalculated each
			frame, and can instead be recalculated using the transform matrix,
			of the rigid body, applied to its local position and orientation.

			The vertices sent need to be in local coordinates, so the
			transform matrix of the OBB can be extracted from that of the
			rigid body it encapsulates.
		*/
		static void calculateOrientedBoundingBox(
			const std::vector<Vector3D>& vertices,
			Vector3D& halfsize,
			Vector3D& offset,
			Quaternion& orientation
		);


		/*
			This works in the same way as a the bounding box function,
			where we find the smallest sphere that fully contains the
			polyhedron, even if it means the centre of the sphere is not
			the same as the centre of gravity (which means we will have an
			offset). 

			Like the OBB, the bounding sphere is calculated once from local
			vertices and then transformed each frame (translated, since
			there is no need for rotation).
		*/
		static void calculateBoundingSphere(
			const std::vector<Vector3D>& points,
			real& radius,
			Vector3D& offset
		);


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

		Vector3D furthestPoint;

		// Bounding box information
		Vector3D OBBHalfsize;
		Vector3D OBBOffset;
		Quaternion OBBOrientation;

		/*
			The AABB is technically meant to be recalculated each frame
			(since the AABB can't be transformed lest it no longer is axis
			aligned). However, in this case, it can be useful to save the
			local AABB information (the AABB in local coordinates)
			as we may want to convert it to an OBB later on by applying
			transformations on it.
		*/
		Vector3D AABBHalfsize;
		Vector3D AABBOffset;

		// Bounding sphere information
		real boundingSphereRadius;
		Vector3D boundingSphereOffset;

		Polyhedron(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		);


		/*
			Updates the global vertices and normals and centroid etc...
			using the transform matrix.
		*/
		void update();


		Vector3D findFurthestPoint() const;

		
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


		/*
			The halfsize of the shape can be used to determine its collision
			box (the rectangular prism which encompasses it) in case we want
			to avoid doing detailed collision detection for complex shapes.
			The same applies for collision spheres, which will have
			the radius as the magnitude of the halfsize.
		*/
		Vector3D getOBBHalfsize() const;


		Vector3D getAABBHalfsize() const;


		real getBoundingSphereRadius() const;


		// Returns the offset of the collision sphere
		Vector3D getSphereOffset() const;


		Vector3D getFurthestPoint() const;

		/*
			Transform matrix of the OBB, which adds the offset to the body
			transform matrix and the initial OBB orientation.
		*/
		Matrix3x4 getOBBTransformMatrix() const;

		/*
			While the AABB is supposed to be calculated each frame in order
			for it to remain axis aligned, we can calculate the AABB once
			and then transform it, effectively making it an OBB, though
			the original algorithm that calculates it is different
			(where the initial local coordinate bounding box is axis aligned).
		*/
		Matrix3x4 getAABBTransformMatrix() const;


		Matrix3x4 getBoundingSphereTransformMatrix() const;


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