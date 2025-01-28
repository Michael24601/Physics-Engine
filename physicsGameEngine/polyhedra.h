/*
	The rigid object gives maximum flexibility in the sense
	that we get to send the mesh and bounding box from outside.
	However, this can make initializing an object take longer,
	as we have to define them seperately first and then send them
	in.

	It also forces us to either know how to create a bounding box
	that perfectly fits the mesh we have, or call the fit() function
	of the bounding volume class we are used, which is computationally
	very expensive.

	We are also forced to either know the mesh shape's inertia tensor,
	or use the inertia tensor approximation function, which is also
	very computationally expensive, as it uses principal component
	analysis, a statistical method, which means it is also not
	necessarily the most accurate.

	These classes solve all these issues.
	- They define the mesh themselves, so they can manage its lifetime.
	- They know what the mesh is, and can create specific mesh shapes
	of likely objects such as cubes, spheres etc...
	- They choose a bounding volume shape that best fits the mesh shape,
	and also fit the bounding volume for the user. Instead of calling
	the expensive fit method, they simply leverage the knowledge of the
	what the shape looks like and set the parameters of the bounding
	volume directly. The bounding volume lifetime is also managed.
	- Knowing what the shape looks like also means that the inertia
	tensor can be calculated very quickly and exactly.

	The only downside is that the mesh/bouding volume are no longer
	shared between multiple rigid objects. This can be efficient
	when multiple objects have the same mesh. However, since most of
	these well known shapes, such as cubes, pyramids, cones etc...
	have only a few vertices, edges, and faces, this is not such 
	a waste. The shared mesh property is best used when importing
	complex models with tens of thounsands of vertices.
*/

#ifndef POLYHEDRA_H
#define POLYHEDRA_H

#include "rigidObject.h"
#include "faceBufferGenerator.h"
#include "edgeBufferGenerator.h"
#include "orientedBoundingBox.h"
#include "boundingSphere.h"
#include "cuboid.h"
#include "pyramid.h"
#include "sphere.h"
#include "cylinder.h"
#include "cone.h"

namespace pe {

	class PolyhedronObject : public RigidObject {

	public:

		VertexBuffer faceBuffer;
		VertexBuffer edgeBuffer;

		RenderComponent faceRenderer;
		RenderComponent edgeRenderer;

		PolyhedronObject(
			Mesh* mesh,
			BoundingVolume* boundingVolume,
			const Vector3D& position = Vector3D::ZERO,
			const Quaternion& orientation = Quaternion::IDENTITY,
			real mass = 0,
			const Matrix3x3& inertiaTensor = Matrix3x3::IDENTITY,
			bool smooth = false
		) : 
			RigidObject(
			mesh, boundingVolume, position, 
			orientation, mass, inertiaTensor
			),
			faceBuffer(createFaceVertexBuffer(
				this->mesh, GL_STATIC_DRAW, 
				(smooth ? NORMALS::VERTEX_NORMALS : NORMALS::FACE_NORMALS),
				UV::INCLUDE
			)),
			edgeBuffer(createEdgeVertexBuffer(
				this->mesh, GL_STATIC_DRAW
			)) {
			
			faceBuffer.setData(generateFaceData(
				this->mesh,
				(smooth ? NORMALS::VERTEX_NORMALS : NORMALS::FACE_NORMALS),
				UV::INCLUDE
			));

			edgeBuffer.setData(generateEdgeData(this->mesh));

			faceRenderer.setVertexBuffer(&faceBuffer);
			edgeRenderer.setVertexBuffer(&edgeBuffer);
		}

		void updateModelMatrix() {
			faceRenderer.setModel(convertToGLM(body.transformMatrix));
		}

	};

	class CuboidObject : public PolyhedronObject {

	public:

		static Matrix3x3 getInertiaTensor(
			real width,
			real height,
			real depth,
			real mass
		) {
			return Matrix3x3(
				(mass / 12.0) * (height * height + depth * depth), 0, 0,
				0, (mass / 12.0) * (width * width + depth * depth), 0,
				0, 0, (mass / 12.0) * (width * width + height * height)
			);
		}

		const real width;
		const real height;
		const real depth;

		CuboidObject(
			real width, real height, real depth,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : width{ width }, height{ height }, depth{depth},
			PolyhedronObject(
			new Cuboid(width, height, depth),
			new OrientedBoundingBox(Vector3D(width / 2.0, height / 2.0, depth / 2.0)),
			position,
			orientation,
			mass,
			getInertiaTensor(width, height, depth, mass),
			false
		) {}


		~CuboidObject() {
			delete mesh;
			delete boundingVolume;
		}
	};


