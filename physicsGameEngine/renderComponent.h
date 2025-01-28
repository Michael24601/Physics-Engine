/*
	Everything renderable should be rendered through a rendered component.
	The datafields of the class are the object specific shader uniforms,
	such as color and model matrix.

	This is just a container class for the uniforms and shaders, it does
	not create or manage them, as the user probably needs to reuse a texture
	or share a shader between several objects.
*/

#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "shader.h"
#include "rigidObject.h"

namespace pe {

	class RenderComponent {

	public:

		// The color of the object
		glm::vec4 color;

		// The texture (if there is one)
		GLuint texture;

		// The tetxure (if there is a cubemap one)
		GLuint cubemapTexture;

		/*
			In case the shader has a reflection component, this datafield
			holds an enviornment map from this object's point of view.
		*/
		GLuint environmentMap;

		// The model matrix
		glm::mat4 model;

		/*
			The vertex buffer object used(a pointer as it may be shared
			with other objects).
		*/
		const VertexBuffer* vertexBuffer;

		/*
			The shader is also likely shared between objects, so we have
			an address to a shader.
		*/
		Shader* shader;

		RenderComponent() : 
			model(glm::mat4(1.0)), vertexBuffer{ nullptr },
			shader{ nullptr }, color{ glm::vec4(0.0) }, texture{ 0 },
			environmentMap{ 0 } {}

		void setModel(const glm::mat4& model) {
			this->model = model;
		}

		void setColor(const glm::vec4& color) {
			this->color = color;
		}

		void setTexture(GLuint texture) {
			this->texture = texture;
		}

		void setCubemapTexture(GLuint cubemapTexture) {
			this->cubemapTexture = cubemapTexture;
		}

		void setEnvironmentMap(GLuint environmentMap) {
			this->environmentMap = environmentMap;
		}

		void setVertexBuffer(const VertexBuffer* vertexBuffer) {
			this->vertexBuffer = vertexBuffer;
		}

		void setShader(Shader* shader) {
			this->shader = shader;
		}

		void render() {
			if (shader == nullptr || vertexBuffer == nullptr) {
				return;
			}

			// Updates the shader with the object data
			shader->setObjectData(*this);
			shader->render(*vertexBuffer);
		}

	};
}

#endif