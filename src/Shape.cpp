// Jalen Ng - 014390116

#include "ofApp.h"

Shape::Shape(vector<int> indices)
{
	this->indices = indices;
};

void Shape::draw(vector<glm::vec3> vertices, glm::vec3 offset = glm::vec3(0))
{
	glm::vec3 startingVertex = vertices[indices[0]] + offset;
	glm::vec3 prevVertex = startingVertex;

	// Iterate through each index
	for (int i = 1; i < indices.size(); i++)
	{
		// Lookup the vertex based on the index
		glm::vec3 vertex = vertices[indices[i]] + offset;

		// Draw line between the two vertices
		ofDrawLine(prevVertex, vertex);

		prevVertex = vertex;
	}

	// Draw last line
	ofDrawLine(prevVertex, startingVertex);
}