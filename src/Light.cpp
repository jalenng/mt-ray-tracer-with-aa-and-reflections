// Jalen Ng - 014390116

#include "ofApp.h";

Light::Light(glm::vec3 p, float intensity)
{
	this->position = p;
	this->intensity = intensity;
	this->specularColor = ofColor::white;
}

//  Draw proxy sphere
//
void Light::draw()
{
	ofSetColor(ofColor::white);
	ofDrawSphere(position, 0.1);
}

// Calculate this light's contribution.
// Given a point and its surface normal, the diffuse color, specular color, vector of scene objects, and camera position,
// return the light contribution in an ofColor
//
ofColor Light::calculateLight(
	const glm::vec3 &p,
	const glm::vec3 &normal,
	const ofColor diffuse,
	float specularPower,
	vector<SceneObject *> &scene,
	glm::vec3 cameraPos)
{
	// Input vectors
	glm::vec3 lightVec = position - p;
	glm::vec3 normalizedCameraVec = glm::normalize(cameraPos - p);
	glm::vec3 normalizedNormal = glm::normalize(normal);

	// Calculate the light contribution
	ofColor lambertResult = lambert(diffuse, this->intensity, normalizedNormal, lightVec);
	ofColor phongResult = phong(specularColor, this->intensity, lightVec, normalizedNormal, normalizedCameraVec, specularPower);
	ofColor lightContribution = lambertResult + phongResult;

	// If a shadow is casted, dim the area
	if (shadowCasted(p, normal, lightVec, scene))
		lightContribution /= 4;

	return lightContribution;
}

// Given a point p and its surface normal, check if the light vector is blocked by an object in the scene
//
bool Light::shadowCasted(glm::vec3 p, glm::vec3 normal, glm::vec3 lightVec, vector<SceneObject *> &scene)
{
	// Ncormalize input vectors
	glm::vec3 normalizedNormal = glm::normalize(normal);
	glm::vec3 normalizedLightVec = glm::normalize(lightVec);

	// Create light ray
	glm::vec3 lightRayOrigin = p + (normalizedNormal * 0.1);
	Ray lightRay(lightRayOrigin, normalizedLightVec);

	glm::vec3 pt;
	glm::vec3 nm;

	// Iterate through each scene object
	for (int i = 0; i < scene.size(); i++)
	{
		SceneObject *sceneObject = scene[i];

		// If the light ray intersects the SceneObject...
		if (sceneObject->intersect(lightRay, pt, nm))
		{
			// ... and the intersection point is between the light and given point...
			if (glm::distance(pt, lightRayOrigin) < glm::length(lightVec))
				return true; // ... then it's casting a shadow from the light onto the given point...
		}
	}

	return false;
}

// Lambert shading function.
// Given the diffuse color, intensity, normalized normal vector, and unnormalized light vector,
// returns the lighting contribution
//
ofColor Light::lambert(const ofColor diffuse, float intensity, const glm::vec3 &normal, const glm::vec3 &lightVec)
{

	// Calculate point-to-light distance
	float r = glm::length(lightVec);

	// Compute subtotals
	float illuminationFromSource = intensity / pow(r, 2);
	float diffuseReflection = glm::dot(glm::normalize(normal), glm::normalize(lightVec));
	diffuseReflection = std::max(float(0), diffuseReflection);

	// Use the Lambert shading equation to yield desired result
	ofColor result = diffuse * illuminationFromSource * diffuseReflection;

	return result;
}

// Phong shading function.
// Given the specular color, intensity, unnormalized light vector, normalized normal vector, normalized camera vector, and Phong power,
// returns the lighting contribution
//
ofColor Light::phong(
	const ofColor specular, 
	float intensity, 
	const glm::vec3 &lightVec, 
	const glm::vec3 &normal, 
	glm::vec3 cameraVec, 
	float power)
{
	// Calculate point-to-light distance
	float r = glm::length(lightVec);

	// Calculate bisector of l and v
	glm::vec3 h = glm::normalize(glm::normalize(lightVec) + cameraVec);

	// Calculate angle between h and normal
	float alpha = glm::angle(h, normal);

	// Compute subtotals
	float illuminationFromSource = intensity / pow(r, 2);
	float specularReflection = pow(std::max(float(0), cos(alpha)), power);

	// Use the Phong shading equation to yield desired result
	ofColor result = specular * illuminationFromSource * specularReflection;

	return result;
}