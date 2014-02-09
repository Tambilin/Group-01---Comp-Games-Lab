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
	int resolutionX; 
	int resolutionY; 
	bool widescreen; 
public:
	menutextures(void);
	void load(void);
	void render(int width, int height);
	void checkButtonClick(int x, int y);
	bool checkScreenSize(int x, int y);
	int getResolutionX();
	int getResolutionY();
	menutextures(char * filePath);
	~menutextures(void);
};