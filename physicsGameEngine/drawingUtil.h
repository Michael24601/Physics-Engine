/*
	File used for drawing util functions.
	It uses both old and new openGl (with and without shaders).
	They should never be mixed. Either use or don't use shaders, and
	if you do mix, draw all old openGL functions first, then use the
	shaders.
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
		Given an sfml window, draws a shape.
		The shape is sent via a vector of pairs of points that are meant
		to be connected. They are they then drawn as vertex arrays,
		with a color, specifying how to draw them.
		This function is used with normal, 2D sfml.
	*/
	void drawVectorOfLines(
		const std::vector<std::pair<Vector3D, Vector3D>>& vector,
		sf::RenderWindow& window,
		sf::Color color);

	/*
		Same, but works in 3D opengl in sfml.
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
		Works because all vertices are ordered(clockwise or counter
		clockwise). Assumes the points are in order and convex.
	*/
	std::vector<std::vector<Vector3D>> triangulateFace(
		const std::vector<Vector3D>& vertices
	);


	/*
		Same, but draws using shaders, not solid colors.
	*/
	void drawVectorOfPolygons3D(
		const std::vector<std::vector<Vector3D>>& polygons,
		glm::mat4& projection,
		glm::mat4& view,
		glm::mat4& model,
		GLuint& shaderProgram,
		real r, real g, real b, real a
	);

	void drawVectorOfLines3D(
		const std::vector<std::pair<Vector3D, Vector3D>>& lines,
		glm::mat4& projection,
		glm::mat4& view,
		glm::mat4& model,
		GLuint shaderProgram,
		real r, real g, real b, real a
	);
}


#endif
