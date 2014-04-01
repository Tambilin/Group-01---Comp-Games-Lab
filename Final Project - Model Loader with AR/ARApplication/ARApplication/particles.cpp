#include "particles.h"

const GLfloat particles::GRAVITY = -0.12f;
const GLfloat particles::YSPEED = 5.0f;
const GLfloat particles::MAXYSPEED = 5.0f;

particles::particles()
{
	srand(time(NULL));
	screen_width= 640;
	screen_height = 480;
	setup();
}

particles::particles(int w, int h)
{
	srand(time(NULL));
	screen_width = w;
	screen_height = h;
	setup();
}


particles::~particles()
{
}

void particles::setup(){
	// Pick an initial x location and  random x/y speeds
	float xLoc = (rand() / (float)RAND_MAX) * screen_width;
	float xSpeedMovement = -2.5 + (rand() / (float)RAND_MAX) * 5.0f;
	float ySpeedMovement = YSPEED + ((float)rand() / (float)RAND_MAX) * MAXYSPEED;

	// Set initial x/y location and speeds
	for (int loop = 0; loop < FIREWORK_PARTS; loop++)
	{
		x[loop] = xLoc;
		y[loop] = -10.0f; //bottom of the screen
		xSpeed[loop] = xSpeedMovement;
		ySpeed[loop] = ySpeedMovement;
	}

	//Assign a colour
	r = (rand() / (float)RAND_MAX);
	g = (rand() / (float)RAND_MAX);
	b = (rand() / (float)RAND_MAX);
	a = 1.0f;

	//Dont start until after a amount of time
	framesStart = ((int)rand() % 100);

	// Size of the particle (for glPointSize)
	particleSize = 1.0f + ((float)rand() / (float)RAND_MAX) * 2.0f;

	exploded = false;
}

void particles::move()
{
	for (int l = 0; l < FIREWORK_PARTS; l++)
	{
		// Once the firework is ready to launch start moving the particles
		if (framesStart <= 0)
		{
			x[l] += xSpeed[l];
			y[l] += ySpeed[l];
			ySpeed[l] += particles::GRAVITY;
		}
	}
	framesStart--;

	// Once a fireworks speed turns positive at top- explode!
	if (ySpeed[0] < 0.0f)
	{
		for (int j = 0; j < FIREWORK_PARTS; j++)
		{
			// Set a random x and y speed
			xSpeed[j] = -5 + (rand() / (float)RAND_MAX) * 10;
			ySpeed[j] = -5 + (rand() / (float)RAND_MAX) * 10;
		}
		exploded = true;
	}
}

void particles::explode()
{
	for (int i = 0; i < FIREWORK_PARTS; i++)
	{
		// Dampen the horizontal speed by 1% per frame
		xSpeed[i] *= 0.99;

		// Move the particle
		x[i] += xSpeed[i];
		y[i] += ySpeed[i];

		// Apply gravity to the particle's speed
		ySpeed[i] += particles::GRAVITY;
	}

	// Fade out the particles (alpha is stored per firework, not per particle)
	if (a > 0.0f)
	{
		a -= 0.05f;
	}
	else // Once the alpha hits zero reset the firework
	{
		setup();
	}
}