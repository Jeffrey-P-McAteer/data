/// \file
/// This is the main file for the project Traffic Simulation.
/// \author Yuzhong Shen, Jeffrey McAteer
/// \version 1.0
/// \date 12/10/2017, 11-10-2022
///

#include <iostream>
#include <sstream>
#include <string>

#if __cplusplus < 201703L
	#error "Must compile using C++17 or better!"
#endif

#include <filesystem>


#include <GL/glew.h>
#include <GL/glut.h>

#include "ObjModel.h"
#include "TrafficLight.h"
#include "utility.h"
#include "BillBoard.h"

using namespace std;

int counter = 0; ///< Counter for the traffic signal.  Only one is needed.
int updateInterval = 20; ///< Update interval for the update function in miliseconds.

ObjModel car;
ObjModel surveillanceCamera;
TrafficLight trafficLight;
Billboard billboard_a;

ObjModel bench01;

int carID; ///< Display List ID for car
int surveillanceCameraID; ///< Display list ID for surveillance camera
int terrainID; ///< Display list ID for terrain
int raised_terrainID; // Bugfix for main view render glitch

int bench01ID;

Signal NS_Signal = Green;  ///< North-South signal.
Signal WE_Signal = Red;  ///< West-East signal.

Vector3 carPosition = { 3, 0, 45 }; ///< Car position with initial value.
Vector3 localCarSpeed = { 0, 0, 0 }; ///< Car speed in car's local space.
Vector3 worldCarSpeed; ///< Car speed in world space.

float carDirection = 180;  ///< Car direction
string carHeading = "N"; ///< String for car heading

Vector3	localCameraOffset = { 0, 0, -6 };  ///< Third person camera offset in the car's local space.
Vector3 worldCameraOffset;  ///< Third person camera offset in world space.

int winWidth; ///< Width of OpenGL window
int winHeight; ///< Height of OpenGL window
int sWidth; ///< Width of the small viewport
int sHeight; ///< Height of the small viewport

/// Update the small viewports' size automatically.
/// \param w Width of the OpenGL window
/// \param h Height of the OpenGL window
void reshape(int w, int h)
{
	winWidth = w, winHeight = h;

	// Update sWidth and sHeight here.
	sWidth = (winWidth / 3) - 30;
	sHeight = (winHeight / 4);
}

/// Callback function for special keys.
/// \param key ASCII code of the key pressed.
/// \param x X coordinate of the mouse cursor when the key is pressed.
/// \param y Y coordinate of the mouse cursor when the key is pressed.
void specialKey(int key, int x, int y)
{
	std::cout << "specialKey(" << key << ", " << x << ", " << y << ")" << std::endl;
	if (key == GLUT_KEY_LEFT)
	{
		// Update car direction
		carDirection += 10;
		while (carDirection > 360) {
			carDirection -= 360;
		}
		while (carDirection < 0) {
			carDirection += 360;
		}

		// Update the third person camera offset in the world frame.
		// TODO localCameraOffset

		// Compute the car heading.
		carHeading = heading_to_string(carDirection);

		/// Test comment

	}
	if (key == GLUT_KEY_RIGHT)
	{
		// Handle the right turns.
		carDirection -= 10;
		while (carDirection < 0) {
			carDirection += 360;
		}
		while (carDirection > 360) {
			carDirection -= 360;
		}

		carHeading = heading_to_string(carDirection);


	}
	if (key == GLUT_KEY_UP)
	{
		// acceleration
		if (localCarSpeed.y < 0.5) {
			localCarSpeed.y += 0.65;
		}
		else {
			localCarSpeed.y *= 1.18;
		}
		
	}

	if (key == GLUT_KEY_DOWN)
	{
		// deceleration
		if (localCarSpeed.y > -0.5) {
			localCarSpeed.y -= 0.65;
		}
		else {
			localCarSpeed.y *= 1.18;
		}

	}
}

