#include "MTL.h"

MTL::MTL() 
{   
}
MTL::~MTL()
{

}

void MTL::setName(std::string inName)
{
    name = inName;
}







void MTL::setLoc(std::string inLocation)
{
    location = inLocation;

    LoadThing();
}
bool MTL::compare(std::string inName) // compare the string with another
{
    //return true if they are the same
    if (name == inName)
        return true;
    else
        return false;

}
void MTL::draw() // change the binds
{
    glBindTexture(GL_TEXTURE_2D, texture); 

}

void MTL::setKa(float ka0, float ka1, float ka2)
{
    ka[0] = ka0;
    ka[1] = ka1;
    ka[2] = ka2;
}

void MTL::setKd(float kd0, float kd1, float kd2)
{
    kd[0] = kd0;
    kd[1] = kd1;
    kd[2] = kd2;
}

void MTL::setKs(float ks0, float ks1, float ks2)
{
    ks[0] = ks0;
    ks[1] = ks1;
    ks[2] = ks2;
}

void MTL::LoadThing()
{
    ILuint devImg;
    ilGenImages(1,&devImg);
    ilBindImage(devImg);

    int error = ilLoadImage("marble_floor2.png");

    int e = ilGetError();

    pixmap = ilGetData();
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    int chan = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

    createTex(width,height,chan);
}

void MTL::createTex(int width, int height, int chan)
{
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, chan, width, height, 0, (chan == 4)?GL_RGBA: GL_RGB, GL_UNSIGNED_BYTE, pixmap);

    //no clue
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}