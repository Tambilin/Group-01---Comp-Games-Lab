#include "menutextures.h"


menutextures::menutextures(void)
{
	//resolutionX = 640;
	//resolutionY = 480;
	//widescreen = false;
}


menutextures::~menutextures(void)
{
}

GLuint menutextures::loadTexture(char *fileName)
{
	ILuint ILImage;
	GLuint textureHandle;
	ilGenImages(1,&ILImage);
	glGenTextures(1,&textureHandle);

	ilBindImage(ILImage);
	ILboolean success = ilLoadImage(fileName);
	if (!success)
	{
		MessageBox(NULL, "could not load texture", "problem", 0);
	}

	glBindTexture(GL_TEXTURE_2D, textureHandle);
	// enable automatic mipmap generation
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		ilGetInteger(IL_IMAGE_BPP), 
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 
		0, 
		ilGetInteger(IL_IMAGE_FORMAT), 
		GL_UNSIGNED_BYTE,
		ilGetData()
		);

	fprintf (stderr, "Loaded file: \"%s\"!\n", fileName);

	// release IL image
	ilDeleteImages(1, &ILImage);
	return(textureHandle);
}

void menutextures::render(int width, int height){

 glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,width,0,height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  if(widescreen == false)
	glBindTexture(GL_TEXTURE_2D, menuTex[0]);
  else 
    glBindTexture(GL_TEXTURE_2D, menuTex[1]);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(0, 0);

  glTexCoord2f(1, 1);
  glVertex2f(width, 0);

  glTexCoord2f(1, 0);
  glVertex2f(width, height);

  glTexCoord2f(0, 0);
  glVertex2f(0, height);
  glEnd();


  glBindTexture(GL_TEXTURE_2D, menuTex[2]);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(0, 0);

  glTexCoord2f(1, 1);
  glVertex2f(width, 0);

  glTexCoord2f(1, 0);
  glVertex2f(width, height);

  glTexCoord2f(0, 0);
  glVertex2f(0, height);
  glEnd();

  glDisable(GL_BLEND);

  glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, width/(GLdouble)height, 0.1, 1000.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
 glPopMatrix();
 glEnable( GL_DEPTH_TEST );
}

// Menu handling function definition
void screenSizeMenu(int item)
{
        switch (item)
        {
        case 0:
			glutReshapeWindow(640, 480);       
			break;
        case 1:
			glutReshapeWindow(800, 600);       
			break;
        case 2:
			glutReshapeWindow(1280, 720);       
			break;
        case 3:
			glutReshapeWindow(1600, 1200);        
			break;
        case 4:
			glutReshapeWindow(1920, 1080);        
			break;
        default:
            glutReshapeWindow(640, 480);      
			break;
        }
		glutPositionWindow(100,100);

        glutPostRedisplay();
        return;
}

void menutextures::checkButtonClick(int x, int y){
		
}

bool menutextures::checkScreenSize(int w, int h){
	if((w == 640 && h == 480) || (w == 800 && h == 600) || (w == 1600 && h == 1200)){
			resolutionX = w;
			resolutionY = h;
			widescreen = false;
		return true;
	}
	if((w == 1280 && h == 720) || (w == 1920 && h == 1080)){
		    resolutionX = w;
			resolutionY = h;
			widescreen = true;
		return true;
	}
	return false;
}

void menutextures::load(void){
	menuTex[2] = loadTexture("../Assets/Textures/Transparent.png");
	menuTex[0] = loadTexture("../Assets/Textures/Border_4-3.png");
	menuTex[1] = loadTexture("../Assets/Textures/Border_16-9.png");

	// Create a menu
	glutCreateMenu(screenSizeMenu);

        // Add menu items
        glutAddMenuEntry("640 × 480 (VGA) 4:3", 0);
        glutAddMenuEntry("800 x 600 (SVGA) 4:3 ", 1);
		glutAddMenuEntry("1280 x 720 (720p) 16:9 ", 2);
        glutAddMenuEntry("1600 x 1200 (UXGA) 4:3 ", 3);
        glutAddMenuEntry("1920 x 1080 (1080p) 16:9", 4);

        // Associate a mouse button with menu
        glutAttachMenu(GLUT_LEFT_BUTTON);
}

int menutextures::getResolutionX(){
	return resolutionX;
}

int menutextures::getResolutionY(){
	return resolutionY;
}