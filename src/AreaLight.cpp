// Jalen Ng - 014390116

#include "ofApp.h";

// Area light constructor
//
AreaLight::AreaLight(glm::vec3 p, Mesh *mesh, float intensity)
{
	this->position = p;
	this->mesh = mesh;
	this->intensity = intensity;
	this->specularColor = ofColor::white;
}

// Draw proxy mesh
//
void AreaLight::draw()
{
	mesh->draw(this->position);
}

// Calculate this light's contribution.
// Given a point and its surface normal, the diffuse color, specular color, vector of scene objects, and camera position,
// return the light contribution in an ofColor
//
// This function overrides the Light class's function
//
ofColor AreaLight::calculateLight(
	const glm::vec3 &p,
	const glm::vec3 &normal,
	const ofColor diffuse,
	float specularPower,
	vector<SceneObject *> &scene,
	glm::vec3 cameraPos)
{
	vector<glm::vec3> vertices = mesh->vertices;
	int verticesSize = vertices.size();
	float intensityPerVertex = this->intensity;

	glm::vec3 resultVec;

	// Normalize the camera vector and normal vector
	glm::vec3 normalizedCameraVec = glm::normalize(cameraPos - p);
	glm::vec3 normalizedNormal = glm::normalize(normal);

	// Iterate through each vertex, counting how many are not blocked
	int numVerticesNotBlocked = 0;
	for (int i = 1; i < verticesSize; i++) // Ignore index 0 to account for non-zero-based vertex counting
	{
		glm::vec3 vertex = vertices[i];
		glm::vec3 lightVec = (vertex + position) - p;

		// If a shadow is casted, ignore this light's contribution
		if (shadowCasted(p, normal, lightVec, scene))
			continue;
		else
			numVerticesNotBlocked++;

	}

	glm::vec3 lightVec = position - p;
	ofColor lambertResult = lambert(diffuse, intensityPerVertex, normalizedNormal, lightVec);
	ofColor phongResult = phong(specularColor, this->intensity, lightVec, normalizedNormal, normalizedCameraVec, specularPower);
	
	float ratio = (float)numVerticesNotBlocked / (float)(verticesSize - 1);
	return ratio * (lambertResult + phongResult);
}