
#ifndef CLOTH_WITH_BUNGEE_CORDS_H
#define CLOTH_WITH_BUNGEE_CORDS_H

#include "particleMesh.h"
#include "particleBungeeForce.h"
#include <assert.h>

namespace pe {

	class ClothWithBungeeCord : public ParticleMesh {

	private:

		/*
			Returns a grid of particles between the top left and bottom
			right corners, such as there are the specified number of
			particles per column and row.
		*/
		std::vector<Vector3D> returnParticleGrid(
			int columnSize,
			int rowSize,
			Vector3D topLeft,
			Vector3D bottomRight
		) {
			std::vector<Vector3D> positions;

			// Step sizes
			real columnStep = (bottomRight.x - topLeft.x) /
				static_cast<real>(columnSize - 1);
			real rowStep = (bottomRight.y - topLeft.y) /
				static_cast<real>(rowSize - 1);

			for (int row = 0; row < rowSize; ++row) {
				for (int col = 0; col < columnSize; ++col) {
					real x = topLeft.x + col * columnStep;
					real y = topLeft.y + row * rowStep;
					real z = topLeft.z + (bottomRight.z - topLeft.z) * 
						(col) / (columnSize - 1);

					positions.push_back(Vector3D(x, y, z));
				}
			}

			return positions;
		}

		/*
			Bungee force, where each force applies from one particle onto
			the other (two sided).
		*/
		struct BungeeForce {
			ParticleBungeeForce force1;
			ParticleBungeeForce force2;
		};

	public:

		/*
			Number of particles in the columns and rows.
			Their product will be the total number of particles.
		*/
		int columnSize;
		int rowSize;

		std::vector<BungeeForce> forces;

		ClothWithBungeeCord(
			// The mass and damping are the same for all particles here
			real mass,
			real damping,
			// So is the bungee rope strength (spring constant)
			real ropeStrength,
			int columnSize,
			int rowSize,
			Vector3D topLeft,
			Vector3D bottomRight
		) : 
			ParticleMesh(
				returnParticleGrid(
					columnSize, 
					rowSize,
					topLeft,
					bottomRight
				),
				// Number of particles is row * column count
				std::vector<real>(columnSize * rowSize, mass),
				std::vector<real>(columnSize* rowSize, damping)
			),
			rowSize{ rowSize },
			columnSize{ columnSize }{

			setEdges();
			setFaces();

			for (const MeshEdge& edge : edges) {
				// Creates a force for each edge
				Vector3D distanceVector = edge.particles.first->position - 
					edge.particles.second->position;
				real distance = distanceVector.magnitude();
				BungeeForce force{
					ParticleBungeeForce(edge.particles.first, ropeStrength,
						distance),
					ParticleBungeeForce(edge.particles.second, ropeStrength,
						distance),
				};
				forces.push_back(force);
			}
		}

		void setEdges() {
			for (int i = 0; i < columnSize * rowSize; i++) {
				// Structural links (horizontal and vertical)
				if (i - rowSize >= 0) {
					edges.push_back(MeshEdge(&particles[i], 
						&particles[i - rowSize]));
				}
				if (i - 1 >= 0 && i % rowSize != 0) {
					edges.push_back(MeshEdge(&particles[i], &particles[i - 1]));
				}
				// Other vertical (for extra support)
				if (i - 2 * rowSize >= 0) {
					edges.push_back(MeshEdge(&particles[i], 
						&particles[i - 2 * rowSize]));
				}
				// Diagonals (shear)
				if (i - rowSize - 1 >= 0 && i % rowSize != 0) {
					edges.push_back(MeshEdge(&particles[i], 
						&particles[i - rowSize - 1]));
				}
				if (i - rowSize + 1 >= 0 && (i + 1) % rowSize != 0) {
					edges.push_back(MeshEdge(&particles[i], 
						&particles[i - rowSize + 1]));
				}
			}
		}


		void setFaces() {
			for (int i = 0; i < columnSize - 1; i++) {
				for (int j = 0; j < rowSize - 1; j++) {
					/*
						Square faces(between columns and rows, in counter - 
						clockwise order).
					*/
					faces.push_back(
						MeshFace(
							std::vector<Particle*>{
								&particles[i * rowSize + j],
								& particles[(i + 1) * rowSize + j],
								& particles[(i + 1) * rowSize + (j + 1)],
								& particles[i * rowSize + (j + 1)]
							}
						)
					);
				}
			}
		}
	};
}

#endif
