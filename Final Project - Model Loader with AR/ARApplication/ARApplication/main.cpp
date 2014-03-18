#include "GLEW/glew.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <vector>
#include "md5load.h"
#include "menutextures.h"
#include "objload.h"
#include "soundeffect.h"

#include "object.c"


#define COLLIDE_DIST 30000.0

#define PI	3.14159265358979323846

//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

char            *model_name = "Data/object_data2";
ObjectData_T    *object;
int             objectnum;

int             width, height;
int             thresh = 100;
int             count2 = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

//char           *patt_name      = "Data/patt.hiro";
//int             patt_id;
//double          patt_width     = 80.0;
//double          patt_center[2] = {0.0, 0.0};
//double          patt_trans[3][4];

//Additional Variables
bool			loadOpenGL = false;
static bool     leftButtonDown = false;
int             c = 0;
bool            renderModel = false;
int				movement = 0;
int				fps = 0;
int				lastPlayedID = -1;
vector<int>		robotsDrawn;

//Class Objects
md5load RobotP1;
md5load RobotP2;
md5load Alien;

menutextures * Menu = new menutextures();
objload * Wings = new objload();
objload * objModel = new objload();
soundeffect * t = new soundeffect();

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static int    draw(ObjectData_T *object, int objectnum);
static int    draw_object(int obj_id, double gl_para[16]);
static void   loadData(void);
static void   keyboard(unsigned char key, int x, int y);
static void   mouse(int button, int state, int x, int y);
static float getMarkerDiffX(int m1, int m2);
static float getMarkerDiffY(int m1, int m2);
static float getAngleBetweenRobots(int m1, int m2);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	init();

    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
}

static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count2/arUtilTimer());
        cleanup();
        exit(0);
    }
}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i, j, k;

    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count2 == 0 ) arUtilTimerReset();
    count2++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	//glColor3f(1.0, 0.0, 0.0);
	//glLineWidth(6.0);

	/* detect the markers in the video frame */
	if (arDetectMarker(dataPtr, thresh,	&marker_info, &marker_num) < 0) {
		cleanup();
		exit(0);
	}
	for (i = 0; i < marker_num; i++) {
		argDrawSquare(marker_info[i].vertex, 0, 0);
	}

	/* check for known patterns */
	for (i = 0; i < objectnum; i++) {
		k = -1;
		for (j = 0; j < marker_num; j++) {
			if (object[i].id == marker_info[j].id) {

				/* you've found a pattern */
				//printf("Found pattern: %d ",patt_id);
				//glColor3f(0.0, 1.0, 0.0);
				argDrawSquare(marker_info[j].vertex, 0, 0);

				if (k == -1) k = j;
				else /* make sure you have the best pattern (highest confidence factor) */
					if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}
		if (k == -1) {
			object[i].visible = 0;
			continue;
		}

		/* calculate the transform for each marker */
		if (object[i].visible == 0) {
			arGetTransMat(&marker_info[k],
				object[i].marker_center, object[i].marker_width,
				object[i].trans);
		}
		else {
			arGetTransMatCont(&marker_info[k], object[i].trans,
				object[i].marker_center, object[i].marker_width,
				object[i].trans);
		}
		object[i].visible = 1;
	}

	//Get next video capture
    arVideoCapNext();

    draw(object, objectnum);

	//Swap buffers
	argSwapBuffers();
	//glutSwapBuffers();
	//glutPostRedisplay();
}

static void init( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ARParam  wparam;

	/* open the video path */
	if (arVideoOpen(vconf) < 0) exit(0);
	/* find the size of the window */
	if (arVideoInqSize(&width, &height) < 0) exit(0);
	printf("Image size (x,y) = (%d,%d)\n", width, height);

	/* set the initial camera parameters */
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("Camera parameter load error !!\n");
		exit(0);
	}
	arParamChangeSize(&wparam, width, height, &cparam);
	arInitCparam(&cparam);
	printf("*** Camera Parameter ***\n");
	arParamDisp(&cparam);

	/* load in the object data - trained markers and associated bitmap files */
	if ((object = read_ObjData(model_name, &objectnum)) == NULL) exit(0);
	printf("Objectfile num = %d\n", objectnum);

	/* open the graphics window */
	argInit(&cparam, 1.0, 0, 0, 0, 0);
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static int draw(ObjectData_T *object, int objectnum)
{
	int     i;
	double  gl_para[16];

	/*One Off Initialise Functions*/
	if (!loadOpenGL){
		loadData();
	}

	//Setup camera for 3D rendering
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );

	//Clear Depth Buffer
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	//glEnable(GL_LIGHTING);
    

	/* Setup Lighting */
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 25.0, 25.0, 50.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	/// Rendering Section ///
	glutSolidCube(20.0);

	//enable client states for glDrawElements
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	/* calculate the viewing parameters - gl_para */
	robotsDrawn.clear();
	for (i = 0; i < objectnum; i++) {
		if (object[i].visible == 0) continue;
		argConvGlpara(object[i].trans, gl_para);
		draw_object(object[i].id, gl_para);
		if (i < 4) {
			robotsDrawn.push_back(i);
		}
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	/// 2D Menu Rendering ///
	argDrawMode2D();

	//Render Menus
	glPushMatrix();
	Menu->render(width, height);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(250, 250, 0);
	objModel->DrawModelUsingFixedFuncPipeline();
	glPopMatrix();

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT1);

	//Update Rotations
	if (movement < 720)
		movement = movement + 1;
	else
		movement = 0;

	return 0;
}

