#include "menutextures.h"

int menutextures::resolutionX = 640;
int menutextures::resolutionY = 480; 

// This holds all the information for the font that we are going to create.
freetype_mod::font_data * our_font;
freetype_mod::font_data * our_subfont;
freetype_mod::font_data our_font16;
freetype_mod::font_data our_font32;
freetype_mod::font_data our_font64;

menutextures::menutextures(void)
{
	widescreen = false;
	mode = 0;
	alpha = 5;
	rollSize = 1;
	our_font16.init("../Assets/Fonts/BuxtonSketch.TTF", 16);					    //Build the freetype font
	our_font32.init("../Assets/Fonts/BuxtonSketch.TTF", 32);					    //Build the freetype font
	our_font64.init("../Assets/Fonts/BuxtonSketch.TTF", 64);					    //Build the freetype font
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

  //Useful variables
  int offset = 0;
  int res = 1+resolutionX/1280;
  int total = 0;
  int size = rolls.size();

  //Sort out font sizes for different resolutions
  if (res > 1){
	  our_font = &our_font64;
	  our_subfont = &our_font32;
  }
  else {
	  our_font = &our_font32;
	  our_subfont = &our_font16;
  }

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
	  drawQuad(0 + offset, 0, 0, width, height); //Border
	  drawQuad(7, width - (128 * res), height - (64 * res), width - 10, height - 10); //Next turn
	  ///////////DICE ROLLS RESULTS///////////////////
	  if (size > 12){
		  size = 12;
	  }

	  for (int i = 0; i < size; i++){
		  int c = i / 4;
		  int r = i % 4;
		  total += rolls.at(i);
		  glColor4f(1.0, 1.0, 1.0, alpha);
		  drawQuad(11 + rolls.at(i), width / 4 + (r*width / 8) + 4, height - width / 4 / res - (width / 8) - (width / 8 * c), width / 4 + (width / 8) + (width / 8 * r), height - width / 4 / res - (width / 8 * c) - 4); //Dice Image
	  }

	  if (alpha > 0){
		  alpha = alpha - 0.01;
	  }
	  glPushMatrix();//Draw Dice Roll
	  glRasterPos2f(width / 2 - 30*res, height - width / 4 / res);
	  freetype_mod::print(*our_font, "+%3.0f", (float)total);	// Print GL Text To The Screen
	  glColor4f(1.0, 1.0, 1.0, 1.0);
	  glPopMatrix();
	  ////////////////////////////////////////
	  break;
  default:
	  drawQuad(3 + offset, 0, 0, width, height); //Menu background
	  break; 
  }



  if (mode != 0){
	  drawQuad(10, 2, height - 64, 256, height-2); //Menu background
	  drawQuad(9, 0, 0, (128 * res), (256 * res));
	  drawQuad(9, width - (128 * res), 0, width, (256 * res));

	  glPushMatrix();//P1
	  glColor3f(0.0, 0.0, 0.0);
	  glRasterPos2f(54 * res, 46 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.first.evasion);	// Print GL Text To The Screen
	  glRasterPos2f(54 * res, 86 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.first.defence);	// Print GL Text To The Screen
	  glRasterPos2f(54 * res, 126 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.first.attack);	// Print GL Text To The Screen
	  glRasterPos2f(54 * res, 166 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.first.hp);	// Print GL Text To The Screen
	  glRasterPos2f(54 * res, 206 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::manaPoints.first);	// Print GL Text To The Screen
	  glRasterPos2f(52 * res, 15 * res);
	  freetype_mod::print(*our_subfont, "P1");	// Print GL Text To The Screen
	  glPopMatrix();


	  glPushMatrix();//P2
	  glColor3f(0.0, 0.0, 0.0);
	  glRasterPos2f(width - 70 * res, 46 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.second.evasion);	// Print GL Text To The Screen
	  glRasterPos2f(width - 70 * res, 86 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.second.defence);	// Print GL Text To The Screen
	  glRasterPos2f(width - 70 * res, 126 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.second.attack);	// Print GL Text To The Screen
	  glRasterPos2f(width - 70 * res, 166 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::heroStats.second.hp);	// Print GL Text To The Screen
	  glRasterPos2f(width - 70 * res, 206 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::manaPoints.second);	// Print GL Text To The Screen
	  glRasterPos2f(width - 72 * res, 15 * res);
	  freetype_mod::print(*our_subfont, "P2");	// Print GL Text To The Screen
	  glPopMatrix();
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

int menutextures::getMode(void){
	return mode;
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
	  int res = 1 + resolutionX / 1280;

	  if(x > width-(128*res) && 
		 y > 10 &&
		 x < width-10 &&
		 y < (64*res)){ //Start Bo
		  if (this->mode == 1){
			  srand(time(NULL));
			  int a = rand() % 2;
			  rolls.push_back(a);
			  for (int i = 0; i < (int)gamestate::turnID / 2; i++){
				  if (i < 11){
				  int temp = rand() % 2;
				  rolls.push_back(temp);
				  a += temp;
				  }
			  }
			  if (gamestate::turnID % 2 == 0){
				  gamestate::manaPoints.first += a;
			  }
			  else {
				  gamestate::manaPoints.second += a;
				  rollSize ++;
			  }
		  }

		  this->mode++;
		  if (this->mode > 2){
			  gamestate::turnID++;
			  rolls.clear();
			  alpha = 5;
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
	menuTex[9] = loadTexture("../Assets/Textures/Stats2.png");
	menuTex[10] = loadTexture("../Assets/Textures/Titlebar.png");
	menuTex[11] = loadTexture("../Assets/Textures/0.jpg");
	menuTex[12] = loadTexture("../Assets/Textures/1.jpg");

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

int menutextures::getRollSize(){
	return rollSize;
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