/// Function to draw the entire scene (in GL_MODELVIEW mode)
void drawScene()
{
	// Draw terrain
	glCallList(terrainID);

	glEnable(GL_LIGHTING);

	// North-East (NS_Signal)
	glPushMatrix();
		glTranslatef(10, 0, -10.5);
		glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
		trafficLight.setSignal(NS_Signal);
		trafficLight.Draw();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(10, 0, -10);
		glRotatef(-45, 0, 1, 0);
		glCallList(surveillanceCameraID);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(20, 0, -20);
		glScalef(0.002, 0.002, 0.002); // Original model was huge
		//glRotatef(-45, 0, 1, 0);
		glCallList(bench01ID);
	glPopMatrix();

	// South-West (NS_Signal)
	glPushMatrix();
		glTranslatef(-10, 0, 10.5);
		glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
		glRotatef(180, 0, 1, 0);
		trafficLight.setSignal(NS_Signal);
		trafficLight.Draw();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-10, 0, 10);
		glRotatef(180, 0, 1, 0);
		glCallList(surveillanceCameraID);
	glPopMatrix();
	
	// South-East (WE_Signal)
	glPushMatrix();
		glTranslatef(10, 0, 10.5);
		glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
		glRotatef(270, 0, 1, 0);
		trafficLight.setSignal(WE_Signal);
		trafficLight.Draw();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(10, 0, 10);
		glRotatef(270, 0, 1, 0);
		glCallList(surveillanceCameraID);
	glPopMatrix();

	// North-West (WE_Signal)
	glPushMatrix();
		glTranslatef(-10, 0, -10.5);
		glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
		glRotatef(90, 0, 1, 0);
		trafficLight.setSignal(WE_Signal);
		trafficLight.Draw();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-10, 0, -10);
		glRotatef(90, 0, 1, 0);
		glCallList(surveillanceCameraID);
	glPopMatrix();

	// Draw all billboard objects
	glPushMatrix();
		billboard_a.Draw();
	glPopMatrix();


	// Draw the car.
	glPushMatrix();
		float center_offset = 3.0f; // no car.GetCenter().z? Different coordinate system?
		glTranslatef(carPosition.x, carPosition.y, carPosition.z - center_offset);
		glRotatef(carDirection, 0, 1, 0);

		glTranslatef(0.0f, 0.0f, center_offset);
		//glScalef(1.0/26.0f, 1.0/26.0f, 1.0/26.0f);
		glScalef(1.25f, 1.25f, 1.25f);
		
		//car.Draw();
		glCallList(carID);


	glPopMatrix();


	// Some bug above means we need to re-draw the terrain 0.5 units above to fix the glitch.
	glCallList(raised_terrainID);


}