	class SphereObject : public PolyhedronObject {

	public:

		static Matrix3x3 getInertiaTensor(
			real radius,
			real mass
		) {
			return Matrix3x3(
				(2.0 / 5.0) * mass * radius * radius, 0, 0,
				0, (2.0 / 5.0) * mass * radius * radius, 0,
				0, 0, (2.0 / 5.0) * mass * radius * radius
			);
		}

		const real radius;
		const int latitudeSegments;
		const int longitudeSegments;

		SphereObject(
			real radius, int latitudeSegments, int longitudeSegments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : radius{ radius }, latitudeSegments{ latitudeSegments },
			longitudeSegments{ longitudeSegments },
			PolyhedronObject(
			new Sphere(radius, latitudeSegments, longitudeSegments),
			new BoundingSphere(radius),
			position,
			orientation,
			mass,
			getInertiaTensor(radius, mass),
			true
		) {}


		~SphereObject() {
			delete mesh;
			delete boundingVolume;
		}
	};


	class PyramidObject : public PolyhedronObject {

	public:

		static Matrix3x3 getInertiaTensor(
			real side,
			real height,
			real mass
		) {
			return Matrix3x3(
				(mass / 10.0) * (3 * side * side + height * height), 0, 0,
				0, (mass / 10.0) * (3 * side * side + height * height), 0,
				0, 0, (mass / 5.0) * side * side
			);
		}

		const real side;
		const real height;

		PyramidObject(
			real side, real height,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : side{ side }, height{height},
			PolyhedronObject(
			new Pyramid(side, height),
			/*
				The centroid of the pyramid is one fourth of the way
				up from the base. The bounding box is centred halfway
				between the base and apex of the pyramid, so we have
				to shift it by a quarter of the height.
				The halfsize of the bounding box is half the side,
				half the height, then half a side.
			*/
			new OrientedBoundingBox(
				Vector3D(side / 2.0, height / 2.0, side / 2.0),
				Vector3D(0, height / 4.0, 0)
			),
			position,
			orientation,
			mass,
			getInertiaTensor(side, height, mass),
			false
		) {}


		~PyramidObject() {
			delete mesh;
			delete boundingVolume;
		}
	};


	class ConeObject : public PolyhedronObject {

	public:

		static Matrix3x3 getInertiaTensor(
			real radius,
			real height,
			real mass
		) {
			return Matrix3x3(
				(3.0 / 80.0) * mass * (radius * radius + 4.0 * height * height), 0, 0,
				0, (3.0 / 80.0) * mass * (radius * radius + 4.0 * height * height), 0,
				0, 0, (3.0 / 40.0) * mass * radius * radius
			);
		}

		const real radius;
		const real height;
		const int segments;

		ConeObject(
			real radius, real height, int segments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : radius{ radius }, height{ height }, segments{ segments },
			PolyhedronObject(
				new Cone(radius, height, segments),
				/*
					The centroid of the cone is one fourth of the way
					up from the base. The bounding box is centred halfway
					between the base and apex of the pyramid, so we have
					to shift it by a quarter of the height.
					The halfsize of the bounding box is the radius,
					half the height, then the radius.
				*/
				new OrientedBoundingBox(
					Vector3D(radius, height / 2.0, radius),
					Vector3D(0, height / 4.0, 0)
				),
				position,
				orientation,
				mass,
				getInertiaTensor(radius, height, mass),
				true
		) {}


		~ConeObject() {
			delete mesh;
			delete boundingVolume;
		}
	};


	class CylinderObject : public PolyhedronObject {

	public:

		static Matrix3x3 getInertiaTensor(
			real radius,
			real height,
			real mass
		) {
			return Matrix3x3(
				(1.0 / 12.0) * mass *
				(3.0 * radius * radius + height * height), 0, 0,
				0, (1.0 / 12.0) * mass *
				(3.0 * radius * radius + height * height), 0,
				0, 0, (1.0 / 12.0) * mass * (3.0 * radius * radius)
			);
		}

		const real radius;
		const real height;
		const int segments;

		CylinderObject(
			real radius, real height, int segments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass
		) : radius{ radius }, height{ height }, segments{ segments },
			PolyhedronObject(
			new Cylinder(radius, height, segments),
			new OrientedBoundingBox(Vector3D(radius, 0, radius)),
			position,
			orientation,
			mass,
			getInertiaTensor(radius, height, segments),
			true
		) {}


		~CylinderObject() {
			delete mesh;
			delete boundingVolume;
		}
	};
}

#endif