// Jalen Ng - 014390116

#include "ofApp.h"

void ofApp::setUpScene()
{
	// Scene objects
	//
	// For sake of simplicity, I'm using the diffuse texture as the specular texture as well
	Material *woodMat = new Material();
	woodMat->setDiffuse(ofImage("diffuse_textures/wood.jpg"));
	woodMat->setSpecular(8);
	woodMat->setTileFrequency(2, 2);
	Plane *woodWall = new Plane(glm::vec3(0.0, 5.5, -7.5), glm::vec3(0, 0, 1), 50, 50, woodMat);
	scene.push_back(woodWall);

	Material *waterMat = new Material();
	waterMat->setDiffuse(ofImage("diffuse_textures/water.jpg"));
	waterMat->setTileFrequency(4, 4);
	waterMat->setReflectivity(1.0);
	waterMat->setSpecular(1000);
	Plane *waterPlane = new Plane(glm::vec3(0.0, -2, 0.0), glm::vec3(0, 1, 0), 50, 50, waterMat);
	scene.push_back(waterPlane);

	Material *rockMat = new Material();
	rockMat->setDiffuse(ofImage("diffuse_textures/rocks_equirectangular.png"));
	rockMat->setSpecular(5);
	Sphere *rockBall = new Sphere(glm::vec3(0.0, -0.5, 5.0), 1.0f, rockMat);
	scene.push_back(rockBall);

	Material *salmonMirrorMat = new Material();
	salmonMirrorMat->setDiffuse(ofColor::salmon);
	salmonMirrorMat->setReflectivity(0.25);
	salmonMirrorMat->setSpecular(50);
	Sphere *salmonBall = new Sphere(glm::vec3(-8.0, 3.0, -1.0), 3.0f, salmonMirrorMat);
	scene.push_back(salmonBall);

	Material *noisyMirrorMat = new Material();
	noisyMirrorMat->setDiffuse(ofImage("diffuse_textures/mirror.png"));
	noisyMirrorMat->setReflectivity(1.0);
	noisyMirrorMat->setSpecular(500);
	Sphere *noisyMirrorBall = new Sphere(glm::vec3(-2.0, 4.5, -2.5), 3.0f, noisyMirrorMat);
	scene.push_back(noisyMirrorBall);

	Material *cyanMirrorMat = new Material();
	cyanMirrorMat->setDiffuse(ofColor::cyan);
	cyanMirrorMat->setReflectivity(0.5);
	cyanMirrorMat->setSpecular(250);
	Sphere *cyanMirrorBall = new Sphere(glm::vec3(4.0, 3.5, -0.5), 2.0f, cyanMirrorMat);
	scene.push_back(cyanMirrorBall);

	Material *blackMirrorMat = new Material();
	blackMirrorMat->setDiffuse(ofColor::black);
	blackMirrorMat->setReflectivity(1.0);
	blackMirrorMat->setSpecular(500);
	Sphere *blackMirrorBall = new Sphere(glm::vec3(6.0, 0.5, 4.0), 1.0f, blackMirrorMat);
	scene.push_back(blackMirrorBall);

	// Lights
	//
	/*Mesh *smallDiskMesh = parser.loadObj("./data/meshes/disk_small.obj");
	AreaLight *smallDiskLight = new AreaLight(glm::vec3(0, 15, 0), smallDiskMesh, 66.6666);
	lights.push_back(smallDiskLight);

	Mesh *diskMesh = parser.loadObj("./data/meshes/disk.obj");
	AreaLight *diskLight = new AreaLight(glm::vec3(0, 15, 0), diskMesh, 100.0);
	lights.push_back(diskLight);*/

	Light *pointLight = new Light(glm::vec3(0, 10, 5), 100);
	lights.push_back(pointLight);

	Light *pointLight2 = new Light(glm::vec3(10, -2, 10), 50);
	lights.push_back(pointLight2);
}

void ofApp::setUpCameras()
{
	// Set up background color
	ofSetBackgroundColor(ofColor::black);

	// Set up main camera
	mainCam.setDistance(25.0);
	mainCam.lookAt(glm::vec3(0, 0, 0));

	// Set up side camera
	sideCam.setPosition(glm::vec3(25, 0, 0));
	sideCam.lookAt(glm::vec3(0, 0, 0));

	// Set up preview camera
	previewCam.setPosition(renderCam.position);
	previewCam.lookAt(glm::vec3(0, 0, 0));

	// Set main camera as default camera
	theCam = &mainCam;
}

