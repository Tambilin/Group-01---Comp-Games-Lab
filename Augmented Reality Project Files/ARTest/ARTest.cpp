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

#ifdef __cplusplus
extern "C" {
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub.h>
#endif

	//
	// Camera configuration.
	//
#ifdef _WIN32
	char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
	char			*vconf = "";
#endif

	int             xsize, ysize;
	int             thresh = 100;
	int             count = 0;

	char           *cparam_name = "Data/camera_para.dat";
	ARParam         cparam;

	char           *patt_name = "Data/patt.hiro";
	int             patt_id;
	double          patt_width = 80.0;
	double          patt_center[2] = { 0.0, 0.0 };
	double          patt_trans[3][4];

	static void   init();
	static void   cleanup();
	static void   keyEvent(unsigned char key, int x, int y);
	static void   mainLoop();
	static void   draw();

	void main(int argc, char *argv[]) {
		init();
		arVideoCapStart();
		argMainLoop(NULL, keyEvent, mainLoop);
	}

	static void init() {
		ARParam  wparam;

		//Open video path, vconf is default camera config
		if (arVideoOpen(vconf) < 0) {
			exit(0);
		}

		//Get size of window
		if (arVideoInqSize(&xsize, &ysize) < 0) {
			exit(0);
		}

		//Set camera paramaters
		if (arParamLoad(cparam_name, 1, &wparam) < 0) {
			printf("Couldn't load camera parameters");
			exit(0);
		}
		arParamChangeSize(&wparam, xsize, ysize, &cparam);
		arInitCparam(&cparam);

		//Load pattern
		if ((patt_id = arLoadPatt(patt_name)) < 0) {
			printf("Failed to load pattern");
			exit(0);
		}

		//Open graphics window
		argInit(&cparam, 1, 0, 0, 0, 0);
	}

	static void keyEvent(unsigned char key, int x, int y) {
		//Exit program is ESC key pressed
		if (key == 0x1b) {
			cleanup();
			exit(0);
		}
	}

	static void mainLoop() {
		ARUint8         *dataPtr;
		ARMarkerInfo    *marker_info;
		int             marker_num;
		int             j, k;

		//Get video frame
		if ((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
			arUtilSleep(2);
			return;
		}
		if (count == 0) {
			arUtilTimerReset();
		}
		count++;

		//Display video stream
		argDrawMode2D();
		argDispImage(dataPtr, 0, 0);

		/* detect the markers in the video frame */
		if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) {
			cleanup();
			exit(0);
		}

		//Get next video frame
		arVideoCapNext();

		/* check for object visibility */
		k = -1;
		for (j = 0; j < marker_num; j++) {
			if (patt_id == marker_info[j].id) {
				if (k == -1) k = j;
				else if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}

		//Don't bother drawing or calculating orientations if no patterns are found
		if (k == -1) {
			argSwapBuffers();
			return;
		}

		/* get the transformation between the marker and the real camera */
		arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

		draw();

		argSwapBuffers();
	}

	static void cleanup() {
		arVideoCapStop();
		arVideoClose();
		argCleanup();
	}

	static void draw() {
		double    gl_para[16];
		GLfloat   mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
		GLfloat   mat_flash[] = { 0.0, 0.0, 1.0, 1.0 };
		GLfloat   mat_flash_shiny[] = { 50.0 };
		GLfloat   light_position[] = { 100.0, -200.0, 200.0, 0.0 };
		GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
		GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };

		//Initialise OpenGL renderer
		argDrawMode3D();
		argDraw3dCamera(0, 0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		/* load the camera transformation matrix */
		argConvGlpara(patt_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		//Lighting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
		glutSolidCube(50.0);
		glDisable(GL_LIGHTING);

		glDisable(GL_DEPTH_TEST);
	}

#ifdef __cplusplus
}
#endif