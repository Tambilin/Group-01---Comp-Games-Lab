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

//Additional Variables
bool			loadOpenGL = false;
static bool     leftButtonDown = false;
int             c = 0;
bool            renderModel = false;
int				movement = 0;
int				fps = 0;
int				lastPlayedID = -1;
vector<int>		robotsDrawn;
int				robotMode1 = 0; //0 for idle, 1 for going towards enemy, 2 for going away from enemy, 3 for attacking enemy
int				robotMode2 = 0; //0 for idle, 1 for going towards enemy, 2 for going away from enemy, 3 for attacking enemy
float			distX = 0;
float			distY = 0;
float			distZ = 0;
float			stepX = 0;
float			stepY = 0;
float			stepZ = 0;
int				numSteps = 0;
int				currentStep = 0;
// declaration of players positions array
int				mech1Position[6];
int				mech2Position[6];

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
static void   reshape(int w, int h);
static float  getMarkerDiffX();
static float  getMarkerDiffY();
static float  getMarkerDiffZ();
static float  getAngleBetweenRobots();

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
		//argDrawSquare(marker_info[i].vertex, 0, 0);
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
	if ((object = read_ObjData(model_name, &objectnum)) == NULL) cout << "ERROR LOADING FILE DATA";
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
	/*GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 25.0, 25.0, 50.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);*/
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
	
	//Precompute the positioning data
	robotsDrawn.clear();
	for (i = 0; i < objectnum; i++) {
		if (object[i].visible == 0) continue;
		//cout << i << " : " << gamestate::heroStats.first.id << endl;
		if (i+1 == gamestate::heroStats.first.id){
			//cout << "Setup first Mech" << endl;
			argConvGlpara(object[i].trans, gl_para);
			mech1Position[0] = gl_para[12];//X Position
			mech1Position[1] = gl_para[13];//Y Position
			mech1Position[2] = gl_para[14];//Z Position
			if (gl_para[0] != 0.0f || gl_para[1] != 0.0f) {
				const float alignment_x = atan2(-gl_para[1], gl_para[0]);
				float c2;
				if (cosf(alignment_x) != 0) {
					c2 = gl_para[0] / cosf(alignment_x);
				}
				else {
					c2 = gl_para[1] / -sinf(alignment_x);
				}
				const float alignment_y = atan2(gl_para[2], c2);
				const float alignment_z = atan2(-gl_para[6], gl_para[10]);
				mech1Position[3] = alignment_x*57.2957795;//X Rotation (Degrees)
				mech1Position[4] = alignment_y*57.2957795;//Y Rotation (Degrees)
				mech1Position[5] = alignment_z*57.2957795;//Z Rotation (Degrees)
			}
		}
		else if (i+1 == gamestate::heroStats.second.id){
			//cout << "Setup second Mech" << endl;
			argConvGlpara(object[i].trans, gl_para);
			mech2Position[0] = gl_para[12];//X Position
			mech2Position[1] = gl_para[13];//Y Position
			mech2Position[2] = gl_para[14];//Z Position
			if (gl_para[0] != 0.0f || gl_para[1] != 0.0f) {
				const float alignment_x = atan2(-gl_para[1], gl_para[0]);
				float c2;
				if (0 != cosf(alignment_x)) {
					c2 = gl_para[0] / cosf(alignment_x);
				}
				else {
					c2 = gl_para[1] / -sinf(alignment_x);
				}
				const float alignment_y = atan2(gl_para[2], c2);
				const float alignment_z = atan2(-gl_para[6], gl_para[10]);
				mech2Position[3] = alignment_x*57.2957795;//X Rotation (Degrees)
				mech2Position[4] = alignment_y*57.2957795;//Y Rotation (Degrees)
				mech2Position[5] = alignment_z*57.2957795;//Z Rotation (Degrees)
			}
		}
		robotsDrawn.push_back(i);
	}


	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glMatrixMode(GL_MODELVIEW);
	for (i = 0; i < objectnum; i++) {
		if (object[i].visible == 0) continue;
		glPushMatrix();
		argConvGlpara(object[i].trans, gl_para);
		draw_object(object[i].id, gl_para);
		glPopMatrix();
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
	GLfloat   mat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat   mat_ambient_collide[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat   mat_flash[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat   mat_flash_collide[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat   mat_flash_shiny[] = { 50.0 };
	GLfloat   light_position[] = { 100.0, -200.0, 200.0, 0.0 };
	GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
	GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };

	//cout << "Vector: " << mech2Position[0] - mech1Position[0] << " , " << mech2Position[1] - mech1Position[1] << " , " << mech2Position[2] - mech1Position[2] << endl;
	//float ax = mech2Position[0] - mech1Position[0];
	//float ay = mech2Position[1] - mech1Position[1];
	//float az = mech2Position[2] - mech1Position[2];
	//float magnitude = sqrt((ax * ax) + (ay * ay) + (az * az));

	//gl_para[12] = gl_para[12] + (ax*currentStep * 10);
	//gl_para[13] = gl_para[13] + (ay*currentStep * 10);
	//gl_para[14] = gl_para[14] + (az*currentStep * 10);
	//cout << "Normalised Vector: " << ax / magnitude << " , " << ay / magnitude << " , " << az / magnitude << endl;
	//glTranslatef(stepX * currentStep*100, stepY * currentStep*100, stepZ * currentStep*100);
	glLoadMatrixd(gl_para);

	//Clear Depth Buffer
	//glClearDepth(1.0);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	/* set the material */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

	//Check for new detected card
	if ((!gamestate::confirmed) && gamestate::lastPlayedID != obj_id + 1 && gamestate::heroStats.first.id != obj_id + 1 && gamestate::heroStats.second.id != obj_id + 1){
		if (gamestate::checkCard(Menu->getMode(), obj_id + 1)){
			Menu->setConfirm(true);
			cout << "Detected New Card!" << endl;
			gamestate::lastPlayedID = obj_id + 1;
			gamestate::confirmed = true;
			t->play(1);
		}
	}

	//gamestate::cardlist[gamestate::heroStats.first.id].model.AnimateSound(50, 1, *t);
	
	//gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, 0, 0, 0, 0);

	//Make robots face and attack eachother
	if (robotsDrawn.size() >= 2) {
		

		//stepX = ax / magnitude * 10;
		//stepY = ay / magnitude * 10;
		//stepZ = az / magnitude * 10;
		float Pi = 3.14159265;
		//float stepX2 = (cosf(getAngleBetweenRobots() - abs(mech1Position[3])) * stepX - (sinf(getAngleBetweenRobots() - abs(mech1Position[3])) * stepZ));
		//float stepZ2 = (sinf(getAngleBetweenRobots() - abs(mech1Position[3])) * stepX + cosf(getAngleBetweenRobots() - abs(mech1Position[3])) * stepZ);
		//glTranslatef(stepX2*currentStep, stepY2*currentStep, az / magnitude*currentStep);
		//stepX = stepX2, stepY = stepY, stepZ = stepZ2;
		//cout << mech1Position[3] << "A: " << mech1Position[3] << " A: " << mech2Position[3]<<" Angle:" << getAngleBetweenRobots() << endl;
		if (gamestate::heroStats.first.id == obj_id + 1) {
			//gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, mech1Position[3] - mech2Position[3], 0, 0, 1);
			if (robotMode1 == 0) {
				gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, -getAngleBetweenRobots() + 180 - mech1Position[3], 0, 0, 1);
			}
			else if (robotMode1 == 1) {
				//glTranslatef(0, 0, currentStep * 10);
				gamestate::cardlist[obj_id + 1].drawModel(stepX * currentStep, stepY * currentStep, stepZ * currentStep, -getAngleBetweenRobots() + 180 - mech1Position[3], 0, 0, 1);
				if (currentStep == 0) {
					//gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Walk.md5anim");
				}
				currentStep++;
				if (currentStep == numSteps) {
					robotMode1 = 3;
				}
			}
			else if (robotMode1 == 2) {
				gamestate::cardlist[obj_id + 1].drawModel(stepX * currentStep, stepY * currentStep, stepZ * currentStep, -getAngleBetweenRobots() + 180 - mech1Position[3] + 180, 0, 0, 1);
				if (currentStep == numSteps) {
					//gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Walk.md5anim");
				}
				//cout << currentStep << endl;
				currentStep--;
				if (currentStep == 0) {
					robotMode1 = 0;
				}
			}
			else {
				gamestate::cardlist[obj_id + 1].drawModel(stepX * numSteps, stepY * currentStep, stepZ * currentStep, -getAngleBetweenRobots() + 180 - mech1Position[3], 0, 0, 1);
				//gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Attack(DualSwords).md5anim");
				//if (animation done) {
					robotMode1 = 2;
				//}
			}
		}
		else if (gamestate::heroStats.second.id == obj_id + 1) {
			//gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, 360-(mech1Position[3] - mech2Position[3]), 0, 0, 1);
			if (robotMode2 == 0) {
				gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, -getAngleBetweenRobots() - mech2Position[3], 0, 0, 1);
			}
			else if (robotMode2 == 1) {
				gamestate::cardlist[obj_id + 1].drawModel(-stepX * currentStep, -stepY * currentStep, -stepZ * currentStep, getAngleBetweenRobots() - mech2Position[3] + 180, 0, 0, 1);
				if (currentStep == 0) {
					//gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Walk.md5anim");
				}
				currentStep++;
				if (currentStep == numSteps) {
					robotMode2 = 3;
				}
			}
			else if (robotMode2 == 2) {
				gamestate::cardlist[obj_id + 1].drawModel(-stepX * currentStep, -stepY * currentStep, -stepZ * currentStep, getAngleBetweenRobots() - mech2Position[3], 0, 0, 1);
				if (currentStep == numSteps) {
					//gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Walk.md5anim");
				}
				currentStep--;
				if (currentStep == 0) {
					robotMode2 = 0;
				}
			}
			else {
				gamestate::cardlist[obj_id + 1].drawModel(-stepX * numSteps, -stepY * numSteps, -stepZ * numSteps, getAngleBetweenRobots() - mech2Position[3], 0, 0, 1);
				//gamestate::cardlist[gamestate::heroStats.second.id].model.loadAnimation("../Assets/Animations/Alpha_Attack(DualSwords).md5anim");
				//if (animation done) {
				robotMode2 = 2;
				//}
			}
		}
		else {
			gamestate::cardlist[obj_id+1].drawModel();
		}
	}
	else {
		gamestate::cardlist[obj_id+1].drawModel();
	}
	
	//argDrawMode2D();
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
	Menu->setResolutionX(width);
	Menu->setResolutionY(height);
	//Load Sounds
	t->createSound("../Assets/Sounds/Entrance.wav", 1);
	t->createSound("../Assets/Sounds/Ambient2.wav", 2);
	t->createSound("../Assets/Sounds/Upgrade.wav", 3);
	//Play Ambient Sounds
	t->toggleBackgroundSound(2, true);
	//Initialise Glut Functionality
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	loadOpenGL = true;
}

