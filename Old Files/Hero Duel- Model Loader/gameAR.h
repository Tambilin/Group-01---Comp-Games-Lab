#ifndef GAMEAR_H
#define GAMEAR_H

#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __APPLE__
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif
#include "AR/gsub.h"
#include "AR/param.h"
#include <AR/ar.h>
#include <AR/video.h>

#include "object.h"

#define COLLIDE_DIST 30000.0

class gameAR
{
public:
	gameAR();
	~gameAR();
	static void   init(void);
	static void   cleanup(void);
	static void   mainLoop(void);
	static int    draw(ObjectData_T *object, int objectnum);
	static int    draw_object(int obj_id, double gl_para[16]);
};
#endif