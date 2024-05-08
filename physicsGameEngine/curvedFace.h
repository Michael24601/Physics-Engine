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
		Vector3D calculateTangent(int index) const {

			/*
				We need to find distinct vertices to either side of any
				vertex (first one, 0 in our case), since the tangent
				is uniform across all the face (assuming it's flat).
			*/
			int firstIndex = index, secondIndex, thirdIndex;
			findDistinctVertices(
				firstIndex, secondIndex, thirdIndex
			);

			// Get two edges of the triangle
			Vector3D edge1 = getVertex(secondIndex) -
				getVertex(firstIndex);
			Vector3D edge2 = getVertex(thirdIndex) -
				getVertex(firstIndex);

			// Get the corresponding texture coordinate differences
			Vector2D deltaUV1 = textureCoordinates[secondIndex] -
				textureCoordinates[firstIndex];
			Vector2D deltaUV2 = textureCoordinates[thirdIndex] -
				textureCoordinates[firstIndex];

			// Solve linear equation system to find tangent
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			Vector3D tangent(
				f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
				f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
				f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
			);

			// Orthogonalize the tangent with respect to the vertex normal
			tangent = (
				tangent - (getVertexNormal(firstIndex) * 
				(getVertexNormal(firstIndex).scalarProduct(tangent)))
			).normalized();

			return tangent;
		}


		/*
			Like the flat face, the bitangent of any vertex in a curved face
			is calculated using the vertices to either side of it. Only
			difference is that we have to specify the given vertex index
			for each bitangent.
		*/
		Vector3D calculateBitangent(int index) const {

			/*
				We need to find distinct vertices to either side of any
				vertex (first one, 0 in our case), since the bitangent
				is uniform across all the face (assuming it's flat).
			*/
			int firstIndex = index, secondIndex, thirdIndex;
			findDistinctVertices(
				firstIndex, secondIndex, thirdIndex
			);

			// Get two edges of the triangle
			Vector3D edge1 = getVertex(secondIndex) -
				getVertex(firstIndex);
			Vector3D edge2 = getVertex(thirdIndex) -
				getVertex(firstIndex);

			// Get the corresponding texture coordinate differences
			Vector2D deltaUV1 = textureCoordinates[secondIndex] -
				textureCoordinates[firstIndex];
			Vector2D deltaUV2 = textureCoordinates[thirdIndex] -
				textureCoordinates[firstIndex];

			// Solve linear equation system to find bitangent
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			Vector3D bitangent(
				f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
				f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
				f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
			);

			/*
				Orthogonalize the bitangent with respect to the vertex normal
				This step is crucial since the vertex normal ensures the
				tangent and bitangents are different for different vertices,
				otherwise they won't be created orthogonally relative to each
				vertex's normal.
			*/
			bitangent = (
				bitangent - (getVertexNormal(firstIndex) *
					(getVertexNormal(firstIndex).scalarProduct(bitangent)))
			).normalized();

			return bitangent;
		}

	public:

		CurvedFace(
			std::vector<Vector3D>* vertices,
			std::vector<int>& indices,
			std::vector<Vector3D>& vertexNormals
		) : Face(vertices, indices),
			vertexNormals{vertexNormals} {

			/* 
				We then calculate each tangentand bitangent, which are
				also distinct on a curved surface.
			*/
			vertexTangents.resize(getVertexNumber());
			vertexBitangents.resize(getVertexNumber());
			for (int i = 0; i < getVertexNumber(); i++) {
				vertexTangents[i] = calculateTangent(i);
				vertexBitangents[i] = calculateBitangent(i);
			}
		}

		
		/*
			This function is called when the vertices or normals
			or both change, and the tangents and bitangents need to be
			recalculated.
			The vertex normals can't be changed by the face, as they are
			managed from the outside of this class.
		*/
		void recalculateFrameVectors() override {
			Face::recalculateFrameVectors();
			for (int i = 0; i < getVertexNumber(); i++) {
			//	vertexTangents[i] = calculateTangent(i);
			//	vertexBitangents[i] = calculateBitangent(i);
			}
		}


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
		void setNormal(int index, const Vector3D& normal) {
			vertexNormals[index] = normal;
		}


		/*
			This function is overriden because in a curved face, the normal
			at each vertex may not be the same as the face's.
		*/
		virtual Vector3D getVertexNormal(int index) const override {
			return vertexNormals[index];
		}


		virtual Vector3D getVertexTangent(int index) const override {
			return vertexTangents[index];
		}


		virtual Vector3D getVertexBitangent(int index) const override {
			return vertexBitangents[index];
		}


		void setTextureCoordinates(
			std::vector<Vector2D>& textureCoordinates
		) override {
			this->textureCoordinates = textureCoordinates;

			/*
				We then need to recalculate the tangentand bitangents which depend
				on the texture coordinates.
			*/
			tangent = Face::calculateTangent();
			bitangent = Face::calculateBitangent();

			for (int i = 0; i < getVertexNumber(); i++) {
				vertexTangents[i] = calculateTangent(i);
				vertexBitangents[i] = calculateBitangent(i);
			}
		}

	};
}

#endif