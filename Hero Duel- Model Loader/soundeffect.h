#pragma once
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <al.h>
#include <alc.h>
#include <conio.h> 
#include <time.h> 
#include <stdlib.h> 
#include <alut.h> 

using namespace std;

class soundeffect
{
private:
	// Maximum data buffers we will need. 
#define NUM_BUFFERS 3 
// Maximum emissions we will need. 
#define NUM_SOURCES 3 

// Buffers hold sound data. 
ALuint buffer[NUM_BUFFERS]; 

unsigned char* buf[3];
// Sources are points of emitting sound. 
ALuint source[NUM_SOURCES]; 
// Position of the source sounds. 
ALfloat SourcesPos[NUM_SOURCES][3]; 
// Velocity of the source sounds. 
ALfloat SourcesVel[NUM_SOURCES][3]; 

	int endWithError(char* msg);
	int createDeviceContext();

public:
	int createSound(char * filename, int track);
	soundeffect(void);
	soundeffect(char * filePath);
	void setLoop(int track, bool status);
	int play(int track);
	~soundeffect(void);
};