#pragma once
#include <iostream>
#include "gamestate.h"
using namespace std;

#include "GLEW/glew.h"
#include "glut.h"

//texture loading
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#include "freetype.h"		// Header for our little font library.

class menutextures
{
private:
	GLuint loadTexture(char *fileName);
	GLuint menuTex[256];//Max 256 Textures
	static int resolutionX; 
	static int resolutionY; 
	int mode;
	bool widescreen; 
public:
	menutextures(void);
	void load(void);
	int getMode(void);
	void render(int width, int height);
	void checkButtonClick(int x, int y, int width, int height);
	void text(void *font, const char *fmt, int x, int y, float r, float g, float b);
	bool checkScreenSize(int x, int y);
	int getResolutionX();
	int getResolutionY();
	static void setResolutionX(int x);
	static void setResolutionY(int y);
	static void _cdecl screenSizeMenu(int item);
	void drawQuad(int textureID, int minX, int minY, int maxX, int maxY);
	menutextures(char * filePath);
	~menutextures(void);
};