void draw_terrain(float y_offset) {
	// Grass
	glColor3f(0, 0.7, 0);
	glBegin(GL_QUADS);
		glVertex3f(-1000, 0 + y_offset, 1000);
		glVertex3f(-10, 0 + y_offset, 1000);
		glVertex3f(-10, 0 + y_offset, 10);
		glVertex3f(-1000, 0 + y_offset, 10);

		glVertex3f(10, 0 + y_offset, 1000);
		glVertex3f(1000, 0 + y_offset, 1000);
		glVertex3f(1000, 0 + y_offset, 10);
		glVertex3f(10, 0 + y_offset, 10);

		glVertex3f(10, 0 + y_offset, -10);
		glVertex3f(1000, 0 + y_offset, -10);
		glVertex3f(1000, 0 + y_offset, -1000);
		glVertex3f(10, 0 + y_offset, -1000);

		glVertex3f(-1000, 0 + y_offset, -10);
		glVertex3f(-10, 0 + y_offset, -10);
		glVertex3f(-10, 0 + y_offset, -1000);
		glVertex3f(-1000, 0 + y_offset, -1000);
	glEnd();

	// Roads
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.2);

		glVertex3f(-10, 0 + y_offset, 1000);
		glVertex3f(10, 0 + y_offset, 1000);
		glVertex3f(10, 0 + y_offset, -1000);
		glVertex3f(-10, 0 + y_offset, -1000);

		glVertex3f(-1000, 0 + y_offset, 10);
		glVertex3f(1000, 0 + y_offset, 10);
		glVertex3f(1000, 0 + y_offset, -10);
		glVertex3f(-1000, 0 + y_offset, -10);
	glEnd();

	// Yellow line
	// glBegin(GL_POLYGON);
	// 	glColor3f(1, 1, 0);
	// 	glVertex3f(-0.1, 0.05, 1000);
	// 	glVertex3f(0.1, 0.05, 1000);
	// 	glVertex3f(0.1, 0.05, -1000);
	// 	glVertex3f(-0.1, 0.05, -1000);
	// glEnd();

	// Stripes are white
	glColor3f(1, 1, 1);
	int inner_box_size = 16;

	int stripe_len = 9;
	int stripe_stride = 20;
	for (int stripe_begin=-1000; stripe_begin < 1000; stripe_begin += stripe_stride) {
		if (stripe_begin >= -inner_box_size && stripe_begin <= inner_box_size) {
			continue; // No stripes in center line
		}
		// Draw a stripe north-south
		glBegin(GL_POLYGON);
			glVertex3f(-0.1, 0.05 + y_offset, stripe_begin);
			glVertex3f(0.1, 0.05 + y_offset, stripe_begin);
			glVertex3f(0.1, 0.05 + y_offset, stripe_begin-stripe_len);
			glVertex3f(-0.1, 0.05 + y_offset, stripe_begin-stripe_len);
		glEnd();
		// Draw a stripe east-west
		glBegin(GL_POLYGON);
			glVertex3f(stripe_begin, 0.05 + y_offset, -0.1);
			glVertex3f(stripe_begin, 0.05 + y_offset, 0.1);
			glVertex3f(stripe_begin-stripe_len, 0.05 + y_offset, 0.1);
			glVertex3f(stripe_begin-stripe_len, 0.05 + y_offset, -0.1);
		glEnd();

	}

	// Draw 4 white blocks for stop bars at the lights
	int bar_height = 3;
	for (auto rotation : {0, 90, 180, 270}) {
			glRotatef(rotation, 0, 1, 0);
			glBegin(GL_POLYGON);
				glVertex3f(   0, 0.05 + y_offset, 12);
				glVertex3f(   9, 0.05 + y_offset, 12);
				glVertex3f(   9, 0.05 + y_offset, 12-bar_height);
				glVertex3f(   0, 0.05 + y_offset, 12-bar_height);
			glEnd();
	}
	// Set rotation back after using it to avoid calculating stop bar line coordinates
	glRotatef(180, 0, 1, 0);

}


/// Initialization.
/// Set up lighting, generate display lists for the surveillance camera, 
/// car, and terrain.
void init()
{
	glClearColor(0.5, 0.5, 1.0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Note that the light is defined in the eye or camera frame.
	GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};

	GLfloat ambient[] = {0.3, 0.3, 0.3, 1};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);	
	glEnable(GL_LIGHT0);

	// Generate display list for the surveillance camera.
	surveillanceCameraID = glGenLists(1);
	
	glNewList(surveillanceCameraID, GL_COMPILE);
	surveillanceCamera.Draw();
	glEndList();

	// Generate display list for the car.
	carID = glGenLists(1);
	glNewList(carID, GL_COMPILE);
	car.Draw();
	glEndList();

	// Generate display list for the bench01.
	bench01ID = glGenLists(1);
	glNewList(bench01ID, GL_COMPILE);
	bench01.Draw();
	glEndList();

	// Generate the display list for terrain, including road and grass.
	terrainID = glGenLists(1);
	glNewList(terrainID, GL_COMPILE);
	glDisable(GL_LIGHTING);
	draw_terrain(0.0);
	glEndList();

	raised_terrainID = glGenLists(1);
	glNewList(raised_terrainID, GL_COMPILE);
	glDisable(GL_LIGHTING);
	draw_terrain(0.02);
	glEndList();


}