/* draw the user object */
static int draw_object(int obj_id, double gl_para[16])
{
	GLfloat   mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat   mat_ambient_collide[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat   mat_flash[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat   mat_flash_collide[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat   mat_flash_shiny[] = { 50.0 };
	GLfloat   light_position[] = { 100.0, -200.0, 200.0, 0.0 };
	GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
	GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };

	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	//Clear Depth Buffer
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* set the material */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);

	if (Menu->getMode() != 0 && Menu->getMode() != 3 && gamestate::lastPlayedID != obj_id + 1 && gamestate::heroStats.first.id != obj_id + 1 && gamestate::heroStats.second.id != obj_id + 1){
		Menu->setConfirm(true);
		cout << "Detected New Card!" << endl;
		gamestate::lastPlayedID = obj_id + 1;
	}

	if (robotsDrawn.size() == 2) {
		float angle = getAngleBetweenRobots(robotsDrawn.at(0), robotsDrawn.at(1));
		if (obj_id == robotsDrawn.at(0)) {
			glPushMatrix();
				glRotatef(angle, 0, 1, 0);
				gamestate::cardlist[obj_id+1].drawModel();
			glPopMatrix();
		}
		else if (obj_id == robotsDrawn.at(1)) {
			glPushMatrix();
				glRotatef(-angle, 0, 1, 0);
				gamestate::cardlist[obj_id+1].drawModel();
			glPopMatrix();
		}
		else {
			gamestate::cardlist[obj_id+1].drawModel();
		}
	}
	else {
		gamestate::cardlist[obj_id+1].drawModel();
	}
	
	argDrawMode2D();

	return 0;
}

void loadData(){
	gamestate::init();
	//Load OBJ Models
	Wings->InitGL();
	Wings->LoadModel("../Assets/Models/coin.obj");
	objModel->LoadModel("../Assets/Models/dice.obj");
	//Load Menus
	Menu->load();
	//Load Sounds
	t->createSound("../Assets/Sounds/inception.wav", 0);
	t->createSound("../Assets/Sounds/sound.wav", 1);
	t->createSound("../Assets/Sounds/lorry.wav", 2);
	//Play Ambient Sounds
	t->toggleBackgroundSound(2, true);
	//Initialise Glut Functionality
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	loadOpenGL = true;
}

void keyboard(unsigned char key, int x, int y)
{
	/* Escape */
	if (key == 27)
		exit(0);
	if (key == 48){ //'0' Key{
		RobotP1.cleanup();
		RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk.md5anim", "../Assets/Textures/Head.tga");
	}
	if (key == 49){ //'1' Key{
		RobotP1.cleanup();
		RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Idle.md5anim", "../Assets/Textures/Head.tga");
	}
	if (key == 50){ //'2' Key{
		RobotP1.cleanup();
		RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Attack(DualSwords).md5anim", "../Assets/Textures/Head.tga");
	}
	if (key == 51){ //'3' Key{
		RobotP1.cleanup();
		RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Victory1.md5anim", "../Assets/Textures/Head.tga");
	}
	if (key == 53){ //'5' Key{
		if (robotsDrawn.size() == 2) {
			RobotP1.cleanup();
			RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk.md5anim", "../Assets/Textures/Head.tga");
			float distX = getMarkerDiffX(robotsDrawn.at(0), robotsDrawn.at(1));
			float distY = getMarkerDiffY(robotsDrawn.at(0), robotsDrawn.at(1));
			float stepX = distX / 100;
			float stepY = distY / 100;
			for (int i = 0; i < 100; i++) {
				glPushMatrix();
					glTranslatef(stepX, stepY, 0);
				glPopMatrix();
			}
			RobotP1.cleanup();
			RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Attack(DualSwords).md5anim", "../Assets/Textures/Head.tga");
			//Need to find a way to wait here until attack animation is finished
			RobotP1.cleanup();
			RobotP1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk.md5anim", "../Assets/Textures/Head.tga");
			for (int i = 0; i < 100; i++) {
				glPushMatrix();
				glTranslatef(-stepX, -stepY, 0);
				glPopMatrix();
			}
		}
	}

	if (key == 32){ //'Space' Key{
		t->play(0);
	}
	if (key == 46){ //'.' Key{
		t->play(1);
	}
}

void mouse(int button, int state, int x, int y)
{
	// Respond to mouse button presses.
	// If button1 pressed, mark this state so we know in motion function.
	if (button == GLUT_LEFT_BUTTON)
	{
		cout << "CLICK" << endl;
		leftButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE;
		if (leftButtonDown == true){
			Menu->checkButtonClick(x, y, width, height);
		}
	}
}

static float getMarkerDiffX(int m1, int m2) {
	return object[m2].marker_center[0] - object[m1].marker_center[0] - (object[m1].marker_width * 2) - (object[m2].marker_width * 2);
}

static float getMarkerDiffY(int m1, int m2) {
	return object[m2].marker_center[1] - object[m1].marker_center[1] - (object[m1].marker_width * 2) - (object[m2].marker_width * 2);
}

static float getAngleBetweenRobots(int m1, int m2) {
	return atan2(getMarkerDiffX(m1, m2), getMarkerDiffY(m1, m2)) * 180 / PI;
}