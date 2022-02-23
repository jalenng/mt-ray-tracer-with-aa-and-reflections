// Jalen Ng - 014390116

#define _USE_MATH_DEFINES

#include <math.h>
#include "ofApp.h"

Sphere::Sphere(glm::vec3 p, float r, Material *material)
{
	this->position = p;
	this->radius = r;
	this->material = material;
}

bool Sphere::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	// Use glm's built-in intersect function
	bool intersectionFound = glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal);
	return intersectionFound;
}

void Sphere::draw()
{
	ofSetColor(ofColor::white);
	ofDrawSphere(position, radius);
}

// Reference: https://en.wikipedia.org/wiki/UV_mapping
//
glm::vec2 Sphere::getUVCoords(glm::vec3 point)
{
	// d: normalized vector from center of sphere to point of intersection
	glm::vec3 d = glm::normalize(point - position);

	float u = 0.5 + (atan2(d.x, d.z) / (2 * M_PI));
	float v = 0.5 - (asin(d.y) / M_PI);

	return glm::vec2(u, v);
}