// Jalen Ng - 014390116

#include "ofApp.h"

bool SceneObject::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	return false;
}

glm::vec2 SceneObject::getUVCoords(glm::vec3 point)
{
	return glm::vec2(0, 0);
}

ofColor SceneObject::sampleDiffuseColor(glm::vec3 point)
{
	glm::vec2 uv = getUVCoords(point);
	ofColor diffuse = material->sampleDiffuse(uv);
	return diffuse;
}

float SceneObject::sampleSpecularPower(glm::vec3 point)
{
	glm::vec2 uv = getUVCoords(point);
	float specular = material->sampleSpecular(uv);
	return specular;
}