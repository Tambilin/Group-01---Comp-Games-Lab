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
#include "particles.h"
#include "gamestate.h"
#include "objload.h"
#include "soundeffect.h"
#include "vmath.h"

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
double          frustrum = 2.0;
bool Mode3D = false;
bool winner = false;

particles * fireworks[particles::FIREWORKS_COUNT];

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
static void   updateRobots();

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    argDrawMode2D();
	//argDispHalfImage(dataPtr, 0, 0);
	if (Mode3D){
		glColorMask(true, false, false, false);
		glTranslatef(-frustrum*4, 0, 0);
		glScalef(1.0, 1, 1);
		argDispImage(dataPtr, 0, 0);
		glColorMask(false, true, true, false);
		glTranslatef(frustrum*4, 0, 0);
		argDispImage(dataPtr, 0, 0);
		glColorMask(true, true, true, true);
	}
	else {
		argDispImage(dataPtr, 0, 0);
	}

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
		//Requires minimum confidence value
		if (marker_info[k].cf < 0.7) {
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

	if (Mode3D){
		glColorMask(true, false, false, false);
		frustrum = frustrum * -1;
		draw(object, objectnum);
		glColorMask(false, true, true, false);
		frustrum = frustrum * -1;
		draw(object, objectnum);
		glColorMask(true, true, true, true);
	}
	else {
		draw(object, objectnum);
	}

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

	//enable client states for glDrawElements
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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

	if (winner){
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw fireworks
	for (int loop = 0; loop < particles::FIREWORKS_COUNT; loop++)
	{
		for (int particleLoop = 0; particleLoop < particles::FIREWORK_PARTS; particleLoop++)
		{

			// Set the point size of the firework particles (this needs to be called BEFORE opening the glBegin(GL_POINTS) section!)
			glPointSize(fireworks[loop]->particleSize);
			glBegin(GL_POINTS);

			// Set colour to yellow on way up, then whatever colour firework should be when exploded
			if (fireworks[loop]->exploded == false)
			{
				glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
			}
			else
			{
				glColor4f(fireworks[loop]->r, fireworks[loop]->g, fireworks[loop]->b, fireworks[loop]->a);
			}

			// Draw the point
			glVertex2f(fireworks[loop]->x[particleLoop], fireworks[loop]->y[particleLoop]);
			glEnd();
		}

		// Move the firework appropriately depending on its explosion state
		if (fireworks[loop]->exploded == false)
		{
			fireworks[loop]->move();
		}
		else
		{
			fireworks[loop]->explode();
		}
	}
	glDisable(GL_BLEND);
	glPopMatrix();
	glPopAttrib();
	//glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
	}

	//Render Menus
	glPushMatrix();
	glTranslatef(frustrum, 0, 0);
	Menu->render(width, height);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(250, 250, 0);
	objModel->DrawModelUsingFixedFuncPipeline();
	glPopMatrix();

	//Update Rotations
	if (movement < 720)
		movement = movement + 1;
	else
		movement = 0;

	updateRobots();
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
	GLfloat   light_position[] = { 100.0, -200.0, 250.0, 0.0 };
	GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
	GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };

	//Perform matrix calculations to move & position robots
	Matrix4d mat = Matrix4d(gl_para);
	Matrix4d rot;
	if (gamestate::heroStats.first.id == obj_id + 1) {
		rot = mat.createRotationAroundAxis(0, 0, getAngleBetweenRobots() + 180 + mech1Position[3]);
	}
	else {
		rot = mat.createRotationAroundAxis(0, 0, getAngleBetweenRobots() + mech2Position[3]);
	}
	Matrix4d trans = Matrix4d();
	Matrix4d Offset3D = Matrix4d();
	if (Mode3D){
		Offset3D.setTranslation(Vector3i(frustrum*1.5, 0, 0));
	}
	if ((robotMode1 >= 1 && gamestate::heroStats.first.id == obj_id + 1) || (robotMode2 >= 1 && gamestate::heroStats.second.id == obj_id + 1)){
		trans = mat.createTranslation(0, -currentStep*stepX, 0, 1);
	}
	glLoadMatrixd(Offset3D*mat*rot*trans);


	/* set the lighting */
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

	//Draw robots
	if (Menu->getMode() > 2) {
		if (gamestate::heroStats.first.id == obj_id + 1 || gamestate::heroStats.second.id == obj_id + 1) {
			gamestate::cardlist[obj_id + 1].drawModel(0, 0, 0, 0, 0, 0, 0); //Grounded selected robots
		}
	}
	else {
		gamestate::cardlist[obj_id + 1].drawModel(); //Hovering Robots
	}
	return 0;
}

