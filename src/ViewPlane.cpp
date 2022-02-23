// Jalen Ng - 014390116

#include "ofApp.h"

ViewPlane::ViewPlane(glm::vec2 p0, glm::vec2 p1)
{
	min = p0;
	max = p1;
}

ViewPlane::ViewPlane()
{
	min = glm::vec2(-4, -2.25);
	max = glm::vec2(4, 2.25);
	position = glm::vec3(0, 0, 10);
	normal = glm::vec3(0, 0, 1); // viewplane currently limited to Z axis orientation
}

// Convert (u, v) to (x, y, z)
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v)
{
	float w = getWidth();
	float h = getHeight();
	return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

void ViewPlane::setSize(glm::vec2 min, glm::vec2 max)
{
	this->min = min;
	this->max = max;
}
float ViewPlane::getAspect()
{
	return getWidth() / getHeight();
}

void ViewPlane::draw()
{
	ofNoFill();
	ofDrawRectangle(glm::vec3(min.x, min.y, position.z), getWidth(), getHeight());
}

float ViewPlane::getWidth()
{
	return (max.x - min.x);
}
float ViewPlane::getHeight()
{
	return (max.y - min.y);
}

// some convenience methods for returning the corners
//
glm::vec2 ViewPlane::getTopLeft()
{
	return glm::vec2(min.x, max.y);
}

glm::vec2 ViewPlane::getTopRight()
{
	return max;
}

glm::vec2 ViewPlane::getBottomLeft()
{
	return min;
}

glm::vec2 ViewPlane::getBottomRight()
{
	return glm::vec2(max.x, min.y);
}