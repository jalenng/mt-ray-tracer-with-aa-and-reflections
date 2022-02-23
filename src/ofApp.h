// Jalen Ng - 014390116

#pragma once

#include <thread>
#include "ofMain.h"
#include "ofxGui.h"
#include <glm/gtx/intersect.hpp>

// 2D shape
//
class Shape
{
public:
	Shape(vector<int> indices);
	void draw(vector<glm::vec3> vertices, glm::vec3 offset);

	vector<int> indices;
};

// General-purpose ray
//
class Ray
{
public:
	Ray(glm::vec3 p, glm::vec3 d);

	void draw(float t);
	glm::vec3 evalPoint(float t);

	glm::vec3 p, d;
};

// Material that contains diffuse and specular parameters
//
class Material
{
public:
	Material();

	void setReflectivity(float reflectivity);
	void setTileFrequency(float xTile, float yTile);

	void setDiffuse(ofColor color);
	void setDiffuse(ofImage texture);

	void setSpecular(float power);
	void setSpecular(ofImage texture);

	ofColor sampleDiffuse(glm::vec2 uv);
	float sampleSpecular(glm::vec2 uv);

	float xTileFrequency = 1;
	float yTileFrequency = 1;

	float reflectivity = 0;
	ofColor diffuseColor = ofColor::gray;
	float specularPower = 10;

	ofImage diffuseTexture, specularTexture;

	bool isUsingDiffuseTexture = false;
	bool isUsingSpecularTexture = false;
};

// Base class for any renderable object in the scene
//
class SceneObject
{
public:
	// pure virtual funcs - must be overloaded
	virtual void draw() = 0;
	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);

	virtual glm::vec2 getUVCoords(glm::vec3 point);

	virtual ofColor sampleDiffuseColor(glm::vec3 point);
	virtual float sampleSpecularPower(glm::vec3 point);

	glm::vec3 position;
	Material *material;
};

// Mesh that holds a collection of shapes
//
class Mesh
{
public:
	Mesh();
	Mesh(vector<glm::vec3> vertices, vector<Shape> shapes);

	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	void draw(glm::vec3 offset);

	vector<glm::vec3> vertices; // Each glm::vec3 represents a vertex
	vector<Shape> shapes;		// Each triangle has a vector that holds three indices
};

// OBJ-to-Mesh parser
//
class Parser
{
public:
	static Mesh *loadObj(string filepath);
	static glm::vec3 parseVertexData(string vertexString);
	static Shape parseFaceData(string faceString);
};

// General-purpose light (point light)
//
class Light : public SceneObject
{
public:
	Light() {}
	Light(glm::vec3 p, float intensity);

	virtual void draw();

	virtual ofColor calculateLight(
		const glm::vec3 &p,
		const glm::vec3 &normal,
		const ofColor diffuse,
		float specularIntensity,
		vector<SceneObject *> &scene,
		glm::vec3 cameraPos);

	bool shadowCasted(
		glm::vec3 p,
		glm::vec3 normal,
		glm::vec3 lightVec,
		vector<SceneObject *> &scene);

	ofColor lambert(
		const ofColor diffuse,
		float intensity,
		const glm::vec3 &normal,
		const glm::vec3 &lightVec);

	ofColor phong(
		const ofColor specular,
		float intensity,
		const glm::vec3 &lightVec,
		const glm::vec3 &normal,
		glm::vec3 cameraVec,
		float power);

	float intensity;
	ofColor specularColor;
};

// Area light
//
class AreaLight : public Light
{
public:
	AreaLight(glm::vec3 p, Mesh *mesh, float intensity);

	void draw();

	ofColor calculateLight(
		const glm::vec3 &p,
		const glm::vec3 &normal,
		const ofColor diffuse,
		float specularIntensity,
		vector<SceneObject *> &scene,
		glm::vec3 cameraPos);

	Mesh *mesh;
};

//  General-purpose sphere
//
class Sphere : public SceneObject
{
public:
	Sphere();
	Sphere(glm::vec3 p, float r, Material *material);

	void draw();
	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);

	glm::vec2 getUVCoords(glm::vec3 point);

	float radius = 1.0;
};

//  General-purpose plane
//
class Plane : public SceneObject
{
public:
	Plane();
	Plane(glm::vec3 p, glm::vec3 n, float w, float h, Material *material);

	void draw();
	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);

	glm::vec2 getUVCoords(glm::vec3 point);
	glm::vec3 getNormal(const glm::vec3 &p);

	ofPlanePrimitive plane;
	glm::vec3 normal;
	float width, height;
};

// View plane for render camera
//
class ViewPlane : public Plane
{
public:
	ViewPlane();
	ViewPlane(glm::vec2 p0, glm::vec2 p1);

	void draw();

	float getAspect();
	float getWidth();
	float getHeight();
	glm::vec2 getTopLeft();
	glm::vec2 getTopRight();
	glm::vec2 getBottomLeft();
	glm::vec2 getBottomRight();

	void setSize(glm::vec2 min, glm::vec2 max);

	glm::vec3 toWorld(float u, float v); // (u, v) --> (x, y, z) [ world space ]

	glm::vec2 min, max;
};

// Render camera - currently must be z axis aligned (we will improve this in project 4)
//
class RenderCam : public SceneObject
{
public:
	RenderCam();

	void draw();
	Ray getRay(float u, float v);

	glm::vec3 aim;
	ViewPlane view; // The camera viewplane, this is the view that we will render
};

// The openFrameworks application
//
class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// Helper functions for app setup
	//
	void setUpCameras(); 
	void setUpScene();	 
	void setUpGUI();	 

	// Helper functions for ray-tracing
	//
	void captureImage();
	void imageCaptureThreadRunner(int start_i, int end_i, int imageWidth, int imageHeight);
	ofColor getImagePixelColor(
		int i,
		int j,
		int imageWidth,
		int imageHeight
	);
	ofColor rayTrace(Ray ray, float reflectionLevels);
	ofColor calculateLight(
		const glm::vec3 &p,
		const glm::vec3 &normal,
		const ofColor diffuse,
		float specularIntensity
	);

	// Cameras
	ofEasyCam mainCam;
	ofCamera sideCam;
	ofCamera previewCam;
	ofCamera *theCam;

	RenderCam renderCam;
	ofImage image; // The rendered result

	// GUI
	ofxPanel gui;
	ofxToggle renderDraft;
	ofxIntSlider viewDistance;
	ofxIntSlider numThreads;			// number of threads to use for ray tracing
	ofxIntSlider samplingPower;		// how many additional samples per axis
	ofxIntSlider reflectionLevels;	// how many times can a ray reflect?

	// Mesh parser
	Parser parser;

	// Scene
	vector<SceneObject *> scene; // Stores SceneObjects
	vector<Light *> lights;		 // Stores Lights

	// Image size
	int originalImageWidth = 1920;
	int originalImageHeight = 1080;
	float draftScale = 4;
};