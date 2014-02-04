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
#include <C:\Program Files (x86)\ARToolKit/include\AR\gsub.h>
#include <C:\Program Files (x86)\ARToolKit/include\AR/video.h>
#include <C:\Program Files (x86)\ARToolKit/include\AR/param.h>
#include <C:\Program Files (x86)\ARToolKit/include\AR/ar.h>


void main(int argc, char *argv[]) {
    init();
    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
}

void init() {
	//Open video path, vconf is default camera config
	if(arVideoOpen(vconf) < 0) {
		exit(0);
	}

	//Get size of window
	if(arVideoInqSize(&xsize, &ysize) < 0) {
		exit(0);
	}
}