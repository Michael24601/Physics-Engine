/*
	Class for curved face. Inherits from the face, but adds datafields
	and functions that can describe its curvature.
*/

#ifndef CURVED_FACE_H
#define CURVED_FACE_H

#include "face.h"

namespace pe {

	class CurvedFace : public Face {

	public:

		std::vector<Vector3D> localVertexNormals;
		std::vector<Vector3D> vertexNormals;


		CurvedFace(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::vector<int> indeces
		) : Face(localVertices, globalVertices, indeces) {}


		/*
			Function that sets the local normal vector at each vertex.
		*/
		void setLocalVertexNormals(
			const std::vector<Vector3D>& localVertexNormals
		) {
			this->localVertexNormals = localVertexNormals;
		}


		/*
			Overrides the update function to update the vertex normals.
		*/
		void update(const Matrix3x4& transformMatrix) override {
			normal = transformMatrix.transform(localNormal);
			normal.normalize();
			centroid = transformMatrix.transform(localCentroid);

			for (int i = 0; i < getVertexNumber(); i++) {
				vertexNormals[0] = transformMatrix.transform(
					localVertexNormals[0]
				);
			}
		}
	};
}

#endif