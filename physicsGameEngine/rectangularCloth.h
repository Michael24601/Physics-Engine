
#ifndef RECTANGULAR_CLOTH_H
#define RECTANGULAR_CLOTH_H

#include "particleMesh.h"

namespace pe {

	class RectangularCloth : public ParticleMesh {

	private:

		/*
			Returns a grid of particles between the top left and bottom
			right corners, such as there are the specified number of
			particles per column and row.
		*/
		static std::vector<Vector3D> returnParticleGrid(
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

		RectangularCloth(
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
				std::vector<real>(columnSize* rowSize, mass),
				std::vector<real>(columnSize* rowSize, damping),
				ropeStrength
			),
			rowSize{ rowSize },
			columnSize{ columnSize }{
			
			// Sets the edges, associations and faces
			setEdges();
			edgeAssociations = getEdgeAssociations();

			setFaces();
			setForces();
			setConstraints();
		}


		// The egdes just correspond to the associations
		virtual void setEdges() override {
			/*
				Note that we only include unique edges: if we have
				an edge from particle i to j, we don't also include one
				from j to i.
			*/
			for (int i = 0; i < columnSize * rowSize; i++) {
				// Structural links (horizontal and vertical)
				if (i >= rowSize) {
					edges.push_back(
						new Edge(&localVertices, &globalVertices, i, i - rowSize)
					);
				}
				if (i % rowSize != 0) {
					edges.push_back(
						new Edge(&localVertices, &globalVertices, i, i - 1)
					);
				}
				// Diagonals (shear)
				if (i >= rowSize && i % rowSize != 0) {
					edges.push_back(
						new Edge(&localVertices, &globalVertices, i, i - rowSize - 1)
					);
				}
				if (i >= rowSize && (i + 1) % rowSize != 0) {
					edges.push_back(
						new Edge(&localVertices, &globalVertices, i, i - rowSize + 1)
					);
				}
			}

		}


		virtual void setFaces() override {

			// Initial normals
			std::vector<Vector3D> normals = calculateMeshNormals();

			for (int i = 0; i < columnSize - 1; i++) {
				for (int j = 0; j < rowSize - 1; j++) {
					/*
						Square faces (between columns and rows, in counter -
						clockwise order).
					*/

					std::vector<int> faceIndexes{
						i * rowSize + j,
						(i + 1) * rowSize + j,
						(i + 1) * rowSize + (j + 1),
						i * rowSize + (j + 1)
					};

					std::vector<Vector3D>faceNormals;
					for (int index : faceIndexes) {
						faceNormals.push_back(normals[index]);
					}

					faces.push_back(
						new CurvedFace(
							&localVertices, 
							&globalVertices,
							faceIndexes,
							faceNormals
						)
					);
				}

			}
		}
	};
}

#endif