/*
	A wrapper for RenderComponent that specifically renders skyboxes
	using a cubemap.
*/

#ifndef SKYBOX_RENDERER_H
#define SKYBOX_RENDERER_H

#include "skyboxBufferGenerator.h"
#include "openglUtility.h"
#include "renderComponent.h"
#include "skyboxShader.h"

namespace pe {

	class SkyboxRenderer {

	private:

		VertexBuffer buffer;

		GLuint cubemap;

		SkyboxShader shader;

	public:

		RenderComponent skyboxRenderer;

		SkyboxRenderer(
			const std::vector<std::string>& facesSource,
			float scale
		) : buffer(createSkyboxVertexBuffer()),
			cubemap(loadCubemap(facesSource)) {

			buffer.setData(skyboxVertices);

			glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
			skyboxRenderer.setModel(model);
			skyboxRenderer.setShader(&shader);
			skyboxRenderer.setVertexBuffer(&buffer);
			skyboxRenderer.setCubemapTexture(cubemap);
		}

		void setViewProjection(
			const glm::mat4 view,
			const glm::mat4 projection
		) {
			shader.setViewMatrix(view);
			shader.setProjectionMatrix(projection);
		}

		void renderSkybox() {
			skyboxRenderer.render();
		}

	};
}


#endif