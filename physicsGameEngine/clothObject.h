
#ifndef CLOTH_OBJECT_H
#define CLOTH_OBJECT_H

#include "cloth.h"
#include "renderComponent.h"
#include "faceBufferGenerator.h"
#include "edgeBufferGenerator.h"

namespace pe {

	class ClothObject : public Cloth {

	public:

		VertexBuffer faceBuffer;
		VertexBuffer edgeBuffer;

		RenderComponent faceRenderer;
		RenderComponent edgeRenderer;

		ClothObject(
			int columnDensity, int rowDensity,
			real height, real width,
			const Vector3D& direction1,
			const Vector3D& direction2,
			const Vector3D& origin,
			real mass,
			real damping,
			real dampingCoefficient,
			real structuralStiffness,
			real shearStiffness,
			real bendStiffness
		) : Cloth(columnDensity, rowDensity, height, width,
			direction1, direction2, origin, mass, damping,
			dampingCoefficient, structuralStiffness,
			shearStiffness, bendStiffness),
			faceBuffer(createFaceVertexBuffer(
				&this->mesh, GL_DYNAMIC_DRAW,
				(isCurved ? NORMALS::VERTEX_NORMALS : NORMALS::FACE_NORMALS),
				UV::INCLUDE
			)),
			edgeBuffer(createEdgeVertexBuffer(
				&this->mesh, GL_STATIC_DRAW
			)) {

			faceBuffer.setData(generateFaceData(
				&this->mesh,
				(isCurved ? NORMALS::VERTEX_NORMALS : NORMALS::FACE_NORMALS),
				UV::INCLUDE
			));

			edgeBuffer.setData(generateEdgeData(&this->mesh));

			faceRenderer.setVertexBuffer(&faceBuffer);
			edgeRenderer.setVertexBuffer(&edgeBuffer);
		
		}


		// Updates the vertex buffer (needed since the object is soft
		// Make sure the update function of the base soft object class
		// is called first (updates the mesh using the particles).
		void updateVertexBuffer() {

			faceBuffer.setData(generateFaceData(
				&this->mesh,
				(isCurved ? NORMALS::VERTEX_NORMALS : NORMALS::FACE_NORMALS),
				UV::INCLUDE
			));
			edgeBuffer.setData(generateEdgeData(&this->mesh));
		}
	};
}


#endif
