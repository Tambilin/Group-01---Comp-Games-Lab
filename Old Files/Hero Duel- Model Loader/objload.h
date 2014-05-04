#pragma once
//#include "GLEW/glew.h"

#include <cstdlib>
#include <iostream>
#include "model_obj.h"
#include "bitmap.h"
using namespace std;

#include "GLEW/glew.h"
#include "glut.h"

//-----------------------------------------------------------------------------
// Type definitions.
//-----------------------------------------------------------------------------

typedef std::map<std::string, GLuint> ModelTextures;

class objload
{
private:
	ModelOBJ            g_model;
	ModelTextures       g_modelTextures;
	bool                g_enableTextures;
	bool                g_cullBackFaces;

public:
objload(void);
objload(char * filePath);
void LoadModel(const char *pszFilename);
GLuint LoadTexture(const char *pszFilename);
GLuint CreateNullTexture(int width, int height);
void DrawModelUsingFixedFuncPipeline();
void enableBackFaceCulling(bool g_cullBackFaces);
void enableTextures(bool g_textured);
void UnloadModel();
void InitGL();
~objload(void);
};