#pragma once
#include <iostream>
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
	void render(int width, int height);
	void checkButtonClick(int x, int y, int width, int height);
	void text(void *font, const char *fmt, int x, int y, float r, float g, float b);
	bool checkScreenSize(int x, int y);
	int getResolutionX();
	int getResolutionY();
	void setResolutionX(int x);
	void setResolutionY(int y);
	static void _cdecl screenSizeMenu(int item);
	void drawQuad(int textureID, int minX, int minY, int maxX, int maxY);
	menutextures(char * filePath);
	~menutextures(void);
};