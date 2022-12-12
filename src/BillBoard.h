#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <GL/glut.h>

#include "PPMImage.h"
#include "Vector3D.h"

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
	bool IsAnimated();
	GLuint ActiveTextureNumber();
	void SetDelayMs(int delay_ms);

private:
	PPMImage textureImage;
	float width, height;
	// The variable location contains 3 variables: x, y, z, where y is really the elevation of the board.
	Vector3 location;
	float orientation;
	GLuint textureNumber;
	std::vector<GLuint> textureNumbers; // Used when we have an animated sign
	int delay_ms; // used for rotating files when ReadFile() is given a directory.

	// Optimization that lets us pretend we're a dynamic, expensive object
	// but actually re-use gl lists so we only need to by expensive once.
	map<GLuint, int> gl_texture_num_to_list_ids;
};

