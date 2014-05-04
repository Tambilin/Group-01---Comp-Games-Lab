#pragma once 

//#include "GL/freeglut.h"
//#include "GLEW/glew.h"

#include <string>
#include <fstream>

//texture loading
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")
#include <iostream>
//#include <IL/il.h>

//#include "GLEW/glew.h"
#include "glut.h"
//#include "GL/freeglut.h"

class MTL
{
public:
    MTL(); //name of the file
    ~MTL(void);

    void bind(std::string bindType);// somehow set the bind type eg tile stretch ect

    void setVar(float ka0, float ka1, float ka2,float kd0, float kd1, float kd2, float ks0, float ks1, float ks2); // takes in all the inputs and sets them to the class
    void setLoc(std::string inLocation); // set the location
    void setName(std::string inName);

    void setKa(float ka0, float ka1, float ka2);
    void setKd(float kd0, float kd1, float kd2);
    void setKs(float ks0, float ks1, float ks2);

    void setNs(float inNs);

    bool compare(std::string inName); // compare the string with another
    void draw(); // change the binds
    GLuint getGLID() { return texture;};

private:

    std::string name;
    std::string location; // the location of the image

    float ka[3];
    float kd[3];
    float ks[3];


    // must have freeglut in order to use this
    BYTE *pixmap; // contains the data (not yet set)
    GLuint texture;

    //two functions that you need to load and display
    void LoadThing();
    void createTex(int width, int height, int chan);

};