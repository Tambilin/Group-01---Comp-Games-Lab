/* Main Class handling the glut OpenGL functions - Thomas Linstead */
#include <iostream>
#include <string>		    // String Library
#include <sstream>		    // String Stream Library
#include "GLEW/glew.h"
#include "glut.h"
#include "md5load.h"
#include "soundeffect.h"
#include "objload.h"
using namespace std;

typedef float vec3_t[3];


//function declares
void keyboard (unsigned char key, int x, int y);
void reshape (int w, int h);
void cleanup();
void display();
void update();

GLvoid buildFont(GLvoid);
GLvoid KillFont(GLvoid);	
GLvoid printText(void *font, const char *fmt, int x, int y, float r, float g, float b);
GLvoid glPrint(string fmt);

//Global Variables
soundeffect * t = new soundeffect();
objload * Wings = new objload();
objload * Pallet = new objload();

md5load md5object;
md5load md5object1;
md5load md5object2;

int movement = 1;
int fps;
int width = 640, height = 480;

GLuint	base;		// Base Display List For The Font Set
HDC		hDC=NULL;   // Device context


int main()
{
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (width, height);
	glutCreateWindow ("3D Model Loader - Thomas Linstead");

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	cout << "Texture loaded correctly" << endl;

	//load the animated models
	md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/idle1.md5anim", "../Assets/Textures/pinky_s.tga");
	md5object1.init ("../Assets/Models/boblampclean.md5mesh" , "../Assets/Animations/boblampclean.md5anim", "../Assets/Textures/guard1_body.tga");
	md5object1.useModelShaderTextures("../Assets/Textures/");

	//Object loader
	Wings->InitGL();
	Wings->LoadModel("../Assets/Models/WingsOBJ.obj");
	Pallet->LoadModel("../Assets/Models/Pallet.obj");

	//load sounds
	t->createSound("../Assets/Sounds/inception.wav", 0);
	t->createSound("../Assets/Sounds/sound.wav", 1);
	t->createSound("../Assets/Sounds/lorry.wav", 2);

	atexit(cleanup);

	glutReshapeFunc (reshape);
	glutDisplayFunc (display);
	glutKeyboardFunc (keyboard);

	glutMainLoop ();

	// OpenGL init
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	
	// Face culling (for textures)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); 
	glEnable(GL_DEPTH_TEST);
	
	// Lighting
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	//wglUseFontBitmaps(hDC, 32, 96, base);
	//buildFont();

	return 0;
}

void update(void){
	if(movement < 90000)
		movement = movement + 1;
	else 
		movement = 1;

	t->toggleBackgroundSound(2, true);
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

	////////////////////////////////////////////
	/* Calculate The Frames-Per-Second (FPS) */
	time_t seconds;
	seconds = time (NULL);
	static int lastTime = time (NULL);
	static int framesDone = 0; 
	int currentTime = time (NULL);
	if(currentTime > lastTime) 
	{ 
		fps = framesDone; 
		framesDone = 0; 
		lastTime = currentTime; 
	} 
	framesDone++; 
	////////////////////////////////////////////


	/*GLUT_BITMAP_8_BY_13
GLUT_BITMAP_9_BY_15
GLUT_BITMAP_TIMES_ROMAN_10
GLUT_BITMAP_TIMES_ROMAN_24
GLUT_BITMAP_HELVETICA_10
GLUT_BITMAP_HELVETICA_12
GLUT_BITMAP_HELVETICA_18*/
	stringstream t;
	t << "Current FPS: " << fps;
	printText(GLUT_BITMAP_HELVETICA_18, t.str().c_str(), 20, height-30, 1, 0, 0);

	glPushMatrix();
	glTranslatef(0,5,42);
	glRotatef(180,0,1,0);
	glScalef(64,64,64);
	Wings->DrawModelUsingFixedFuncPipeline();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(55,0,30);
	glRotatef(((int)(movement))+55,0,1,1);
	glScalef(32,32,32);
	Pallet->DrawModelUsingFixedFuncPipeline();
	glPopMatrix();

	//enable client states for glDrawElements
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	md5object.enableTextured(true);
	md5object.enableSkeleton(false);
	md5object.enableRotate(false);
	md5object.draw(0.0, -35.0, -150.0, 0.5);

	md5object1.enableTextured(true);
	md5object1.enableSkeleton(false);
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

	width = w;
	height = h;

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
	if(key == 51){ //'2' Key{
		md5object.cleanup();
		md5object.init ("../Assets/Models/pinky.md5mesh" , "../Assets/Animations/attack.md5anim", "../Assets/Textures/pinky_d.tga");
	}
	if(key == 32){ //'Space' Key{
		t->play(0);
	}
	if(key == 46){ //'.' Key{
		t->play(1);
	}

}

GLvoid printText(void *font, const char *fmt, int x, int y, float r, float g, float b) {
 glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,width,0,height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(r,g,b);
  glRasterPos2f(x, y);
  const char *c;
  for (c=fmt; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
  glColor3f(1,1,1) ;
  glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, width/(GLdouble)height, 0.1, 1000.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
 glPopMatrix();
}