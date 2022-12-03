#pragma once

#include <iostream>
#include <string>
#include <gl/glut.h>

#include "PPMImage.h";
#include "Vector3D.h";

using namespace std;

class Billboard
{
public:
	Billboard(void);
	~Billboard(void);
	void ReadFile(string fileName);
	void SetSize(float width, float height);
	void SetLocation(Vector3 location);
	void SetOrientation(float orientation);
	void Draw();

private:
	PPMImage textureImage;
	float width, height;
	// The variable location contains 3 variables: x, y, z, where y is really the elevation of the board.
	Vector3 location;
	float orientation;
	GLuint textureNumber;
};

