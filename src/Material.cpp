// Jalen Ng - 014390116

#include "ofApp.h"

Material::Material()
{
	diffuseColor = ofColor::gray;
	isUsingDiffuseTexture = false;
	isUsingSpecularTexture = false;
}

void Material::setReflectivity(float reflectivity)
{
	this->reflectivity = reflectivity;
}

void Material::setDiffuse(ofColor color)
{
	this->diffuseColor = color;
	this->isUsingDiffuseTexture = false;
}

void Material::setSpecular(float power)
{
	this->specularPower = power;
	this->isUsingSpecularTexture = false;
}

void Material::setDiffuse(ofImage texture)
{
	this->diffuseTexture = texture;
	this->isUsingDiffuseTexture = true;
}

void Material::setSpecular(ofImage texture)
{
	this->specularTexture = texture;
	this->isUsingSpecularTexture = true;
}

void Material::setTileFrequency(float xTile, float yTile)
{
	this->xTileFrequency = xTile;
	this->yTileFrequency = yTile;
}

ofColor Material::sampleDiffuse(glm::vec2 uv)
{
	if (!isUsingDiffuseTexture)
		return diffuseColor;

	// Deconstruct uv vector into its values
	float u = uv[0];
	float v = uv[1];

	// Loop u and v to yield tiled effect
	float uDivisor = 1.0 / this->xTileFrequency;
	float vDivisor = 1.0 / this->yTileFrequency;

	u = fmod(u, uDivisor) / uDivisor;
	v = fmod(v, vDivisor) / vDivisor;

	// Convert uv coords to texture pixel coords
	int textureWidth = diffuseTexture.getWidth();
	int textureHeight = diffuseTexture.getHeight();

	int px = int(u * textureWidth);
	int py = int(v * textureHeight);

	// Retrieve the color from the texture
	ofColor diffuse = diffuseTexture.getColor(px, py);

	return diffuse;
}

float Material::sampleSpecular(glm::vec2 uv)
{
	if (!isUsingSpecularTexture)
		return specularPower;

	// Deconstruct uv vector into its values
	float u = uv[0];
	float v = uv[1];

	// Loop u and v to yield tiled effect
	float uDivisor = 1.0 / this->xTileFrequency;
	float vDivisor = 1.0 / this->yTileFrequency;

	u = fmod(u, uDivisor) / uDivisor;
	v = fmod(v, vDivisor) / vDivisor;

	// Convert uv coords to texture pixel coords
	int textureWidth = specularTexture.getWidth();
	int textureHeight = specularTexture.getHeight();

	int px = int(u * textureWidth);
	int py = int(v * textureHeight);

	// Retrieve the color from the texture
	float specularIntensity = specularTexture.getColor(px, py).getLightness() / 255;

	return specularIntensity;
}