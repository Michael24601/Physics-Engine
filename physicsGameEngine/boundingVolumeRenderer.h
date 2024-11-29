/*
	A simple and quick way to render bounding volumes.

*/

#ifndef BOUNDING_VOLUME_RENDERER_H
#define BOUNDING_VOLUME_RENDERER_H

#include "boundingVolume.h"
#include "edgeBufferGenerator.h"
#include "cuboid.h"
#include "sphere.h"
#include "solidColorShader.h"
#include "boundingBox.h"
#include "boundingSphere.h"
#include "camera.h"

namespace pe {

	class BoundingVolumeRenderer {

	private:

		// Solid color shader to draw the bounding boxes
		SolidColorShader shader;

		/*
			Vertex buffer with the vertices of a cube with half sizes 1, 1, 1,
			centered at the origin. The cube can be scaled, rotated, and
			translated in the shader when a specific bounding box needs to
			be rendered.
		*/
		VertexBuffer* boxVertices;

		/*
			Vertex buffer for a sphere of radius 1 centered at the origin.
			Can also be scaled and translated for a specific bounding sphere
			rendering.
		*/
		VertexBuffer* sphereVertices;

		// Number of segments in sphere mesh
		const int sphereTesselation;

		// Color of the bounding box
		glm::vec4 color;

		/*
			To avoid having the bounding box clip the object, we can introduce
			a small offset factor.
		*/
		float offsetFactor = 1.02f;

	public:


		BoundingVolumeRenderer(int sphereTesselation, const glm::vec4& color)
			: sphereTesselation{ sphereTesselation }, color{color} {

			// Cube with halfsize 1, 1, 1
			Cuboid cube(2, 2, 2);
			Sphere sphere(1, sphereTesselation, sphereTesselation);

			boxVertices = new VertexBuffer(createEdgeVertexBuffer(&cube, GL_STATIC_DRAW));
			sphereVertices = new VertexBuffer(createEdgeVertexBuffer(&sphere, GL_STATIC_DRAW));

			boxVertices->setData(generateEdgeData(&cube));
			sphereVertices->setData(generateEdgeData(&sphere));

			shader.setObjectColor(color);
		}


		~BoundingVolumeRenderer(){
			delete boxVertices;
			delete sphereVertices;
		}


		void setCameraMatrices(const Camera& camera) {
			shader.setProjectionMatrix(camera.getProjectionMatrix());
			shader.setViewMatrix(camera.getViewMatrix());
		}


		void renderBoundingBox(
			const Matrix3x4& boundingBoxTransform,
			const Vector3D& halfsize
		) {

			glm::mat4 scaleMatrix = glm::scale(
				glm::mat4(1.0f), glm::vec3(
					halfsize.x * offsetFactor, 
					halfsize.y * offsetFactor,
					halfsize.z * offsetFactor
				)
			);
			glm::mat4 transformMatrix = convertToGLM(boundingBoxTransform);

			// Combining scaling and transform matrix
			glm::mat4 finalMatrix = transformMatrix * scaleMatrix;

			shader.setModelMatrix(finalMatrix);
			shader.render(*boxVertices);
		}


		void renderBoundingSphere(
			const Matrix3x4& boundingSphereTransform,
			real radius
		) {

			glm::mat4 scaleMatrix = glm::scale(
				glm::mat4(1.0f), glm::vec3(
					radius * offsetFactor, 
					radius * offsetFactor, 
					radius * offsetFactor
				)
			);
			glm::mat4 transformMatrix = convertToGLM(boundingSphereTransform);

			// Combining scaling and transform matrix
			glm::mat4 finalMatrix = transformMatrix * scaleMatrix;

			shader.setModelMatrix(finalMatrix);
			shader.render(*sphereVertices);
		}


		void renderBoundingVolume(
			const BoundingVolume* volume, 
			const Matrix3x4& boundingVolumeTransform
		) {
			if (volume->getType() == BoundingVolume::TYPE::BOX) {
				const BoundingBox* box = static_cast<const BoundingBox*>(volume);
				renderBoundingBox(boundingVolumeTransform, box->getHalfsize());
			}
			else if (volume->getType() == BoundingVolume::TYPE::SPHERE) {
				const BoundingSphere* sphere = static_cast<const BoundingSphere*>(volume);
				renderBoundingSphere(boundingVolumeTransform, sphere->getRadius());
			}
		}
	};

}

#endif