void keyboard(unsigned char key, int x, int y)
{
	/* Escape */
	if (key == 27)
		exit(0);
	if (Menu->getMode() > 2){
		if (key == 48){ //'0' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Defence1.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Alpha/Textures/");
		}
		if (key == 49){ //'1' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/MD5s/Delta/Delta_Mesh.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Delta_Gun.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Delta/Textures/");
		}
		if (key == 50){ //'2' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Epsilon_Dodge.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Epsilon/Textures/");
		}
		if (key == 51){ //'3' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/Animations/Alpha_Walk.md5anim");
		}
	}
	if (key == 53){ //'5' Key{
		if (robotsDrawn.size() >= 2) {
			cout << "Key 5" << endl;
			robotMode1 = 1;
			distX = getMarkerDiffX();
			distY = getMarkerDiffZ();
			distZ = getMarkerDiffY();
			numSteps = 50;
			currentStep = 0;
			stepX = distX / 50;
			stepY = distY / 50;
			stepZ = distZ / 50;
		}
	}

	if (key == 54){ //'6' Key{
		if (robotsDrawn.size() >= 2) {
			cout << "Key 6" << endl;
			robotMode2 = 1;
			distX = getMarkerDiffX();
			distY = getMarkerDiffZ();
			distZ = getMarkerDiffY();
			numSteps = 50;
			currentStep = 0;
			stepX = distX / 50;
			stepY = distY / 50;
			stepZ = distZ / 50;
		}
	}

	//Memory Leak Issue
	if (key == 32){ //'Space' Key{
		t->cleanup(1);
		t->createSound("../Assets/Sounds/Dodge.wav", 1);
		t->play(1);
	}
	if (key == 46){ //'.' Key{
		t->createSound("../Assets/Sounds/Sword.wav", 1);
		t->play(1);
	}
	if (key == 47){ //'/' Key{
		t->createSound("../Assets/Sounds/Upgrade.wav", 1);
		t->play(1);
	}
	if (key == 77){ //'N' Key{
		t->createSound("../Assets/Sounds/Entrance.wav", 1);
		t->play(1);
	}
	if (key == 78){ //'M' Key{
		t->createSound("../Assets/Sounds/Sound.wav", 1);
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

static float getMarkerDiffX() {
	//return object[m2].marker_center[0] - object[m1].marker_center[0];// -(object[m1].marker_width * 2) - (object[m2].marker_width * 2);
	return mech2Position[0] - mech1Position[0];
}

static float getMarkerDiffY() {
	//return object[m2].marker_center[1] - object[m1].marker_center[1];// - (object[m1].marker_width * 2) - (object[m2].marker_width * 2);
	return mech2Position[1] - mech1Position[1];
}

static float getMarkerDiffZ() {
	//return object[m2].marker_center[2] - object[m2].marker_center[2];// - (object[m1].marker_width * 2) - (object[m2].marker_width * 2);
	return mech2Position[2] - mech1Position[2];
}

static float getAngleBetweenRobots() {
	return (atan2(getMarkerDiffX(), getMarkerDiffZ()) * 180 / PI);
}

void reshape(int w, int h)
{
	if (!Menu->checkScreenSize(w, h))
		return glutReshapeWindow(Menu->getResolutionX(), Menu->getResolutionY());

	width = w;
	height = h;
}