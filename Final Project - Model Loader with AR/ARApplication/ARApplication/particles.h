#pragma once

#include <iostream>
#include <ctime>
#include <time.h>		// Needed to use random numbers
#include "md5load.h";
#include "GLEW/glew.h"
#include <GL/gl.h>
#include <GL/glut.h>
using namespace std;


class particles
{
public:
	static const int FIREWORKS_COUNT = 15;           // Number of particle systems
	static const int FIREWORK_PARTS = 50;  // Number of particles per firework
	static const GLfloat GRAVITY;
	static const GLfloat YSPEED;
	static const GLfloat MAXYSPEED;

	int screen_width, screen_height;

	GLfloat x[FIREWORK_PARTS];
	GLfloat y[FIREWORK_PARTS];
	GLfloat xSpeed[FIREWORK_PARTS];
	GLfloat ySpeed[FIREWORK_PARTS];

	GLfloat r, g, b, a;
	GLfloat particleSize;

	GLint framesStart;
	GLboolean exploded;

	//Methods
	particles();
	particles(int w, int h);
	~particles();

	void setup();
	void move();
	void explode();
};