void updateRobots(){
	//Update robots positions
	if (robotsDrawn.size() >= 2) {
		if (robotMode1 == 0) {
		}
		else if (robotMode1 == 1) {
			if (currentStep == 0) {
				currentStep = 0;
				gamestate::cardlist[gamestate::heroStats.first.id].model.animationFinished = false;
				gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation = true;
				stepX = distX / (gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames()/2);
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation(gamestate::cardlist[gamestate::heroStats.first.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.first.id].model.setCurrentFrame(20);
			}
			currentStep ++;
			if (currentStep >= gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames()/2) {
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation();
				robotMode1 = 3;
			}
		}
		else if (robotMode1 == 2) {
			currentStep--;
			if (currentStep <= 0) {
				robotMode1 = 0;
			}
		}
		else {
			if (gamestate::cardlist[gamestate::heroStats.first.id].model.getCurrentFrame() == gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames()) {
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation(gamestate::cardlist[gamestate::heroStats.first.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.first.id].model.setCurrentFrame(gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames() / 2);
				robotMode1 = 2;
			}
		}

		if (robotMode2 == 0) {
		}
		else if (robotMode2 == 1) {
			currentStep++;
			if (currentStep >= numSteps) {
				robotMode2 = 3;
			}
		}
		else if (robotMode2 == 2) {
			currentStep--;
			if (currentStep <= 0) {
				robotMode2 = 0;
			}
		}
		else {
			robotMode2 = 2;
		}
	}
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
	//load particles
	for (int i = 0; i < particles::FIREWORKS_COUNT; i++){
		fireworks[i] = new particles(width, height);
	}

	loadOpenGL = true;
}

void keyboard(unsigned char key, int x, int y)
{
	/* Escape */
	if (key == 27)
		exit(0);
	if (Menu->getMode() > 2){
		if (key == 48){ //'0' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/MD5s/Alpha/Alpha_Mesh.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/MD5s/Alpha/Animations/Alpha_Defence1.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Alpha/Textures/");
		}
		if (key == 49){ //'1' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/MD5s/Delta/Delta_Mesh.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/MD5s/Delta/Animations/Delta_Gun.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Delta/Textures/");
		}
		if (key == 50){ //'2' Key{
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadModel("../Assets/MD5s/Epsilon/Epsilon_Mesh_Gun.md5mesh");
			gamestate::cardlist[gamestate::heroStats.first.id].model.loadAnimation("../Assets/MD5s/Epsilon/Animations/Epsilon_Gun.md5anim");
			gamestate::cardlist[gamestate::heroStats.first.id].model.useModelShaderTextures("../Assets/MD5s/Epsilon/Textures/");
		}
	}
	if (key == 53){ //'5' Key{
		if (robotsDrawn.size() >= 2) {
			cout << "Key 5" << endl;
			robotMode1 = 1;
			distX = abs(getMarkerDiffX()) - thresh;
			//cout << getMarkerDiffX() - thresh;
			///distY = getMarkerDiffY();
			//distZ = getMarkerDiffZ();
			//numSteps = 50;
			currentStep = 0;
			stepX = distX / 50;
			//stepY = distY / 50;
			//stepZ = distZ / 50;
		}
	}

	if (key == 32){ //'6' Key{
		Mode3D = !Mode3D;
	}

	if (key == 54){ //'6' Key{
		if (robotsDrawn.size() >= 2) {
			cout << "Key 6" << endl;
			robotMode2 = 1;
			distX = abs(getMarkerDiffX()) - thresh;
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
		winner = !winner;
		cout << winner << endl;
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