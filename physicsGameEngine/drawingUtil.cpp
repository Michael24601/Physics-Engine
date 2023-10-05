
#include "drawingUtil.h"

using namespace pe;

void pe::drawVectorOfVertexArray(std::vector<sf::VertexArray>& vector,
	sf::RenderWindow& window) {
	for (const sf::VertexArray& vertexArray : vector) {
		window.draw(vertexArray);
	}
}


std::vector<sf::VertexArray> pe::transformLinesToVertexArray(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    const glm::mat4& viewMatrix, sf::Color color) {

    std::vector<sf::VertexArray> transformedVertexArrays;
    transformedVertexArrays.reserve(lines.size()); // Reserve space for efficiency

    for (const auto& line : lines) {
        sf::VertexArray transformedArray(sf::Lines, 2); // Create a vertex array to represent the line

        // Transform the start point of the line
        glm::vec3 start(line.first.x, line.first.y, line.first.z);
        glm::vec4 transformedStart = viewMatrix * glm::vec4(start, 1.0f);
        transformedArray[0].position = sf::Vector2f(transformedStart.x, transformedStart.y);

        // Transform the end point of the line
        glm::vec3 end(line.second.x, line.second.y, line.second.z);
        glm::vec4 transformedEnd = viewMatrix * glm::vec4(end, 1.0f);
        transformedArray[1].position = sf::Vector2f(transformedEnd.x, transformedEnd.y);
        transformedArray[1].color = transformedArray[0].color = color;

        transformedVertexArrays.push_back(transformedArray);
    }

    return transformedVertexArrays;
}