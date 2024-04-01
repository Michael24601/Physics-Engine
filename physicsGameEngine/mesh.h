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
			Vertices corresponding to the location of the particles.
			The local ones correspond to the position of the particles
			at the creation of the cloth.
		*/
		std::vector<Vector3D> localVertices;
		std::vector<Vector3D> globalVertices;

		/*
			Defines how the normals of the vertices should be calculated
			(because the faces are all curved on a mesh, the faces need to
			be told each frame what the vertex normals are).
			Should be overriden by subclasses.
		*/
		virtual void calculateMeshNormals() = 0;

	public:

		std::vector<Particle*> particles;
		// The faces and edges needs to draw the particle mesh
		std::vector<CurvedFace*> faces;
		std::vector<Edge*> edges;


		Mesh(
			const std::vector<Vector3D> initialParticlePositions,
			real mass,
			real damping
		) : particles{particles} {

			localVertices = initialParticlePositions;
			/*
				Initially, the global vertices were the same as the
				original local ones.
			*/
			globalVertices = initialParticlePositions;

			particles.resize(initialParticlePositions.size());
			for (int i = 0; i < initialParticlePositions.size(); i++) {
				Particle* p = new Particle();
				p->position = initialParticlePositions[i];
				p->setMass(mass);
				p->damping = damping;
				particles[i] = p;
			}
		}


		~Mesh() {
			for (int i = 0; i < particles.size(); i++) {
				delete particles[i];
			}
			for (int i = 0; i < edges.size(); i++) {
				delete edges[i];
			}
			for (int i = 0; i < faces.size(); i++) {
				delete faces[i];
			}
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
			vertex normals of each face using the function defined by th
			subclass.
		*/
		void update() {
			/*
				First the global vertices need to be updated to be the same
				as the particle's as there is not transform matrix.
			*/
			for (int i = 0; i < particles.size(); i++) {
				globalVertices[i] = particles[i]->position;
			}

			calculateMeshNormals();
		}
	};
}

#endif