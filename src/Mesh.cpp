// Jalen Ng - 014390116

#include "ofApp.h"

Mesh::Mesh()
{
	this->vertices = {};
	this->shapes = {};
}

Mesh::Mesh(vector<glm::vec3> vertices, vector<Shape> shapes)
{
	this->vertices = vertices;
	this->shapes = shapes;
};

bool Mesh::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	return false;
}

void Mesh::draw(glm::vec3 offset = glm::vec3(0, 0, 0))
{
	// Iterate through and draw each shape
	for (int i = 0; i < shapes.size(); i++)
	{
		ofSetColor(ofColor::white);
		// Shape.draw() takes in a vector of vertices to use for lookup
		shapes[i].draw(vertices, offset);
	}
}