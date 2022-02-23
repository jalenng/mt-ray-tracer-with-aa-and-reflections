// Jalen Ng - 014390116

#include "ofApp.h"

RenderCam::RenderCam()
{
	position = glm::vec3(0, 0, 15);
	aim = glm::vec3(0, 0, -1);
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v)
{
	glm::vec3 pointOnPlane = view.toWorld(u, v);
	return (Ray(position, glm::normalize(pointOnPlane - position)));
}

void RenderCam::draw()
{
	ofNoFill();
	ofSetColor(ofColor::white);

	// Draw box at camera position
	ofDrawBox(position, 1.0);

	// Draw camera frustrum
	view.draw();
	float viewPlaneZ = view.position.z;
	ofDrawLine(position, glm::vec3(view.getTopLeft(), viewPlaneZ));
	ofDrawLine(position, glm::vec3(view.getTopRight(), viewPlaneZ));
	ofDrawLine(position, glm::vec3(view.getBottomLeft(), viewPlaneZ));
	ofDrawLine(position, glm::vec3(view.getBottomRight(), viewPlaneZ));
}