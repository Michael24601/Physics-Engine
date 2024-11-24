
#ifndef MESH_H
#define MESH_H

#include "vector3D.h"
#include "face.h"
#include "edge.h"
#include <numeric>
#include <unordered_set>
#include "util.h"

namespace pe {

	class Mesh {

	private:

		/*
			If the mesh has a curvature, then we need to store the
			vertex normals of each individual vertex in a face in this
			vector.
			Otherwise, it is left empty.
		*/
		std::vector<std::vector<Vector3D>> vertexNormals;		
		
		// Vertices in local coordinates
		std::vector<Vector3D> vertices;


		// Indexes of the faces and edges
		std::vector<Face> faces;
		std::vector<Edge> edges;

		/*
			The total number of vertices if each face vertex is counted as
			unique.
		*/
		int faceVertexCount;

	public:

		Mesh(
			const std::vector<Vector3D>& vertices,
			const std::vector<std::vector<int>>& faceIndexes,
			const std::vector<std::pair<int, int>>& edgeIndexes
		) : vertices{ vertices } {

			faceVertexCount = 0;
			faces.resize(faceIndexes.size());
			for (int i = 0; i < faces.size(); i++) {
				faces[i] = Face(this, faceIndexes[i]);
				faceVertexCount += faces[i].getVertexCount();
			}

			edges.resize(edgeIndexes.size());
			for (int i = 0; i < edges.size(); i++) {
				edges[i] = Edge( 
					edgeIndexes[i].first, 
					edgeIndexes[i].second
				);
			}
		}


		inline bool isCurved() const {
			return vertexNormals.size() > 0;
		}


		/*
			We can change the default vertex normals such that they are
			distinct from the face normals; this is the case when we we have
			a curved object we want to look smooth.
			Ensures the dimentionality of the sent vertex normals matches the
			number of faces and vertices in the faces.
		*/
		void setVertexNormals(
			const std::vector<std::vector<Vector3D>>& vertexNormals
		) {
			if (vertexNormals.size() != faces.size()) {
				throw std::invalid_argument(
					"The vertex normals must match the number of faces\n"
				);
				return;
			}

			this->vertexNormals.clear();
			this->vertexNormals.resize(faces.size());

			for (int i = 0; i < faces.size(); i++) {
				if (faces[i].getVertexCount() != vertexNormals[i].size()) {
					throw std::invalid_argument(
						"The vertex normals must match the number of faces\n"
					);
					return;
				}
				this->vertexNormals[i] = vertexNormals[i];
			}
		}


		/*
			Returns the appropriate vertex normals based on whether the shape
			is curved or not.
		*/
		Vector3D getVertexNormal(int faceIndex, int vertexIndex) const {
			return vertexNormals[faceIndex][vertexIndex];
		}


		Vector3D getFaceVertex(int faceIndex, int vertexIndex) const {
			return faces[faceIndex].getVertex(this, vertexIndex);
		}


		Vector3D getEdgeVertex(int edgeIndex, int vertexIndex) const {
			return edges[edgeIndex].getVertex(this, vertexIndex);
		}

		const Face& getFace(int index) const {
			return faces[index];
		}

		const Edge& getEdge(int index) const {
			return edges[index];
		}

		const Vector3D& getVertex(int index) const {
			return vertices[index];
		}

		const std::vector<Vector3D>& getVertices() const {
			return vertices;
		}

		int getFaceVertexCount() const {
			return faceVertexCount;
		}

		void setFaceTextureCoordinates(
			int index, 
			const std::vector<Vector2D>& uv
		) {
			faces[index].setTextureCoordinates(uv);
		}
		
		/*
			Setter for the vertices of the mesh.
			The number of vertices sent must match the number of
			vertices in the mesh (e.g. this function updates all the
			vertexes, it does not set them.
		*/
		void updateVertices(const std::vector<Vector3D>& vertices) {
			if (vertices.size() == this->vertices.size()) {
				this->vertices = vertices;
				for (Face& face : faces) {
					// Updates the normals and centroids
					face.calculateCentroid(this);
					face.calculateNormal(this);
				}
			}
			else {
				throw std::invalid_argument(
					"Vertex count must match the mesh"
				);
			}
		}


		void updateVertices(const Matrix3x4& transform) {
			for (Vector3D& vertex : vertices) {
				vertex = transform.transform(vertex);
			}
			/*
				Since the vertices are updates using a transform matrix,
				we can update the normals/centroids/vertex normals
				using the transform, instead of recalculating them.
			*/
			for (Face& face : faces) {
				face.normal = transform.transformDirection(face.normal).normalized();
				face.centroid = transform.transform(face.centroid);
			}
		}


		int getFaceCount() const { 
			return faces.size(); 
		}

		int getEdgeCount() const { 
			return edges.size(); 
		}

		int getVertexCount() const {
			return vertices.size();
		}

		/*
			The given point is assumed to be in the same basis of coordinates
			as the vertices. For instance, if the mesh is in its local coordinates
			but the point is in global coordinates, we need to use the inverse
			transform to get the point to the local basis.
		*/
		bool isPointInsideMesh(const Vector3D& point) const {

			int intersectionCount = 0;

			/*
				We then need to construct a ray from the point to a point outside
				the polyhedron. This was described as projecting the ray to
				infinity. Parctically, what we do to ensure the point ends up
				outside the polyhedron, is find the vertex furthest to the point
				and project the ray from the point at least that far.
			*/
			real furthestDistance = findFurthestPointFromCoordinate(
				point, this->vertices
			).magnitude();

			for (const Face& face : this->faces) {
				// Here we check if the point is inside the current face
				if (face.containsPoint(this, point, furthestDistance)) {
					++intersectionCount;
				}
			}

			/*
				If the number of intersections is odd, the point is inside the
				polyhedron.
			*/
			return (intersectionCount % 2) == 1;
		}

	};

}


#endif