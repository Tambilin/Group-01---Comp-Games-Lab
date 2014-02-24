#include "menutextures.h"

int menutextures::resolutionX = 640;
int menutextures::resolutionY = 480; 

menutextures::menutextures(void)
{
	widescreen = false;
	mode = 0;
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
  int offset = 0;
  int res = resolutionX/640;

  if(widescreen == false)
	offset = 0;
  else 
    offset = 1;

  switch (mode)
  {
  case 0:
	  drawQuad(3 + offset, 0, 0, width, height); //Menu background
	  drawQuad(5, width - (128 * res), height - (64 * res), width - 10, height - 10); //Start
	  drawQuad(6, width - (128 * res), height - (64 * res * 2), width - 10, height - 10 - (64 * res)); //Options
	  break;
  case 1:
	  drawQuad(0 + offset, 0, 0, width, height); //Border
	  drawQuad(8, width - (128 * res), height - (64 * res), width - 10, height - 10); //Roll
	  break;
  case 2:
	  //drawQuad(0 + offset, 0, 0, width, height); //Border
	  text(GLUT_BITMAP_HELVETICA_18, "P1:", 10, 10, 1, 1, 0);
	  text(GLUT_BITMAP_HELVETICA_18, "P2:", width - 35, 10, 1, 1, 0);
	  drawQuad(7, width - (128 * res), height - (64 * res), width - 10, height - 10); //Next turn
	  break;
  default:
	  drawQuad(3 + offset, 0, 0, width, height); //Menu background
	  break;
  }

  glDisable(GL_BLEND);

  glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, width/(GLdouble)height, 0.1, 1000.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
 glPopMatrix();
 glEnable( GL_DEPTH_TEST );
}


void menutextures::drawQuad(int textureID, int minX, int minY, int maxX, int maxY){
  glBindTexture(GL_TEXTURE_2D, menuTex[textureID]);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(minX, minY);

  glTexCoord2f(1, 1);
  glVertex2f(maxX, minY);

  glTexCoord2f(1, 0);
  glVertex2f(maxX, maxY);

  glTexCoord2f(0, 0);
  glVertex2f(minX, maxY);
  glEnd();

}

// Menu handling function definition
void menutextures::screenSizeMenu(int item)
{
        switch (item)
        {
        case 0:
			glutReshapeWindow(640, 480);
			resolutionX = 640;
			resolutionY = 480;
			break;
        case 1:
			glutReshapeWindow(800, 600);     
			resolutionX = 800;
			resolutionY = 600;
			break;
        case 2:
			glutReshapeWindow(1280, 720);  
			resolutionX = 1280;
			resolutionY = 720;
			break;
        case 3:
			glutReshapeWindow(1600, 1200); 
			resolutionX = 1600;
			resolutionY = 1200;
			break;
        case 4:
			glutReshapeWindow(1920, 1080);   
			resolutionX = 1920;
			resolutionY = 1080;
			break;
        default:
            glutReshapeWindow(640, 480); 
			resolutionX = 640;
			resolutionY = 480;
			break;
        }
		glutPositionWindow(100,100);


        glutPostRedisplay();
        return;
}



void menutextures::checkButtonClick(int x, int y, int width, int height){
	  int res = resolutionX/640;

	  if(x > width-(128*res) && 
		 y > 10 &&
		 x < width-10 &&
		 y < (64*res)){ //Start Bo
		  this->mode++;
		  if (this->mode > 2){
			  this->mode = 1;
		  }
	  } else if(x > width- (128*res) &&
		 y > 10+(64*res) &&
		 x < width-10 &&
		 y < (64*res*2)){ //Options)

	  }
}

bool menutextures::checkScreenSize(int w, int h){
	if((w == 640 && h == 480) || (w == 800 && h == 600) || (w == 1600 && h == 1200)){
			widescreen = false;
		return true;
	}
	if((w == 1280 && h == 720) || (w == 1920 && h == 1080)){
			widescreen = true;
		return true;
	}
	return false;
}

void menutextures::load(void){
	menuTex[0] = loadTexture("../Assets/Textures/Border_4-3.png");
	menuTex[1] = loadTexture("../Assets/Textures/Border_16-9.png");
	menuTex[2] = loadTexture("../Assets/Textures/Transparent.png");
	menuTex[3] = loadTexture("../Assets/Textures/Menu_4-3.png");
	menuTex[4] = loadTexture("../Assets/Textures/Menu_16-9.png");
	menuTex[5] = loadTexture("../Assets/Textures/Start.png");
	menuTex[6] = loadTexture("../Assets/Textures/Options.png");
	menuTex[7] = loadTexture("../Assets/Textures/NextTurn.png");
	menuTex[8] = loadTexture("../Assets/Textures/Roll.png");

	// Create a menu
	glutCreateMenu(&menutextures::screenSizeMenu);

        // Add menu items
        glutAddMenuEntry("640 × 480 (VGA) 4:3", 0);
        glutAddMenuEntry("800 x 600 (SVGA) 4:3 ", 1);
		glutAddMenuEntry("1280 x 720 (720p) 16:9 ", 2);
        glutAddMenuEntry("1600 x 1200 (UXGA) 4:3 ", 3);
        glutAddMenuEntry("1920 x 1080 (1080p) 16:9", 4);

        // Associate a mouse button with menu
        glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menutextures::text(void *font, const char *fmt, int x, int y, float r, float g, float b){
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	const char *c;
	for (c = fmt; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
	glColor3f(1, 1, 1);
}

int menutextures::getResolutionX(){
	return resolutionX;
}

int menutextures::getResolutionY(){
	return resolutionY;
}

void menutextures::setResolutionX(int x){
	resolutionX = x;
}

void menutextures::setResolutionY(int y){
	resolutionY = y;
}