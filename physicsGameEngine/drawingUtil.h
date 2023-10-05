/*
	File used for drawing util functions.
*/

#include <SFML/Graphics.hpp>
#include "vector3D.h"

namespace pe {

	/*
		Transform lines(made up of a pair of points) in 3D space to lines
		in 2D space transformed using a viewMatrix according to a specific
		camera angle.
	*/
	std::vector<sf::VertexArray> transformLinesToVertexArray(
		const std::vector<std::pair<Vector3D, Vector3D>>& lines,
		const glm::mat4& viewMatrix, sf::Color color);


	// Draws a whole vector of lines in a given window
	void drawVectorOfVertexArray(std::vector<sf::VertexArray>& vector,
		sf::RenderWindow& window);
}

