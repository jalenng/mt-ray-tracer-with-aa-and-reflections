// Jalen Ng - 014390116

#include "ofApp.h"
#include <glm/gtx/intersect.hpp>

Plane::Plane(glm::vec3 p, glm::vec3 n, float w = 10, float h = 10, Material *material = 0)
{
	this->position = p;
	this->normal = glm::normalize(n);
	this->width = w;
	this->height = h;
	this->material = material;
	if (normal == glm::vec3(0, 1, 0))
		plane.rotateDeg(90, 1, 0, 0);
}

Plane::Plane()
{
	normal = glm::vec3(0, 1, 0);
	plane.rotateDeg(90, 1, 0, 0);
}

// Intersect Ray with Plane  (wrapper on glm::intersect)
//
bool Plane::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normalAtIntersect)
{
	float dist;
	bool insidePlane = false;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit)
	{
		Ray r = ray;
		point = r.evalPoint(dist);
		normalAtIntersect = this->normal;
		glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
		glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);
		if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0])
		{
			insidePlane = true;
		}
	}
	return insidePlane;
}

glm::vec3 Plane::getNormal(const glm::vec3 &p)
{
	return this->normal;
}

void Plane::draw()
{
	plane.setPosition(position);
	plane.setWidth(width);
	plane.setHeight(height);
	plane.setResolution(4, 4);
	plane.drawWireframe();
}

// My implementation (only works for planes w/ normal parallel to y-axis)
// Commented out because the new implementation below is superior.
//
//glm::vec2 Plane::getUVCoords(glm::vec3 point)
//{
//		float planeWidth = this->width;
//		float planeHeight = this->height;
//
//		float u = point.x - (position.x - (planeWidth / 2));
//		u /= planeWidth;
//
//		float v = point.z - (position.z - (planeHeight / 2));
//		v /= planeHeight;
//
//		// Convert u,v coords to texture pixel coords
//		float textureWidth = diffuseTexture.getWidth();
//		float textureHeight = diffuseTexture.getHeight();
//
//		int px = int(u * textureWidth);
//		int py = int(v * textureHeight);
//
//		return glm::vec2(u, v);
//}

// Reference: https://stackoverflow.com/questions/18663755/how-to-convert-a-3d-point-on-a-plane-to-uv-coordinates
//
glm::vec2 Plane::getUVCoords(glm::vec3 point)
{
	float planeWidth = this->width;
	float planeHeight = this->height;

	glm::vec3 n = glm::normalize(this->normal);

	float a = n[0] != 0
				  ? n[0]
				  : n[2];
	float b = n[1];

	glm::vec3 u_vec = glm::normalize(glm::vec3(b, -a, 0));
	glm::vec3 v_vec = glm::cross(n, u_vec);

	float u = 0.5 + glm::dot(u_vec, this->position - point) / (planeWidth);
	float v = 0.5 + glm::dot(v_vec, this->position - point) / (planeHeight);

	return glm::vec2(u, v);
}