
#ifndef SOFT_OBJECT_H
#define SOFT_OBJECT_H

#include "mesh.h"
#include "curvature.h"
#include "softBody.h"

namespace pe {

	class SoftObject {

	public:

		SoftBody body;

		/*
			Because a soft object deforms, if it is curved, it is not
			enough to set the vertex normals of each vertex in each face,
			as the object moves. Instead, we need to have a curvature object,
			which remains the same no matter how a mesh deforms.
			It does so by keeping track of which face normals contribute to
			the normal of each vertex, those being the ones with which the
			vertex must have a smooth transition.
		*/
		Curvature curvature;

		/*
			This value is true if the shape has a curvature, and is using it.
		*/
		bool isCurved;

		/*
			Unlike a rigid object, a soft object can't share its mesh
			with others, nor its renderer, as they can be modified by
			the class, and often are, when it deforms, so it's fully
			managed by this class.
		*/
		Mesh mesh;


		/*
			Maps particles in the soft body to a vertex in the mesh that
			corresponds to it, so that we can modify the mesh accordingly.
		*/
		std::vector<std::pair<int, int>> particleVertexMap;
		
		SoftObject(
			const std::vector<Vector3D>& vertices,
			const std::vector<std::vector<int>>& faceIndexes,
			const std::vector<std::pair<int, int>>& edgeIndexes,
			const std::vector<Vector3D>& particleCoordinates,
			real mass,
			real damping,
			const std::vector<std::pair<int, int>>& springPairs,
			const std::vector<real> springStrengths,
			const std::vector<std::pair<int, int>>& particleVertexMap,
			const Curvature& curvature = Curvature()
		) : mesh(vertices, faceIndexes, edgeIndexes),
			body(particleCoordinates, mass, damping, springPairs, springStrengths),
			isCurved{curvature.curvatureMap.size() > 0},
			curvature(curvature),
			particleVertexMap{ particleVertexMap } {
		
			if (isCurved) {
				this->curvature.setMeshVertexNormals(this->mesh);
			}
		}


		/*
			After the soft body particles have had forces applied to them
			and been integrated.
		*/
		void update() {
			std::vector<Vector3D> vertices = mesh.getVertices();
			for (int i = 0; i < particleVertexMap.size(); i++) {
				vertices[particleVertexMap[i].second] =
					body.particles[particleVertexMap[i].first].position;
			}
			// This automatically updates the faces and their normals
			mesh.updateVertices(vertices);

			/*
				However, we still need to update the vertex specific normals
				using the curvature.
			*/
			if (isCurved) {
				curvature.setMeshVertexNormals(mesh);
			}
		}

	};
}

#endif