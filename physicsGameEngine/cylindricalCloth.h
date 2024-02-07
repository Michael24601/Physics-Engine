
#ifndef CYLINDER_CLOTH_H
#define CYLINDER_CLOTH_H

#include "cloth.h"

namespace pe {

	class CylindricalCloth : public Cloth {

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

	public:

		/*
			Number of particles in the columns and rows.
			Their product will be the total number of particles.
		*/
		int columnSize;
		int rowSize;

		CylindricalCloth(
			// The mass and damping are the same for all particles here
			real mass,
			real damping,
			// So is the bungee rope strength (spring constant)
			real ropeStrength,
			int columnSize,
			int rowSize,
			Vector3D topFaceCenter,
			Vector3D bottomFaceCenter
		) :
			Cloth(
				returnParticleGrid(
					columnSize,
					rowSize,
					topFaceCenter,
					bottomFaceCenter
				),
				// Number of particles is row * column count
				std::vector<real>(columnSize* rowSize, mass),
				std::vector<real>(columnSize* rowSize, damping),
				ropeStrength
			),
			rowSize{ rowSize },
			columnSize{ columnSize }{

			// Sets the edges and faces
			setEdges();
			setFaces();

			setForces();
			setConstraints();
		}

		virtual void setEdges() override {
			for (int i = 0; i < columnSize * rowSize; i++) {
				// Structural links (horizontal and vertical)
				if (i - rowSize >= 0) {
					edges.push_back(MeshEdge(&particles[i],
						&particles[i - rowSize]));
				}
				if (i - 1 >= 0 && i % rowSize != 0) {
					edges.push_back(MeshEdge(&particles[i], &particles[i - 1]));
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


		virtual void setFaces() override {
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