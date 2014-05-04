/***********************************************************
	Class for menu buttons actions and menu rendering
	- Thomas Linstead 4759265
************************************************************/

#pragma once
#include <iostream>     /* cout debugging */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "gamestate.h"
#include "aiturn.h"
using namespace std;

//OpenGL Libraries
#include "GLEW/glew.h"
#include <GL/gl.h>
#include <GL/glut.h>
using namespace std;

//Texture loading libraries 
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#include "freetype.h"		// Header for font library.

class menutextures
{
private:
	GLuint loadTexture(char *fileName);
	GLuint menuTex[256];   //Max 256 Textures
	static int resolutionX; 
	static int resolutionY;
	int mode;
	int previousMode;
	bool widescreen; 
	vector<int> rolls;
	float alpha;
	int rollSize;
	bool confirm;
	bool attackedThisTurn;
	int cardTex;
	int AIcardTex;
	bool music;

public:
	bool options;
	static int screenID;
	static int cameraX;
	static int cameraY;

	menutextures(void);
	void load(void);
	void setConfirm(bool t);
	int getMode(void);
	void setMode(int i);
	void render(int width, int height);
	void checkButtonClick(int x, int y, int width, int height);
	void text(void *font, const char *fmt, int x, int y, float r, float g, float b);
	bool checkScreenSize(int x, int y);
	int getResolutionX();
	int getResolutionY();
	int getRollSize();
	static void setResolutionX(int x);
	static void setResolutionY(int y);
	static void _cdecl screenSizeMenu(int item);
	void drawQuad(int textureID, int minX, int minY, int maxX, int maxY);
	void showAIcard(int cardID);

	menutextures(char * filePath);
	~menutextures(void);
};