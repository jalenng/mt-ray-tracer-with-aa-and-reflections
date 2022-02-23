// Jalen Ng - 014390116

#include <fstream>
#include "ofApp.h"

Mesh *Parser::loadObj(string filepath)
{
	// Create file stream object and open file
	ifstream file;
	file.open(filepath);

	// Handle failed opening
	if (!file)
	{
		cerr << "Failed to open file: " << filepath << endl;
		return new Mesh();
	}

	// Collect new vertices and shapes/faces
	vector<glm::vec3> vertices;
	vector<Shape> shapes;

	vertices.push_back(glm::vec3(0, 0, 0)); // obj face indices are not zero-based.
											// Fix this offset by pushing an arbritrary vector.

	// Iterate through each line in the file
	for (string line; getline(file, line);)
	{
		vector<string> splitLine;

		// Get the first part of the line
		int spacePos = line.find(" ", 0);
		string type = line.substr(0, spacePos);

		// Check the type, then parse the data accordingly
		if (type == "v") // Vertex
		{
			string data = line.substr(spacePos + 1, line.length());
			glm::vec3 vertex = Parser::parseVertexData(data);
			vertices.push_back(vertex);
		}
		else if (type == "f") // Face
		{
			string data = line.substr(spacePos + 1, line.length());
			Shape shape = Parser::parseFaceData(data);
			shapes.push_back(shape);
		}
	}

	// Close file
	file.close();

	// Create new mesh
	Mesh *mesh = new Mesh(vertices, shapes);

	//// Print diagnostic info
	//int numVertices = vertices.size() - 1; // Subtract by 1 to account for arbitrary vertex
	//int numFaces = shapes.size();

	//// Calculate size
	//float meshSizeInKB = (float)(numVertices * sizeof(glm::vec3) + numFaces * sizeof(Shape)) / 1000;

	//cout << numVertices << " vertices, " << numFaces << " faces. " << endl;
	//cout << "Mesh size: " << meshSizeInKB << " KB. " << endl;

	// Return new mesh
	return mesh;
}

glm::vec3 Parser::parseVertexData(string vertexString)
{

	vector<float> coords;
	int prevPos = -1;
	int pos = 0;

	// Iterate three times through vertexString to
	// get the vertex components (x, y, z)
	for (int i = 0; i < 3; i++)
	{

		int start = prevPos + 1;

		int end;
		pos = vertexString.find(" ", start);
		if (pos == -1)
			end = vertexString.length();
		else
			end = pos;

		string part = vertexString.substr(start, end);

		float number = stof(part);
		coords.push_back(number);

		prevPos = pos;
	}

	// Return the vertex created by the components
	return glm::vec3(coords[0], coords[1], coords[2]);
}

Shape Parser::parseFaceData(string faceString)
{

	vector<int> indices;
	int prevPos = -1;
	int pos = 0;

	// Iterate through faceString to
	// get the vertices of the face.
	while (pos != -1)
	{

		int start = prevPos + 1;

		int end;
		pos = faceString.find(" ", start);
		if (pos == -1)
			end = faceString.length();
		else
			end = pos;

		string part = faceString.substr(start, end);

		// Find the "/", then take whatever is before it
		int slashPos = part.find("/");
		part = part.substr(0, slashPos);

		int number = stoi(part);

		indices.push_back(number);

		prevPos = pos;
	}

	// Return the Shape created from those indices
	return Shape(indices);
}