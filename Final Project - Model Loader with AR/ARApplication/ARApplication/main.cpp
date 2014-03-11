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
#include "md5load.h"
#include "menutextures.h"
#include "objload.h"
#include "soundeffect.h"
//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int             width, height;
int             thresh = 100;
int             count2 = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char           *patt_name      = "Data/patt.hiro";
int             patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];

//Additional Variables
bool			loadOpenGL = false;
static bool     leftButtonDown = false;
int             c = 0;
bool            renderModel = false;
int				movement = 0;
int				fps = 0;

//Class Objects
md5load md5object;
md5load md5object1;
md5load md5object2;
md5load md5object3;

menutextures * Menu = new menutextures();
objload * Wings = new objload();
soundeffect * t = new soundeffect();

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw( void );
static void   loadData(void);
static void   keyboard(unsigned char key, int x, int y);
static void   mouse(int button, int state, int x, int y);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	init();

    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
	return (0);
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
    int             j, k;

    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count2 == 0 ) arUtilTimerReset();
    count2++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

    /* detect the markers in the video frame */
	arDetectMarker(dataPtr, thresh, &marker_info, &marker_num);

	//Get next video capture
    arVideoCapNext();

    /* check for object visibility */
    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) k = j;
            else if( marker_info[k].cf < marker_info[j].cf ) k = j;
        }
    }

	renderModel = (k != -1); //Enable rendering of model


    /* get the transformation between the marker and the real camera */
	if (c > 10){
		arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
		c = 0;
	}
	else {
		c++;
	}

    draw();

	//Swap buffers
	glutSwapBuffers();
	glutPostRedisplay();
}

static void init( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ARParam  wparam;
	
    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&width, &height) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", width, height);
    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, width, height, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit( &cparam, 1.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static void draw( void )
{
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
    
    /* load the camera transformation matrix and model transformations*/
	double    gl_para[16]; 
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para ); //Load model transformation matrix (Translates model to card position)
	glMatrixMode(GL_MODELVIEW);


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

	if (renderModel){
		md5object.enableTextured(false);
		glTranslatef(0, -0, -200);
		glPushMatrix();
		md5object.draw(0, 0, 0, 1, 0, 0, 0, 0); //Draw Model
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

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	//Render the OBJ coin
	glPushMatrix();
	glTranslatef(width / 2, height / 2, 0);
	glRotatef(90, 1, 0, 0);
	glRotatef(movement, 0, 0, 1);
	glScalef(60, 60, 60);
	Wings->enableTextures(true);
	Wings->enableBackFaceCulling(false);
	Wings->DrawModelUsingFixedFuncPipeline();
	glPopMatrix();

	//Update Rotations
	if (movement < 720)
		movement = movement + 1;
	else
		movement = 0;
}

void loadData(){
	gamestate::init();
	//Load MD5 Models
	md5object.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Gun.md5anim", "../Assets/Textures/Head.tga");
	md5object1.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk2.md5anim", "../Assets/Textures/grass.tga");
	//Load OBJ Models
	Wings->InitGL();
	Wings->LoadModel("../Assets/Models/coin.obj");
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
		md5object.cleanup();
		md5object.init("../Assets/Models/pinky.md5mesh", "../Assets/Animations/run.md5anim", "../Assets/Textures/pinky_d.tga");
	}
	if (key == 49){ //'1' Key{
		md5object.cleanup();
		md5object.init("../Assets/Models/pinky.md5mesh", "../Assets/Animations/run.md5anim", "../Assets/Textures/pinky_s.tga");
	}
	if (key == 50){ //'2' Key{
		md5object.cleanup();
		md5object.init("../Assets/Models/pinky.md5mesh", "../Assets/Animations/idle1.md5anim", "../Assets/Textures/pinky_s.tga");
	}
	if (key == 51){ //'2' Key{
		md5object.cleanup();
		md5object.init("../Assets/Models/pinky.md5mesh", "../Assets/Animations/attack.md5anim", "../Assets/Textures/pinky_d.tga");
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