/*
	File used for drawing util functions.
	This file contains functions that facilitate the drawing process,
	others that act as an interface between the physics module and the
	openGl graphics module.
	Finally, there are "quick and dirty" functions that use old openGl
	(without shaders), to draw lines and faces in solid colors.
	These should only be used for quick testing/debugging, and never
	when other faces/lines are being rendered with shaders elsewhere in
	the code, as they can't be mixed.
*/

#ifndef DRAWING_UTIL_H
#define DRAWING_UTIL_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>

#include <SFML/Graphics.hpp>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <SFML/OpenGL.hpp>
#include "vector3D.h"
#include "matrix3x4.h"

namespace pe {

	/*
		Draws lines in 3D openGl.
		Note that we send a viewProjection matrix as a parameters.
		This transforms each Vector3D point int 3D space into another point,
		before drawing it, in the sfml/opengl world, according to the
		position and angle of the camera (view) and perspective (projection).
		This matrix is a glm concept, and can be used to rotate, project,
		zoom, and translate points in space.
		This function is used with 2D opengl integrated sfml.
	*/
	void drawVectorOfLines3D(
		const std::vector<std::pair<Vector3D, Vector3D>>& lines,
		glm::mat4& projection,
		glm::mat4& view,
		glm::mat4& model,
		sf::Color color
	);


	/*
		Same, but draws faces, instead of lines.
	*/
	void drawVectorOfPolygons3D(
		const std::vector<std::vector<Vector3D>>& polygons,
		glm::mat4& projection,
		glm::mat4& view,
		glm::mat4& model,
		sf::Color color,
		real opacity
	);

	/*
		Returns a homogeneous matrix in glm mat4 format.
		Used in the graphics module.
	*/
	glm::mat4 convertToGLM(const Matrix3x4& m);

	/*
		Returns a 3D matrix in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v);


	/*
		Turns an n-polygon into n-2 triangles.
		Works because all vertices are ordered(clockwise or counter
		clockwise). Assumes the points are in order and convex.
	*/
	std::vector<std::vector<Vector3D>> triangulateFace(
		const std::vector<Vector3D>& vertices
	);

	/*
		Returns the normal of a triangle face (3 vertices).
		Assumes they are not colinear.
		It works because vectors are in the same order
	*/
	Vector3D getNormal(const Vector3D& v1, const Vector3D& v2,
		const Vector3D& v3);
}


#endif