/// Display callback.
/// Displays 4 viewports.  For for each viewport, set up position and size, projection, 
/// and camera (ModelView matrix).
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Generate head-up display (HUD)
	stringstream ss;
	ss << "Heading: " << heading_to_string(carDirection);
	ss << "    Speed: " << localCarSpeed.y << " m/h";

	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glViewport(0, 0, winWidth, winHeight);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, winWidth, 0.0, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	    glLoadIdentity();

	    glColor3f(1.0, 1.0, 0.0); // yellow

	    glRasterPos2i( (winWidth / 2) - 100, (winHeight - sHeight) - 25 );

			printLargeString(ss.str());

			// for (auto& c : ss.str()) {
	    //   glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	    // }

		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  glEnable(GL_TEXTURE_2D);
  glPopMatrix();

	// Setup viewport, projection, and camera for the main view.
	glPushMatrix();
		glLoadIdentity();
		glViewport(0, 0, winWidth, winHeight - sHeight - 50);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30, (float) winWidth / (winHeight - sHeight - 50), 1, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Update the code here for the correct third person camera that moves with the car.
		// gluLookAt(
		// 	carPosition.x, carPosition.y + 2, carPosition.z + 5, // Eye location
		// 	carPosition.x, carPosition.y + 1.5, carPosition.z, // Center location
		// 	0, 1, 0 // Up vector
		// );

		Vector3 eyePosition = { carPosition.x, carPosition.y, carPosition.z - 3.0f };
		Vector3 eyeFocus = { carPosition.x, carPosition.y, carPosition.z - 3.0f };

		//eyePosition.y += 2.0f * sin(deg_to_rad(carDirection));
		float camera_circle_radius = 6.5f;
		float camera_height_offset = 2.80f;
		float eye_x_offset = camera_circle_radius * sin(deg_to_rad(carDirection));
		float eye_z_offset = camera_circle_radius * cos(deg_to_rad(carDirection));

		eyePosition.y += camera_height_offset;
		eyePosition.x -= eye_x_offset;
		eyePosition.z -= eye_z_offset;

		eyeFocus.y += camera_height_offset - 0.5; // look down a bit
		eyeFocus.x += eye_x_offset;
		eyeFocus.z += eye_z_offset;

		gluLookAt(
			eyePosition.x, eyePosition.y, eyePosition.z, // Eye location
			eyeFocus.x, eyeFocus.y, eyeFocus.z, // Center location
			0, 1, 0 // Up vector
		);

		drawScene();
	glPopMatrix();

	// Setup viewport, projection, and camera for the South-East camera and draw the scene again.

	glPushMatrix();
		glLoadIdentity();
		glViewport(
			10, winHeight - sHeight,
			sWidth, sHeight
		);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30, (float) sWidth / (float) sHeight, 2, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(
			carPosition.x + 10.0, carPosition.y + 2.0, carPosition.z + 10.0, // Eye location
			eyeFocus.x, eyeFocus.y, eyeFocus.z, // Center location
			0, 1, 0 // up vector
		);

		drawScene();
	glPopMatrix();


	// Setup the viewport, projection, camera for the top view and draw the scene again.
	glPushMatrix();
		glLoadIdentity();
		glViewport(
			(winWidth/2) - (sWidth/2), winHeight - sHeight,
			sWidth, sHeight
		);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30, (float) sWidth / (float) sHeight, 2, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(
			eyePosition.x, eyePosition.y + 70.0, eyePosition.z, // Eye location
			eyeFocus.x, eyeFocus.y, eyeFocus.z, // Center location
			1.0f * sin(deg_to_rad(carDirection)), 0, 1.0f * cos(deg_to_rad(carDirection)) // Up vector (direction of travel)
			//0, 0, 1 // up vector
		);

		drawScene();
	glPopMatrix();

	
	// Setup viewport, projection, camera for the South-West camera and draw the scene again.
	glPushMatrix();
		glLoadIdentity();
		glViewport(
			(winWidth-sWidth)-10, winHeight - sHeight,
			sWidth, sHeight
		);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30, (float) sWidth / (float) sHeight, 2, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(
			carPosition.x - 10.0, carPosition.y + 2.0, carPosition.z + 10.0, // Eye location
			eyeFocus.x, eyeFocus.y, eyeFocus.z, // Center location
			0, 1, 0 // up vector
		);

		drawScene();
	glPopMatrix();

	glutSwapBuffers();
	glFlush();
}

