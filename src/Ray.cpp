// Jalen Ng - 014390116

#include "ofApp.h"

Ray::Ray(glm::vec3 p, glm::vec3 d)
{
	this->p = p; // origin
	this->d = d; // direction
}

void Ray::draw(float t)
{
	ofDrawLine(p, p + t * d);
}

glm::vec3 Ray::evalPoint(float t)
{
	glm::vec3 point = p + (t * d);
	return point;
}