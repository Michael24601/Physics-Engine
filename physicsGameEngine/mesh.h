/*
	This is the header file for a Mesh class, which includes all
	functionality related to keeping track of and updating the
	particles, and the faces and edges they form.
	It does not however, include any variables or functions related
	to the forces or constraints that would typically be used to keep
	the particles in a mesh connected. This is because the details
	of how the particles are connected is left up to the subclasses.
*/

#ifndef MESH_H
#define MESH_H

#include "curvedFace.h"
#include "edge.h"
#include "particle.h"
#include <vector>

namespace pe {

	class Mesh {

	protected:

		/*
			Defines how the normals of the vertices should be calculated
			(because the faces are all curved on a mesh, the faces need to
			be told each frame what the vertex normals are).
			It returns a vector where each entry is a vector containing the
			normals of the vertices of each face of the mesh in order.
			Should be overriden by subclasses.
		*/
		virtual std::vector<std::vector<Vector3D>> calculateMeshNormals() = 0;

		void clearFacesAndEdges() {
			for (int i = 0; i < edges.size(); i++) {
				delete edges[i];
			}
			for (int i = 0; i < faces.size(); i++) {
				delete faces[i];
			}
		}

	public:

		std::vector<Particle*> particles;

		/*
			The vertices, faces, and edges are given in global coordinates
			because soft bodies like cloth have no transform matrix.
			Note that the vertices are just the positions of the particles.
		*/
		std::vector<Vector3D> vertices;
		std::vector<CurvedFace*> faces;
		std::vector<Edge*> edges;

		Mesh(
			const std::vector<Vector3D> initialParticlePositions,
			real mass,
			real damping
		) {
			particles.resize(initialParticlePositions.size());
			vertices.resize(particles.size());

			// The initial values of the particles
			for (int i = 0; i < initialParticlePositions.size(); i++) {
				Particle* p = new Particle();
				p->position = initialParticlePositions[i];
				p->setMass(mass);
				p->damping = damping;
				particles[i] = p;

				vertices[i] = p->position;
			}
		}


		~Mesh() {
			for (int i = 0; i < particles.size(); i++) {
				delete particles[i];
			}
			clearFacesAndEdges();
		}


		void setFaces(std::vector<CurvedFace*> faces) {
			this->faces = faces;
		}


		void setEdges(std::vector<Edge*> edges) {
			this->edges = edges;
		}


		/*
			Updates the mesh each frame, by setting the position of the
			global vertices as those of the particles, and updating the
			vertex normals of each face using the function defined by the
			subclass.
			This should be called after integrating the particles (moving
			them).
		*/
		void update() {

			/*
				Then we need to recreate the Face and Edge objects. Because
				we don't have a transform matrix, the faces and edes can't
				hold the local vertices, but have to hold the global ones,
				meaning they have to be recreated each frame as it is the
				simplest way to recalculate the tangents and bitangents and
				other values. This is done by using the formulas of these
				values with the new vertices (if we had a transform matrix,
				we could have also used it on these values).
				The only excpetion is the vertex normals which need to be
				calculated by the subclass of mesh, based on the particle
				mesh we have (cloth, blob...).
			*/

			std::vector<std::vector<Vector3D>> normals = calculateMeshNormals();

			for (int i = 0; i < particles.size(); i++) {
				vertices[i] = particles[i]->position;
			}

			for (int i = 0; i < faces.size(); i++) {
				for (int j = 0; j < normals[i].size(); j++) {
					faces[i]->setNormal(j, normals[i][j]);
				}
				faces[i]->recalculateFrameVectors();
			}
		}
	};
}

#endif