/// Keyboard callback
/// Handle regular key presses, and for P2, "r" for reset, "b" for break, and escape for quit.
void keyboard(unsigned char key, int x, int y)
{
	std::cout << "keyboard(" << key << ", " << x << ", " << y << ")" << std::endl;
	switch(key)
	{
	case 'r':
		// Add code for reset
		carPosition = { 3, 0, 45 };
		localCarSpeed = { 0, 0, 0 };
		carDirection = 180;
		carHeading = "N";

		break;

	case 'b':
		// Add code for breaking.
		localCarSpeed.y = 0;

		// localCarSpeed.x *= 0.5f;
		// if (std::abs(localCarSpeed.x) < 0.01f) {
		// 	localCarSpeed.x = 0.0f;
		// }
		// localCarSpeed.y *= 0.5f;
		// if (std::abs(localCarSpeed.y) < 0.01f) {
		// 	localCarSpeed.y = 0.0f;
		// }
		// localCarSpeed.z *= 0.5f;
		// if (std::abs(localCarSpeed.z) < 0.01f) {
		// 	localCarSpeed.z = 0.0f;
		// }

		break;
	
	// Forward WASD as arrow key events
	case 'w':
		specialKey(GLUT_KEY_UP, x, y);
		break;
	case 'a':
		specialKey(GLUT_KEY_LEFT, x, y);
		break;
	case 's':
		specialKey(GLUT_KEY_DOWN, x, y);
		break;
	case 'd':
		specialKey(GLUT_KEY_RIGHT, x, y);
		break;

	case 'q':
	case 27:
		exit(0);
		break;
	}

	glutPostRedisplay();
}

/// Updates the dynamic objects.
/// Update the car position and traffic signals.
void update()
{
	// Compute worldCarSpeed off localCarSpeed + carDirection
	worldCarSpeed.x = localCarSpeed.y * sin(deg_to_rad(carDirection));
	worldCarSpeed.z = localCarSpeed.y * cos(deg_to_rad(carDirection));
	worldCarSpeed.y = 0.0f;

	// Update car position.
	carPosition.x += worldCarSpeed.x * ((double) updateInterval / 1000.0);
	carPosition.y += worldCarSpeed.y * ((double) updateInterval / 1000.0);
	carPosition.z += worldCarSpeed.z * ((double) updateInterval / 1000.0);

	// State machine for the traffic signals using three variables: NS_Signal, WE_Signal, and counter.
	counter += 1;
	bool second_mark = ((counter * updateInterval) % 1000) <= updateInterval;
	if (second_mark) {
		int state_seconds = ( (counter * updateInterval) / 1000) % 12;
		// ^^ TrafficLight State only has 12 seconds we care about.
		if (state_seconds < 6) {
			NS_Signal = Green;
			WE_Signal = Red;
		}
		else if (state_seconds < 7) {
			NS_Signal = Yellow;
			WE_Signal = Red;
		}
		else if (state_seconds < 11) {
			NS_Signal = Red;
			WE_Signal = Green;
		}
		else {
			NS_Signal = Red;
			WE_Signal = Yellow;
		}
		// States circle back
	}

}

/// Set the interval between updates.
/// \param miliseconds is the number of miliseconds passed before this function is called.  It is the third
/// parameter of glutTimerFunc().
void timer(int miliseconds)
{
	update();
	glutTimerFunc(updateInterval, timer, updateInterval);	
	glutPostRedisplay();
}

/// Main function
/// GLUT initialization, load 3D models, and register GLUT callbacks.
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	winWidth = 1300, winHeight = 800;
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Traffic Simulation");
	glewInit();

	// Search for "Models" directory near CWD
	std::string models_directory;
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(".")) {
    auto dirEntry_path = std::filesystem::path(dirEntry);
    if (dirEntry_path.filename() == "TrafficLight.obj") {
      models_directory = dirEntry_path.parent_path();
      break;
    }
  }
  std::cout << "Found Model directory at " << models_directory << std::endl;

	// Load the 3D models.
	trafficLight.ReadFile(models_directory+"/TrafficLight.obj");
	//car.ReadFile(models_directory+"/Honda_S2000_inch.obj");
	car.ReadFile(models_directory+"/taxi.obj");

	surveillanceCamera.ReadFile(models_directory+"/camera.obj");

	// Define billboard object details
	billboard_a.ReadFile("");
	//billboard_a.SetSize();
	billboard_a.SetLocation({0.0f, 0.0f, 0.0f});
	billboard_a.SetOrientation(0.0f);

	// Bonus models
	// https://www.cgtrader.com/free-3d-models/plant/conifer/2-diffrent-tree-kousa-dogwood
	bench01.ReadFile(models_directory+"/Bench.obj");

	std::cout << "before init()" << std::endl;
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutTimerFunc(0, timer, updateInterval);
	std::cout << "before glutMainLoop()" << std::endl;
	glutMainLoop();

	system("pause");
}