void ofApp::setUpGUI()
{
	gui.setup();
	gui.add(renderDraft.setup("Render draft", false));
	gui.add(numThreads.setup("Threads", 16, 1, 32));
	gui.add(viewDistance.setup("View distance", 50, 0, 200));
	gui.add(samplingPower.setup("Sampling power", 2, 1, 4));
	gui.add(reflectionLevels.setup("Reflection levels", 5, 0, 10));
}

//--------------------------------------------------------------
// Functions for ray-tracing algorithm
//--------------------------------------------------------------

void ofApp::captureImage()
{
	// Show message and capture starting time
	cout << "Ray-tracing on " << numThreads << " threads..." << endl;

	float startTime = ofGetSystemTimeMillis();

	int imageWidth = originalImageWidth;
	int imageHeight = originalImageHeight;

	if (renderDraft)
	{
		imageWidth /= draftScale;
		imageHeight /= draftScale;
	}

	// Allocate image
	image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);

	// Divide job into multiple threads
	vector<std::thread *> threads;

	int colsPerThread = imageWidth / numThreads;

	for (int i = 0; i < numThreads; i++)
	{
		int start_i = i * colsPerThread;
		int end_i = (i + 1) * colsPerThread;
		if (i >= numThreads - 1)
			end_i = imageWidth;

		thread *th = new thread(
			&ofApp::imageCaptureThreadRunner, 
			this, 
			start_i, end_i, 
			imageWidth, imageHeight);

		threads.push_back(th);
	}

	for (int i = 0; i < threads.size(); i++)
	{
		threads[i]->join();
		cout << "Thread " << (i + 1) << " of " << threads.size() << " finished" << endl;
	}

	// Construct output file name
	std::string filepath = "renders/final/";
	if (renderDraft)
		filepath = "renders/draft/";

	filepath += "render";

	filepath += "_aa";
	filepath += std::to_string(samplingPower*samplingPower) + "x";

	filepath += "_ref";
	filepath += std::to_string(reflectionLevels);

	filepath += "_vd";
	filepath += std::to_string(viewDistance);

	filepath += ".png";

	// Save image to filesystem
	image.save(filepath);

	// Log duration of image capture 		
	float endTime = ofGetSystemTimeMillis();
	float durationInSecs = (endTime - startTime) / 1000;
	cout << "Rendered " << filepath << " in " << durationInSecs << " seconds." << endl << endl;
}

void ofApp::imageCaptureThreadRunner(int start_i, int end_i, int imageWidth, int imageHeight)
{
	for (int i = start_i; i < end_i; i++)
	{
		for (int j = 0; j < imageHeight; j++)
		{
			ofColor color = getImagePixelColor(i, j, imageWidth, imageHeight);
			image.setColor(i, imageHeight - j - 1, color);
		}
	}
}

// Given i and j, return the color at the pixel of the image. Implements antialiasing.
//
ofColor ofApp::getImagePixelColor(int i, int j, int imageWidth, int imageHeight)
{
	// Get camera position
	glm::vec3 cameraPos = renderCam.position;

	glm::vec3 resultColorVec(0, 0, 0);

	// Distance between samples
	float increment = 1.0 / samplingPower;

	for (float x_offset = increment / 2; x_offset < 1.0; x_offset += increment)
	{
		for (float y_offset = increment / 2; y_offset < 1.0; y_offset += increment)
		{
			// Get ray from the pixel
			float u = (i + x_offset) / imageWidth;
			float v = (j + y_offset) / imageHeight;
			Ray ray = renderCam.getRay(u, v);

			// Ray trace to get color of the point intersected by the ray
			ofColor color = rayTrace(ray, reflectionLevels);

			// Add sample's contribution
			glm::vec3 colorVec(color.r, color.g, color.b);
			resultColorVec += colorVec;
		}
	}

	// Average the contributions
	resultColorVec /= (samplingPower * samplingPower);

	ofColor resultColor(resultColorVec[0], resultColorVec[1], resultColorVec[2]);
	return resultColor;

}

