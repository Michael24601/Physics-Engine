/*
	File used for drawing util functions.
	Temporarily here for debugging, not used in the final project.
*/

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
		glm::mat4& viewProjectionMatrix,
		sf::Color color
	);


	/*
		Same, but draws faces, instead of lines.
	*/
	void drawVectorOfPolygons3D(
		const std::vector<std::vector<Vector3D>>& polygons,
		glm::mat4& viewProjectionMatrix,
		sf::Color color,
		real opacity
	);


	/*
		Same, but draws using shaders, not solid colors.
	*/
	void drawVectorOfPolygons3D(
		const std::vector<std::vector<Vector3D>>& polygons,
		glm::mat4& viewProjectionMatrix,
		GLuint& shaderProgram,
		real r, real g, real b
	);

	void drawVectorOfLines3D(
		const std::vector<std::pair<Vector3D, Vector3D>>& lines,
		glm::mat4& viewProjectionMatrix,
		GLuint shaderProgram,
		real r, real g, real b
	);
}

