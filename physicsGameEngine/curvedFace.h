/*
	Class for curved face. Inherits from the face, but adds datafields
	and functions that can describe its curvature.
*/

#ifndef CURVED_FACE_H
#define CURVED_FACE_H

#include "face.h"

namespace pe {

	class CurvedFace : public Face {

	private:

		std::vector<Vector3D> localVertexNormals;
		std::vector<Vector3D> vertexNormals;

	public:

		CurvedFace(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::vector<int>& indeces,
			std::vector<Vector3D>& localVertexNormals
		) : Face(localVertices, globalVertices, indeces),
			localVertexNormals{localVertexNormals} {

			// Initially, the global vertex normals are the same as the local ones
			vertexNormals = localVertexNormals;
		}


		/*
			Overrides the update function to update the vertex normals.
		*/
		virtual void update(
			const Matrix3x4& transformMatrix,
			const Vector3D& bodyPosition
		) override {
			Vector3D transformedNormal = transformMatrix.transform(localNormal);
			normal = transformedNormal - bodyPosition;
			normal.normalize();
			centroid = transformMatrix.transform(localCentroid);

			for (int i = 0; i < getVertexNumber(); i++) {
				Vector3D transformedNormal = transformMatrix.transform(localVertexNormals[i]);
				vertexNormals[i] = transformedNormal - bodyPosition;
				vertexNormals[i].normalize();
			}
		}


		Vector3D getVertexNormal(int index) const {
			return vertexNormals[index];
		}


		/*
			In a curved face, the normal vectors of the vertices aren't
			all the same.
		*/
		virtual std::vector<Vector3D> getVertexNormals() const override {
			return vertexNormals;
		}
	};
}

#endif