/* Main Class handling the glut OpenGL functions - Thomas Linstead */
#include <iostream>
#include "GLEW/glew.h"
#include "glut.h"
#include "md5load.h"
#include "soundeffect.h"

typedef float vec3_t[3];

//function declares
void keyboard (unsigned char key, int x, int y);
void reshape (int w, int h);
void cleanup();
void display();
void update();
using namespace std;

//Global Variables
//md5load *md5object  = new md5load();
soundeffect * t = new soundeffect();
md5load md5object;
md5load md5object1;
md5load md5object2;
int movement = 0;


int main()
{
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	glutCreateWindow ("3D Model Loader - Thomas Linstead");

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	cout << "Texture loaded correctly" << endl;

	//load the models
	md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/idle1.md5anim", "../Assets/Textures/pinky_s.tga");
	md5object1.init ("../Assets/Models/boblampclean.md5mesh" , "../Assets/Animations/boblampclean.md5anim", "../Assets/Textures/guard1_body.tga");
	md5object1.useModelShaderTextures("../Assets/Textures/");

	//load sounds
	t->createSound("../Assets/Sounds/inception.wav", 0);
	t->createSound("../Assets/Sounds/sound.wav", 1);
	t->createSound("../Assets/Sounds/lorry.wav", 2);

	atexit(cleanup);

	glutReshapeFunc (reshape);
	glutDisplayFunc (display);
	glutKeyboardFunc (keyboard);

	glutMainLoop ();

	return 0;
}

void update(void){
	if(movement < 100)
		movement = movement + 0.01;
	else 
		movement = 0;
}

void cleanup ()
{
	md5object.cleanup();
	md5object1.cleanup();
	md5object2.cleanup();
}

void display()
{ 
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable client states for glDrawElements
	glTranslatef(0,1500,0);
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	md5object.enableTextured(true);
	md5object.enableSkeleton(true);
	md5object.enableRotate(false);
	md5object.draw(0.0+movement, -35.0, -150.0, 0.5);

	md5object1.enableTextured(true);
	md5object1.enableSkeleton(true);
	md5object1.draw(-25.0, 0.0, -150.0, 1.0);

	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);

	glutSwapBuffers ();
	glutPostRedisplay ();
	update();
}


/**
* Check if an animation can be used for a given model.  Model's
* skeleton and animation's skeleton must match.
*/
int
	CheckAnimValidity (const struct md5_model_t *mdl,
	const struct md5_anim_t *anim)
{
	int i;

	/* md5mesh and md5anim must have the same number of joints */
	if (mdl->num_joints != anim->num_joints)
		return 0;

	/* We just check with frame[0] */
	for (i = 0; i < mdl->num_joints; ++i)
	{
		/* Joints must have the same parent index */
		if (mdl->baseSkel[i].parent != anim->skelFrames[0][i].parent)
			return 0;

		/* Joints must have the same name */
		if (strcmp (mdl->baseSkel[i].name, anim->skelFrames[0][i].name) != 0)
			return 0;
	}

	return 1;
}

void reshape (int w, int h)
{
	if (h == 0)
		h = 1;

	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void keyboard (unsigned char key, int x, int y)
{
	/* Escape */
	if (key == 27)
		exit (0);
	if(key == 48){ //'0' Key{
		md5object.cleanup();
		md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/run.md5anim", "../Assets/Textures/pinky_d.tga");
	}
	if(key == 49){ //'1' Key{
		md5object.cleanup();
		md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/run.md5anim", "../Assets/Textures/pinky_s.tga");
	}
	if(key == 50){ //'2' Key{
		md5object.cleanup();
		md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/idle1.md5anim", "../Assets/Textures/pinky_s.tga");
	}
	if(key == 32){ //'Space' Key{
		t->play(0);
	}
}