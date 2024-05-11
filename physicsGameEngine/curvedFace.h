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
		
		std::vector<Vector3D> vertexNormals;
		std::vector<Vector3D> vertexTangents;
		std::vector<Vector3D> vertexBitangents;


		/*
			Like the flat face, the tangent of any vertex in a curved face
			is calculated using the vertices to either side of it.
			Only differences are that we have to specify the given vertex
			index for each tangent (can't just use 0 each time) and we have
			to use each vertex's unique normal, not the face normal.
		*/
		Vector3D calculateTangent(int index) const;


		/*
			Like the flat face, the bitangent of any vertex in a curved face
			is calculated using the vertices to either side of it. Only
			difference is that we have to specify the given vertex index
			for each bitangent.
		*/
		Vector3D calculateBitangent(int index) const;

	public:

		CurvedFace(
			std::vector<Vector3D>* vertices,
			std::vector<int>& indices,
			std::vector<Vector3D>& vertexNormals
		);

		
		/*
			This function is called when the vertices or normals
			or both change, and the tangents and bitangents need to be
			recalculated.
			The vertex normals can't be changed by the face, as they are
			managed from the outside of this class.
		*/
		void recalculateFrameVectors() override;


		/*
			Because not all objects using the face are rigid, there must be
			a way to update all of its contents without using a transform
			matrix.
			For the tangents, centroid etc... we can just recalculate them
			using the formulas but with the global coordinates/basis.
			This isn't possible for the normals of a curved face, as the
			curvature is unknown and set from the outside. So only from the
			outside can they be updated.
		*/
		void setNormal(int index, const Vector3D& normal);


		/*
			This function is overriden because in a curved face, the normal
			at each vertex may not be the same as the face's.
		*/
		virtual Vector3D getVertexNormal(int index) const override;


		virtual Vector3D getVertexTangent(int index) const override;


		virtual Vector3D getVertexBitangent(int index) const override;


		void setTextureCoordinates(
			std::vector<Vector2D>& textureCoordinates
		) override;

	};
}

#endif