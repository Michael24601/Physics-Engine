
#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "mesh.h"
#include <vector>
#include <map>

namespace pe {

	/*
		Returns a homogeneous matrix in the glm format.
	*/
	glm::mat4 convertToGLM(const Matrix3x4& m);


	/*
		Returns a 3D vector in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v);


	/*
		Returns a 2D vector in the glm format.
	*/
	glm::vec2 convertToGLM(const Vector2D& v);

}

#endif