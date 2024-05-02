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

		std::vector<Vector3D> localVertexTangents;
		std::vector<Vector3D> vertexTangents;

		std::vector<Vector3D> localVertexBitangents;
		std::vector<Vector3D> vertexBitangents;


		/*
			Like the flat face, the tangent of any vertex in a curved face
			is calculated using the vertices to either side of it.
			Only differences are that we have to specify the given vertex
			index for each tangent (can't just use 0 each time) and we have
			to use each vertex's unique normal, not the face normal.
		*/
		Vector3D calculateTangent(int index, Basis basis) const {

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
			Vector3D edge1 = getVertex(secondIndex, basis) -
				getVertex(firstIndex, basis);
			Vector3D edge2 = getVertex(thirdIndex, basis) -
				getVertex(firstIndex, basis);

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
				tangent - (getVertexNormal(firstIndex, basis) * 
				(getVertexNormal(firstIndex, basis).scalarProduct(tangent)))
			).normalized();

			return tangent;
		}


		/*
			Like the flat face, the bitangent of any vertex in a curved face
			is calculated using the vertices to either side of it. Only
			difference is that we have to specify the given vertex index
			for each bitangent.
		*/
		Vector3D calculateBitangent(int index, Basis basis) const {

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
			Vector3D edge1 = getVertex(secondIndex, basis) -
				getVertex(firstIndex, basis);
			Vector3D edge2 = getVertex(thirdIndex, basis) -
				getVertex(firstIndex, basis);

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
				bitangent - (getVertexNormal(firstIndex, basis) *
					(getVertexNormal(firstIndex, basis).scalarProduct(bitangent)))
			).normalized();

			return bitangent;
		}

	public:

		CurvedFace(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::vector<int>& indices,
			std::vector<Vector3D>& localVertexNormals
		) : Face(localVertices, globalVertices, indices),
			localVertexNormals{localVertexNormals} {

			/* 
				Initially, the global vertex normals are the same as the
				local ones.
			*/
			vertexNormals = localVertexNormals;

			/* 
				We then calculate each tangentand bitangent, which are
				also distinct on a curved surface.
			*/
			localVertexTangents.resize(getVertexNumber());
			localVertexBitangents.resize(getVertexNumber());
			for (int i = 0; i < getVertexNumber(); i++) {
				localVertexTangents[i] = calculateTangent(i, Basis::LOCAL);
				localVertexBitangents[i] = calculateBitangent(i, Basis::LOCAL);
			}

			vertexTangents = localVertexTangents;
			vertexBitangents = localVertexBitangents;
		}


		/*
			Overrides the update function to update the vertex normals.
		*/
		virtual void update(
			const Matrix3x4& transformMatrix
		) override {

			Vector3D centre = transformMatrix.transform(Vector3D(0, 0, 0));

			Vector3D transformedNormal = transformMatrix.transform(localNormal);
			normal = transformedNormal - centre;
			normal.normalize();

			Vector3D transformedTangent = transformMatrix.transform(localTangent);
			tangent = transformedTangent - centre;
			tangent.normalize();

			Vector3D transformedBitangent = transformMatrix.transform(localBitangent);
			bitangent = transformedBitangent - centre;
			bitangent.normalize();

			centroid = transformMatrix.transform(localCentroid);

			for (int i = 0; i < getVertexNumber(); i++) {

				Vector3D transformedNormal = transformMatrix.transform(
					localVertexNormals[i]
				);
				vertexNormals[i] = transformedNormal - centre;
				vertexNormals[i].normalize();

				Vector3D transformedTangent = transformMatrix.transform(
					localVertexTangents[i]
				);
				vertexTangents[i] = transformedTangent - centre;
				vertexTangents[i].normalize();

				Vector3D transformedBitangent = transformMatrix.transform(
					localVertexBitangents[i]
				);
				vertexBitangents[i] = transformedBitangent - centre;
				vertexBitangents[i].normalize();
			}
		}

		/*
			Updates all the values without using the transform matrix.
			All values but the vertex normals can be updated, so the
			normals are not updated.
			The algorithms that calculated the local frame vectors are
			reused this time with global coordinates.
		*/
		void update() {
			centroid = Face::calculateCentroid(Basis::GLOBAL);
			normal = Face::calculateNormal(Basis::GLOBAL);
			//tangent = Face::calculateTangent(Basis::GLOBAL);
			//bitangent = Face::calculateBitangent(Basis::GLOBAL);
			for (int i = 0; i < getVertexNumber(); i++) {
			//	vertexTangents[i] = calculateTangent(i, Basis::GLOBAL);
			//	vertexBitangents[i] = calculateBitangent(i, Basis::GLOBAL);
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
		virtual Vector3D getVertexNormal(
			int index, Basis basis = Basis::GLOBAL
		) const override {
			if (basis == Basis::LOCAL) {
				return localVertexNormals[index];
			}
			else if (basis == Basis::GLOBAL) {
				return vertexNormals[index];
			}
		}


		virtual Vector3D getVertexTangent(
			int index, Basis basis = Basis::GLOBAL
		) const override {
			if (basis == Basis::LOCAL) {
				return localVertexTangents[index];
			}
			else if (basis == Basis::GLOBAL) {
				return vertexTangents[index];
			}
		}


		virtual Vector3D getVertexBitangent(
			int index, Basis basis = Basis::GLOBAL
		) const override {
			if (basis == Basis::LOCAL) {
				return localVertexBitangents[index];
			}
			else if (basis == Basis::GLOBAL) {
				return vertexBitangents[index];
			}
		}


		void setTextureCoordinates(
			std::vector<Vector2D>& textureCoordinates
		) override {
			this->textureCoordinates = textureCoordinates;

			/*
				We then need to recalculate the tangentand bitangents which depend
				on the texture coordinates.
			*/
			localTangent = Face::calculateTangent(Basis::LOCAL);
			localBitangent = Face::calculateBitangent(Basis::LOCAL);

			for (int i = 0; i < getVertexNumber(); i++) {
				localVertexTangents[i] = calculateTangent(i, Basis::LOCAL);
				localVertexBitangents[i] = calculateBitangent(i, Basis::LOCAL);
			}
		}

	};
}

#endif