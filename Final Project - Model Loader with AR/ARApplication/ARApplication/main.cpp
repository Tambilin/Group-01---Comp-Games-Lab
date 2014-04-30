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
#include "aiturn.h"

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
int				lastMech1Position[6];
int				mech2Position[6];
double          frustrum = 2.0;
bool			Mode3D = false;
bool			winner = false;
bool			updatingBots = false;
bool			attackingBots = false;
bool			aiPlayedCard = false;
int				bestCard = -1;


particles * fireworks[particles::FIREWORKS_COUNT];

//Class Objects
md5load Dice;
md5load Coin;

menutextures * Menu = new menutextures();
objload * Wings = new objload();
objload * objModel = new objload();

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

	frustrum = gamestate::frustrum3D;
	Mode3D = gamestate::activate3D;

    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count2 == 0 ) arUtilTimerReset();
    count2++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    argDrawMode2D();

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
		glPushMatrix();
		glPushAttrib(GL_CURRENT_BIT);
		//glPixelZoom(1.5, 1.5);
		argDispImage(dataPtr, 0, 0);
		glPopAttrib();
		glPopMatrix();
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
		frustrum = 0;
		draw(object, objectnum);
	}

	//Swap buffers
	argSwapBuffers();
}

static void init( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ARParam  wparam;

	/* open the video path */
	//arVideoOpen("inputDevice=WDM_CAP,flipH,flipV,showDlg");
	if (arVideoOpen(vconf) < 0) exit(0);
	/* find the size of the window */
	if (arVideoInqSize(&width, &height) < 0) exit(0);
	printf("Image size (x,y) = (%d,%d)\n", width, height);
	menutextures::cameraX = width;
	menutextures::cameraY = height;


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

	//
	glutReshapeWindow(50, 20);
	//glutPositionWindow(100, 100);
	//Menu->setResolutionX(640);
	//Menu->setResolutionY(480);
	//glViewport(0, 0, width, height);
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

	gamestate::updateDeltaTime();

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
			mech1Position[0] = gl_para[12]; //X Position
			mech1Position[1] = gl_para[13]; //Y Position
			mech1Position[2] = gl_para[14]; //Z Position
			lastMech1Position[0] = mech1Position[0];
			lastMech1Position[1] = mech1Position[1];
			lastMech1Position[2] = mech1Position[2];
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
				lastMech1Position[3] = mech1Position[3];
				lastMech1Position[4] = mech1Position[4];
				lastMech1Position[5] = mech1Position[5];
				//cout << "xrot = " << mech1Position[3] << endl;
			}
		}
		else if (i+1 == gamestate::heroStats.second.id){
			if (gamestate::numPlayers == 2) {
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
			//else {
				//Set AI mech position
				//for (int i = 0; i < 6; i++) {
				//	mech2Position[i] = mech1Position[i];
				//	
				//}
				//mech2Position[1] += 20;
			//}
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
	if (gamestate::numPlayers == 1 && Menu->getMode() > 2) {
		int thisID = gamestate::heroStats.second.id;
		glPushMatrix();
			argConvGlpara(object[gamestate::heroStats.first.id-1].trans, gl_para);
			Matrix4d mat = Matrix4d(gl_para);
			Matrix4d trans = Matrix4d();
			if (mech1Position[3] >= 90 || mech1Position[3] <= -90) {
				trans = mat.createTranslation(mech1Position[0] * 2, 0, 0, 1);
			}
			else {
				trans = mat.createTranslation(mech1Position[0] * -2, 0, 0, 1);
			}
			
			Matrix4d t = mat*trans;
			//Set AI mech position
			mech2Position[0] = t.data[12];
			mech2Position[1] = t.data[13];
			mech2Position[2] = t.data[14];
			mech2Position[3] = lastMech1Position[3];
			mech2Position[4] = lastMech1Position[4];
			mech2Position[5] = lastMech1Position[5];
			//Draw AI mech
			draw_object(object[thisID-1].id, t.data);
		glPopMatrix();
		robotsDrawn.push_back(gamestate::heroStats.second.id - 1);
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	/// 2D Menu Rendering ///
	argDrawMode2D();

	if (gamestate::winner > 0){
		glPushAttrib(GL_CURRENT_BIT);
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
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
		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
		glPopAttrib();
		glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
	}
	else {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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
		movement = movement + md5load::animSpeed;
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
	else if (gamestate::heroStats.second.id == obj_id + 1){
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
	if ((gamestate::winner < 1)&&(!gamestate::confirmed) && gamestate::lastPlayedID != obj_id + 1 && gamestate::heroStats.first.id != obj_id + 1 && gamestate::heroStats.second.id != obj_id + 1){
		if (gamestate::checkCard(Menu->getMode(), obj_id + 1)){
			if (obj_id + 1 != 30){
				Menu->setConfirm(true);
				gamestate::confirmed = true;
			}
			cout << "Detected New Card!" << endl;
			gamestate::lastPlayedID = obj_id + 1;
			gamestate::t->cleanup(1);
			gamestate::t->createSound("../Assets/Sounds/Entrance.wav", 1);
			gamestate::t->play(1);
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

	if (Menu->getMode() == 3) {
		if (gamestate::heroStats.first.id == obj_id + 1 && gamestate::phase == 1 || gamestate::heroStats.second.id == obj_id + 1 && gamestate::phase == 2) {
			Dice.draw(0, 0, 350, 1.2, movement, 1, 1, 1); //Draw Model
		}
	}
	return 0;
}

void updateRobots() {
	//AI player
	if (Menu->getMode() == 4 && gamestate::phase == 2 && gamestate::numPlayers == 1 && !updatingBots) {
		updatingBots = true;
		aiturn::drawCard(1);
		bestCard = aiturn::chooseCard(gamestate::difficulty);
		if (bestCard != -1) {
			aiPlayedCard = false;
			gamestate::cardActivated(gamestate::phase, bestCard);
			gamestate::handSize.second--;
		}
		else {
			aiPlayedCard = false;
		}
	}
	if (gamestate::phase == 2 && gamestate::numPlayers == 1 && bestCard != -1) {
		Menu->showAIcard(bestCard+1);
	}
	if (Menu->getMode() == 4 && gamestate::phase == 2 && gamestate::numPlayers == 1 && !attackingBots) {
		if (gamestate::cardlist[gamestate::heroStats.second.id].model.animationFinished && gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation || !aiPlayedCard) {
			attackingBots = true;
			gamestate::attacking = gamestate::phase;
		}
	}
	//cout << gamestate::deltaTime << endl;
	int numsteps = 1.5 * gamestate::deltaTime/md5load::animSpeed;
	if (gamestate::attacking == 1){
		//numsteps = (gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames() / 2)*
		robotMode1 = 1;
		distX = abs(getMarkerDiffX()) - thresh;
		currentStep = 0;
		stepX = distX / (numsteps);
		gamestate::attacking = 0;
	}
	else if (gamestate::attacking == 2){
		robotMode2 = 1;
		distX = abs(getMarkerDiffX()) - thresh;
		currentStep = 0;
		stepX = distX / (numsteps);
		gamestate::attacking = 0;
	}


	//Update robots positions
	if (robotsDrawn.size() >= 2) {
		if (robotMode1 == 0) {
		}
		else if (robotMode1 == 1) {
			if (currentStep == 0) {
				currentStep = 0;
				gamestate::cardlist[gamestate::heroStats.first.id].model.animationFinished = false;
				gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation = true;
				//stepX = distX / numsteps;//(gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames()/2);
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation(gamestate::cardlist[gamestate::heroStats.first.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.first.id].model.setCurrentFrame(20);
				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Walk.wav", 1);
				gamestate::t->play(1);
			}
			currentStep ++;
			if (currentStep >= numsteps){//gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames() / 2) {
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation();
				robotMode1 = 3;
			}
			if (currentStep % (int)(gamestate::deltaTime*0.4) == 0){
				gamestate::t->play(1);
			}
		}
		else if (robotMode1 == 2) {
			currentStep--;
			if (currentStep <= 0) {
				gamestate::cardlist[gamestate::heroStats.first.id].model.animationFinished = true;
				gamestate::cardlist[gamestate::heroStats.first.id].model.setCurrentFrame(gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames() - md5load::animSpeed);
				robotMode1 = 0;
				if (gamestate::winner > 0){
					Menu->setMode(5);
				}
			}
			if (currentStep % (int)(gamestate::deltaTime*0.4) == 0){
				gamestate::t->play(1);
			}
		}
		else {
			if (gamestate::cardlist[gamestate::heroStats.first.id].model.getCurrentFrame() == (int)(gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames() / 2)){
				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Sword.wav", 1);
				gamestate::t->play(1);
			}
			if (gamestate::cardlist[gamestate::heroStats.first.id].model.getCurrentFrame() >= gamestate::cardlist[gamestate::heroStats.first.id].model.getTotalFrames()-md5load::animSpeed) {
				gamestate::cardlist[gamestate::heroStats.first.id].model.animationFinished = false;
				gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation = true;
				gamestate::cardlist[gamestate::heroStats.first.id].performAnimation(gamestate::cardlist[gamestate::heroStats.first.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.first.id].model.setCurrentFrame(20);
				gamestate::cardAttack();
				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Walk.wav", 1);
				robotMode1 = 2;
			}
		}


		if (robotMode2 == 0) {
		}
		else if (robotMode2 == 1) {
			if (currentStep == 0) {
				currentStep = 0;
				gamestate::cardlist[gamestate::heroStats.second.id].model.animationFinished = false;
				gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation = true;
				//stepX = distX / numsteps;//(gamestate::cardlist[gamestate::heroStats.second.id].model.getTotalFrames() / 2);
				gamestate::cardlist[gamestate::heroStats.second.id].performAnimation(gamestate::cardlist[gamestate::heroStats.second.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.second.id].model.setCurrentFrame(20);

				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Walk.wav", 1);
				gamestate::t->play(1);
			}
			currentStep++;
			if (currentStep >= numsteps){//gamestate::cardlist[gamestate::heroStats.second.id].model.getTotalFrames() / 2) {
				gamestate::cardlist[gamestate::heroStats.second.id].performAnimation();
				robotMode2 = 3;
			}
			else {
				if (currentStep % (int)(gamestate::deltaTime*0.4) == 0){
					gamestate::t->play(1);
				}
			}
		}
		else if (robotMode2 == 2) {
			currentStep--;
			if (currentStep <= 0) {
				gamestate::cardlist[gamestate::heroStats.second.id].model.animationFinished = true;
				gamestate::cardlist[gamestate::heroStats.second.id].model.setCurrentFrame(gamestate::cardlist[gamestate::heroStats.second.id].model.getTotalFrames() - md5load::animSpeed);
				robotMode2 = 0;
				if (gamestate::phase == 2 && gamestate::numPlayers == 1) {
					updatingBots = false;
					attackingBots = false;
					aiPlayedCard = false;
					bestCard = -1;
					gamestate::turnID++;
					if (gamestate::phase == 2){
						gamestate::phase = 1;
					}
					else {
						gamestate::phase = 2;
					}
					Menu->setMode(3);
				}
				if (gamestate::winner > 0){
					Menu->setMode(5);
				}
			}
			if (currentStep % (int)(gamestate::deltaTime*0.4) == 0){
				gamestate::t->play(1);
			}
		}
		else {
			if (gamestate::cardlist[gamestate::heroStats.second.id].model.getCurrentFrame() == (int)(gamestate::cardlist[gamestate::heroStats.second.id].model.getTotalFrames() / 2)){
				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Sword.wav", 1);
				gamestate::t->play(1);
			}

			if (gamestate::cardlist[gamestate::heroStats.second.id].model.getCurrentFrame() >= gamestate::cardlist[gamestate::heroStats.second.id].model.getTotalFrames() - md5load::animSpeed) {
				gamestate::cardlist[gamestate::heroStats.second.id].model.animationFinished = false;
				gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation = true;
				gamestate::cardlist[gamestate::heroStats.second.id].performAnimation(gamestate::cardlist[gamestate::heroStats.second.id].animationWalk);
				gamestate::cardlist[gamestate::heroStats.second.id].model.setCurrentFrame(20);
				gamestate::cardAttack();
				gamestate::t->cleanup(1);
				gamestate::t->createSound("../Assets/Sounds/Walk.wav", 1);
				robotMode2 = 2;
			}
		}
	}
}

void loadData(){
	gamestate::init();
	//Load OBJ Models
	Wings->InitGL();
	Wings->LoadModel("../Assets/Models/coin.obj");
	objModel->LoadModel("../Assets/Models/dice.obj");
	Coin.init("../Assets/MD5s/Coin/Coin.md5mesh", NULL, "../Assets/Textures/DiceCount.png");
	Dice.init("../Assets/MD5s/Dice/Dice.md5mesh", NULL, "../Assets/Textures/0.jpg");
	//Load Menus
	Menu->load();
	Menu->setResolutionX(width);
	Menu->setResolutionY(height);
	//Load Sounds
	gamestate::t->createSound("../Assets/Sounds/Entrance.wav", 1);
	gamestate::t->createSound("../Assets/Sounds/Music.wav", 2);
	gamestate::t->createSound("../Assets/Sounds/Upgrade.wav", 3);
	//Play Ambient Sounds
	gamestate::t->toggleBackgroundSound(2, true);
	//Initialise Glut Functionality
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	//load particles
	for (int i = 0; i < particles::FIREWORKS_COUNT; i++){
		fireworks[i] = new particles(width, height);
	}

	Menu->setResolutionX(640);
	Menu->setResolutionY(480);
	glutReshapeWindow(640, 480);
	glutPositionWindow(100, 100);
	glViewport(0, 0, width, height);

	loadOpenGL = true;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27){ //Exit - Escape
		exit(0);
	}

	if (key == 112){ //Options Menu - P
		Menu->options = !Menu->options;
	}

	if (key == 32){ //Activate 3D - Space
		gamestate::activate3D = !gamestate::activate3D;
	}
}

void mouse(int button, int state, int x, int y)
{
	// Respond to mouse button presses.
	// If button1 pressed, mark this state so we know in motion function.
	if (button == GLUT_LEFT_BUTTON)
	{
		//cout << "CLICK" << endl;
		leftButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE;
		if (leftButtonDown == true){
			Menu->checkButtonClick(x, y, width, height);
		}
	}
}

static float getMarkerDiffX() {
	return mech2Position[0] - mech1Position[0];
}

static float getMarkerDiffY() {
	return mech2Position[1] - mech1Position[1];
}

static float getMarkerDiffZ() {
	return mech2Position[2] - mech1Position[2];
}

static float getAngleBetweenRobots() {
	return atan2(getMarkerDiffX(), getMarkerDiffZ()) * 180 / PI;
}

void reshape(int w, int h)
{
	ARParam  wparam;
	arParamChangeSize(&wparam, Menu->getResolutionX(), Menu->getResolutionY(), &cparam);
	
	if (!Menu->checkScreenSize(w, h) && menutextures::screenID != 4){
		return glutReshapeWindow(Menu->getResolutionX(), Menu->getResolutionY());
	}
	width = w;
	height = h;
}