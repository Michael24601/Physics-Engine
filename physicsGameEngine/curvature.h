/*
	The curvature class can be used to add curvature to a mesh.
	Basically, a curved face in a mesh is a face whose vertices don't
	all have the same normal as the face.
	If a face is meant to have a smooth transition with certain adjacent
	faces, then then vertex's normal will be the combines normals of
	all those faces (not necessarily all the adjacent faces. For instance
	in a cylinder, the side faces transition smoothly from one to the other,
	but have sharp transitions with the base faces).

	We can achieve this by having a map of all the face vertices and the
	faces they transition smoothly with.
*/

#ifndef CURVATURE_H
#define CURVATURE_H

#include "mesh.h"

namespace pe {

	class Curvature {

	public:

		/*
			The outer vector is the faces.
			The vector in it is the vertices of the faces.
			The inner vector is the indexes of the faces whose normals
			will contribute to the vertex's normal.
			If a face is flat, then the vertices in it will only map to
			the face's index, that way its vertices all have the same normal
			as the face.
		*/
		std::vector<std::vector<std::vector<int>>> curvatureMap;


		Curvature(){}

		/*
			Sets the vertex normals using a curvature map.
			The curvature map remains constant no matter how the object
			deforms, so it allows up to recalculate vertex normals without
			a transform matrix, using the normals of the faces each vertex
			is in.
		*/
		void setMeshVertexNormals(Mesh& mesh) const {
			
			std::vector<std::vector<Vector3D>> vertexNormals(mesh.getFaceCount());

			for (int i = 0; i < curvatureMap.size(); i++) {

				vertexNormals[i].resize(mesh.getFace(i).getVertexCount());

				for (int j = 0; j < curvatureMap[i].size(); j++) {

					Vector3D vertexNormal;
					for (int k = 0; k < curvatureMap[i][j].size(); k++) {
						vertexNormal += mesh.getFace(curvatureMap[i][j][k]).getNormal();
					}
					vertexNormals[i][j] = vertexNormal.normalized();
				}
			}

			mesh.setVertexNormals(vertexNormals);
		}

	};
}


#endif