ofColor ofApp::rayTrace(Ray ray, float reflectionLevels) {

	// Variables to store current intersection point and normal
	// while iterating through scene objects
	glm::vec3 point;
	glm::vec3 normal;

	glm::vec3 closestPoint;
	glm::vec3 closestNormal;

	// Variables to store closest object and its distance
	float shortestDistance = std::numeric_limits<float>::max();
	SceneObject *closestObject;
	bool intersectionFound = false;

	// Iterate through each object in the scene to find the object closest to the camera
	for (int si = 0; si < scene.size(); si++)
	{
		SceneObject *sceneObject = scene[si];

		// If an intersection is detected
		if (sceneObject->intersect(ray, point, normal))
		{
			intersectionFound = true;

			float currentDistance = glm::length(point - ray.p);

			// Use the closest intersection point to get the nearest object.
			if (currentDistance <= shortestDistance)
			{
				shortestDistance = currentDistance;
				closestObject = sceneObject;
				closestPoint = point;
				closestNormal = normal;
			}
		}
	}

	// Start off with a color for if there is no intersection
	ofColor color = ofColor::black;

	// Atmospheric pressure
	float atmo = 1.0;

	// If there was an intersection with an object
	if (intersectionFound)
	{
		atmo = 1.0 - (shortestDistance / (float)viewDistance);
		atmo = ofClamp(atmo, 0.0, 1.0);

		// Get the diffuse, specular, and reflectivity
		ofColor diffuse = closestObject->sampleDiffuseColor(closestPoint);
		float specularPower = closestObject->sampleSpecularPower(closestPoint);
		ofColor specularColor = ofColor::white;
		float reflectivity = closestObject->material->reflectivity;

		// Add light contribution from the surface
		color = calculateLight(closestPoint, closestNormal, diffuse, specularPower);

		// If the material is reflective, perform recursive ray tracing
		if (reflectivity > 0 && reflectionLevels > 0)
		{
			// Create the ray from the reflection
			glm::vec3 reflectionDir = ray.d - 2 * glm::dot(ray.d, closestNormal) * closestNormal;
			glm::vec3 reflectionPoint = closestPoint + (closestNormal * 0.001); // minor offset to prevent intersection @ same point
			Ray reflection(reflectionPoint, reflectionDir);

			// Perform ray-tracing off that ray, and add the color contribution
			color += reflectivity * rayTrace(reflection, reflectionLevels - 1);
		}
	}

	return (atmo * color) + (1.0 - atmo) * ofColor::black;
}

ofColor ofApp::calculateLight(
	const glm::vec3 &p,
	const glm::vec3 &normal,
	const ofColor diffuse,
	float specularPower
	)
{
	// Start with ambient light color
	ofColor color(5, 5, 5);

	glm::vec3 normalizedCamVec = glm::normalize(renderCam.position - p);

	// Iterate through each light and calculate its lambert and phong contribution
	for (Light *light : lights)
		color += light->calculateLight(p, normal, diffuse, specularPower, scene, renderCam.position);

	return color;
}

//--------------------------------------------------------------
// Implemented functions inherited from ofBaseApp
//--------------------------------------------------------------

void ofApp::setup()
{
	ofSetWindowShape(originalImageWidth, originalImageHeight);

	setUpCameras();
	setUpScene();
	setUpGUI();
}

void ofApp::draw()
{
	// Start drawing in camera's POV
	ofEnableDepthTest();
	theCam->begin();

	// Draw each scene object in the scene
	for (int i = 0; i < scene.size(); i++)
		scene[i]->draw();

	// Draw each light proxy object in lights
	for (int i = 0; i < lights.size(); i++)
		lights[i]->draw();

	// Draw the render camera
	renderCam.draw();

	// End drawing in camera's POV
	theCam->end();
	ofDisableDepthTest();

	// Draw ray-traced image
	//
	image.update();
	int x = (ofGetWindowWidth() - originalImageWidth) / 2;
	int y = (ofGetWindowHeight() - originalImageHeight) / 2;
	image.draw(x, y, originalImageWidth, originalImageHeight);

	// Draw GUI
	//
	gui.draw();
}

void ofApp::keyPressed(int key)
{
	switch (key)
	{

	case 'm':
		theCam = &mainCam;
		cout << "Switched to main camera." << endl;
		break;

	case 'p':
		theCam = &previewCam;
		cout << "Switched to preview camera." << endl;
		break;

	case 's':
		theCam = &sideCam;
		cout << "Switched to side camera." << endl;
		break;

	case 'r':
		captureImage();
		break;

	case 'c':
		image.clear();
		cout << "Cleared ray-traced image." << endl;
		break;
	}
}

//--------------------------------------------------------------
// Unused functions inherited from ofBaseApp
//--------------------------------------------------------------

void ofApp::update() {}

void ofApp::keyReleased(int key) {}

void ofApp::mouseMoved(int x, int y) {}

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button) {}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::mouseEntered(int x, int y) {}

void ofApp::mouseExited(int x, int y) {}

void ofApp::windowResized(int w, int h) {}

void ofApp::gotMessage(ofMessage msg) {}

void ofApp::dragEvent(ofDragInfo dragInfo) {}
