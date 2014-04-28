#include "menutextures.h"

int menutextures::resolutionX = 640;
int menutextures::resolutionY = 480; 
int menutextures::screenID = 0;

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
	previousMode = 0;
	confirm = false;
	attackedThisTurn = false;
	options = false;
	cardTex = -2;
	alpha = 5;
	music = true;
	rollSize = 1;
	our_font16.init("../Assets/Fonts/BuxtonSketch.TTF", 16);					    //Build the freetype size 16
	our_font32.init("../Assets/Fonts/BuxtonSketch.TTF", 32);					    //Build the freetype size 32
	our_font64.init("../Assets/Fonts/BuxtonSketch.TTF", 64);					    //Build the freetype size 64
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
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  ///VERY IMPORTANT LINE OF CODE!!!!
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
  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //Define Variables
  int offset = 0;
  int res = 1+resolutionX/1380;
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


  //Widescreen mode
  if(widescreen == false)
	offset = 0;
  else 
    offset = 1;

  //Switch between different menus being drawn
  switch (mode)
  {
  case 0: //Main Menu
	  drawQuad(3 + offset, 0, 0, width, height); //Menu background
	  drawQuad(5, width - (128 * res), height - (64 * res), width - 10, height - 10); //Start
	  drawQuad(6, width - (128 * res), height - (64 * res * 2), width - 10, height - 10 - (64 * res)); //Options
	  break;
  case 1: //Player 1 Select Data
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  glColor4f(1.0, 0.0, 0.0, 1.0);
	  glRasterPos2f(width / 2 - 260 * res, height - height / 3 / res);
	  freetype_mod::print(*our_font, "Player 1 - Place Your Selected Mech");	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
	  break;
  case 2: //Player 2 Select Data
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  glColor4f(0.0, 0.0, 1.0, 1.0);
	  glRasterPos2f(width / 2 - 260 * res, height - height / 3 / res);
	  freetype_mod::print(*our_font, "Player 2 - Place Your Selected Mech");	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
	  break;
  case 3: //Roll dice screen
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  if (gamestate::phase == 1){
		  glColor4f(1.0, 0.0, 0.0, 1.0);
	  }
	  else {
		  glColor4f(0.0, 0.0, 1.0, 1.0);
	  }
	  glRasterPos2f(width / 2 - 160 * res, height - height / 3 / res);
	  freetype_mod::print(*our_font, "Player %1.0f - Draw a card.", (float)gamestate::phase);	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
	  drawQuad(8, width - (128 * res), height - (64 * res), width - 10, height - 10); //Roll
	  break;
  case 4: //Turn screen
		  if (gamestate::phase == 1 && gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation == false
			  || gamestate::phase == 2 && gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation == false){
			  drawQuad(7, width - (128 * res), height - (64 * res), width - 10, height - 10); //Next turn
			  if (!attackedThisTurn){
				  drawQuad(16, width - (128 * res), height - (64 * res * 2), width - 10, height - 10 - (64 * res)); //Attack
			  }
		  }
	  ///////////DICE ROLLS RESULTS///////////////////
	  if (size > 12){
		  size = 12;
	  }

	  if (alpha > 0){
	  for (int i = 0; i < size; i++){
		  int c = i / 4;
		  int r = i % 4;
		  total += rolls.at(i);
		  glPushMatrix();//Draw Dice Rolls
		  glPushAttrib(GL_CURRENT_BIT);
		  glColor4f(1.0, 1.0, 1.0, alpha);
		  drawQuad(11 + rolls.at(i), width / 4 + (r*width / 8) + 4, height - width / 4 / res - (width / 8) - (width / 8 * c), width / 4 + (width / 8) + (width / 8 * r), height - width / 4 / res - (width / 8 * c) - 4); //Dice Image
		  glPopAttrib();
		  glPopMatrix();
	  }
	  alpha = alpha - 0.0033*gamestate::deltaTime;
	  }
	  ////////////////////////////////////////
	  break;
  case 5: //Victory screen
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  if (gamestate::phase == 1){
		  glColor4f(1.0, 0.0, 0.0, 1.0);
	  }
	  else {
		  glColor4f(0.0, 0.0, 1.0, 1.0);
	  }
	  glRasterPos2f(width / 2 - 160 * res, height - height / 3 / res);
	  freetype_mod::print(*our_font, "Player %1.0f has won the game!", (float)gamestate::winner);	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
	  drawQuad(8, width - (128 * res), height - (64 * res), width - 10, height - 10); //Roll
	  break;
  default:
	  drawQuad(3 + offset, 0, 0, width, height); //Menu background
	  break; 
  }

  //Draw player stats
  if (mode != 0){
	  drawQuad(10, 2, height - 64, 256, height-2); //TitleBar
	  glPushMatrix();//P1
	  glPushAttrib(GL_CURRENT_BIT);
	  glColor3f(0.0, 0.0, 0.0);
	  glRasterPos2f(32, height-48);
	  freetype_mod::print(our_font16, "Turn %2.0f - Player %1.0f's Turn", (float)gamestate::turnID, (float)gamestate::phase);	// Print GL Text To The Screen
	  glPopAttrib();
	  glPopMatrix();

	  drawQuad(9, 0, 0, (128 * res), (256 * res));
	  drawQuad(9, width - (128 * res), 0, width, (256 * res));

	  drawQuad(17, (18*res), (255 * res), (110 * res), (255 * res)+(80*res));
	  drawQuad(17, width - (18 * res) - (90 * res), (255 * res), width - (18 * res), (255 * res) + (80 * res));

	  glPushMatrix();//P1
	  glPushAttrib(GL_CURRENT_BIT);
	  glColor3f(0.0, 0.0, 0.0);
	  glRasterPos2f(54 * res, 266 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::handSize.first);	// Print GL Text To The Screen
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
	  glPopAttrib();
	  glPopMatrix();



	  glPushMatrix();//P2
	  glPushAttrib(GL_CURRENT_BIT);
	  glColor3f(0.0, 0.0, 0.0);
	  glRasterPos2f(width - 70 * res, 266 * res);
	  freetype_mod::print(*our_font, "%3.0f", (float)gamestate::handSize.second);	// Print GL Text To The Screen
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
	  glPopAttrib();
	  glPopMatrix();
  }

  //Draw text of dice results
  if (mode == 4){ //Dice results
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  glColor4f(1.0, 1.0, 0.0, alpha);
	  glRasterPos2f(width / 2 - 30*res, height - width / 4 / res);
	  freetype_mod::print(*our_font, "+%3.0f", (float)total);	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
  }

  //Draw Options screen
  if (options){
	  int boxH = 64 * res;
	  int boxW = 128 * res;
	  drawQuad(2, 0, height - (boxH * 5) - 10, boxW * 2 + 10, height); //Transparent background
	  drawQuad(18, 10, height - (boxH), (boxW * 2), height - 10); //Option A
	  drawQuad(18, 10, height - (boxH * 2), (boxW * 2), height - 10 - (boxH)); //Option B
	  drawQuad(18, 10, height - (boxH * 3), (boxW * 2), height - 10 - (boxH * 2)); //Option C
	  drawQuad(18, 10, height - (boxH * 4), (boxW * 2), height - 10 - (boxH * 3)); //Option D
	  drawQuad(10, 40 * res, height - (boxH * 5), (boxH * 2), height - 10 - (boxH * 4)); //Option E

	  glPushMatrix();//P2
	  glPushAttrib(GL_CURRENT_BIT);
	  glColor4f(0.0, 0.0, 0.0, 1.0);

	  glRasterPos2f((50 * res), height - (64 * res * 5) + 19 * res);
	  freetype_mod::print(*our_subfont, "Exit Game");	// Print GL Text To The Screen
	  glRasterPos2f((50 * res), height - (64 * res * 4) + 19 * res);
	  freetype_mod::print(*our_subfont, "Animation Speed: %1.1f", (float)md5load::animSpeed);	// Print GL Text To The Screen
	  glRasterPos2f((50 * res), height - (64 * res * 3) + 19 * res);
	  if (screenID != 4){
		freetype_mod::print(*our_subfont, "Window: %4i x %4i", resolutionX, resolutionY);	// Print GL Text To The Screen
	  }	else {
	  freetype_mod::print(*our_subfont, "Window: Fullscreen Mode", resolutionX, resolutionY);	// Print GL Text To The Screen
      }
	  if (gamestate::activate3D){
		  glRasterPos2f((50 * res), height - (64 * res * 2) + 19 * res);
		  freetype_mod::print(*our_subfont, "3D Activated: On");	// Print GL Text To The Screen
	  }
	  else {
		  glRasterPos2f((50 * res), height - (64 * res * 2) + 19 * res);
		  freetype_mod::print(*our_subfont, "3D Activated: Off");	// Print GL Text To The Screen
	  }
	  glRasterPos2f((50 * res), height - (64 * res * 1) + 19 * res);
	  freetype_mod::print(*our_subfont, "Anaglyph 3D: %1.1f", (float)gamestate::frustrum3D);	// Print GL Text To The Screen

	  glPopAttrib();
	  glPopMatrix();
  }

  //Selection screen when new card is played, overrides other modes.
  if (confirm){
	  glPushAttrib(GL_CURRENT_BIT);
	  glPushMatrix();
	  glColor4f(1.0, 1.0, 0.0, 1.0);
	  glRasterPos2f(width - 360 * res, height - (46*res));
	  freetype_mod::print(*our_subfont, "Do you want to use this card?");	// Print GL Text To The Screen
	  glPopMatrix();
	  glPopAttrib();
	  drawQuad(13, width - (128 * res), height - (64 * res), width - 10, height - 10); //Confirm
	  drawQuad(14, width - (128 * res), height - (64 * res * 2), width - 10, height - 10 - (64 * res)); //Cancel
	  char directory[50] = "../Assets/Textures/Cards/";
	  char number[5] = "";
	  char filetype[5] = ".png";
	  sprintf(number, "%d", gamestate::lastPlayedID);
	  strcat(directory, number);
	  strcat(directory, filetype);
	  if (cardTex != gamestate::lastPlayedID){
		  menuTex[15] = loadTexture(directory);
		  cardTex = gamestate::lastPlayedID;
	  }
	  int difference = (((height/2) - (20))*0.72)/2;
	  drawQuad(15, width / 2-difference, 20, width/2+difference, height/2);
  }

  glDisable(GL_BLEND);
}

int menutextures::getMode(void){
	return mode;
}

void menutextures::setMode(int i){
	previousMode = mode;
	mode = i;
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
		screenID = item;
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
			glutReshapeWindow(1920, 1080);   
			resolutionX = 1920;
			resolutionY = 1080;
			break;
		case 4:
			glutFullScreen();
        default:
            glutReshapeWindow(640, 480); 
			resolutionX = 640;
			resolutionY = 480;
			break;
        }
		//glutReshapeWindow(640, 480);
		glutPositionWindow(100,100);
		glViewport(0, 0, resolutionX, resolutionY);

        glutPostRedisplay();
        return;
}

void menutextures::setConfirm(bool t){
	confirm = t;
}

//Check mouse button input interaction with GUI elements
void menutextures::checkButtonClick(int x, int y, int width, int height){
	int res = 1 + resolutionX / 1380;
	if (gamestate::phase == 1 && gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation == false
		|| gamestate::phase == 2 && gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation == false){
		if (confirm && mode != 0){
			if (x > width - (128 * res) &&
				y > 10 &&
				x < width - 10 &&
				y < (64 * res)){
				cout << "gamestate::lastPlayedID:" << gamestate::lastPlayedID << endl;
				if (mode == 1){ //Activate Mech 1
					gamestate::heroStats.first = gamestate::cardlist[(gamestate::lastPlayedID)];
					if (gamestate::numPlayers == 2) {
						mode++;
					}
					else {
						mode += 2;
						aiturn::init();
					}
					confirm = false;
				}
				else if (mode == 2){ //Activate Mech 2
					gamestate::heroStats.second = gamestate::cardlist[(gamestate::lastPlayedID)];
					mode++;
					confirm = false;
				}
				else if (mode == 4){ //Activate Card
					if (gamestate::phase == 1){
						gamestate::handSize.first--;
					}
					else {
						gamestate::handSize.second--;
					}
					gamestate::cardActivated(gamestate::phase, (gamestate::lastPlayedID));
					confirm = false;
				}
				gamestate::confirmed = false;
			}
			else if (x > width - (128 * res) &&
				y > 10 + (64 * res) &&
				x < width - 10 &&
				y < (64 * res * 2)){ //Cancel Place Card
					gamestate::lastPlayedID = -1;
					confirm = false;
					gamestate::confirmed = false;
				
			}
		}
		else {
			if (x > width - (128 * res) &&
				y > 10 &&
				x < width - 10 &&
				y < (64 * res)){ 
				if (this->mode != 1 && this->mode != 2){
					if (this->mode == 5){
						mode = previousMode;
					}

					if (this->mode == 3){ //Calculate Roll Data
						srand(time(NULL));
						int a = rand() % 2;
						rolls.push_back(a);
						for (int i = 0; i < ((int)gamestate::turnID - 1) / 2; i++){
							if (i < 11){
								int temp = rand() % 2;
								rolls.push_back(temp);
								a += temp;
							}
						}
						if (gamestate::turnID % 2 == 0){
							gamestate::manaPoints.second += a;
							gamestate::handSize.second++;
						}
						else {
							gamestate::manaPoints.first += a;
							gamestate::handSize.first++;
							if (rollSize < 12)
								rollSize++;
						}
					}

					this->mode++;
					if (this->mode > 4){ //Switch Turn
						gamestate::turnID++;
						attackedThisTurn = false;
						rolls.clear();
						alpha = 1.5;
						if (gamestate::phase == 2){
							gamestate::phase = 1;
						}
						else {
							gamestate::phase = 2;
						}
						this->mode = 3;
					}
				}
			}
			else if (x > width - (128 * res) &&
				y > 10 + (64 * res) &&
				x < width - 10 &&
				y < (64 * res * 2)){
				if (mode == 0){ //Options Button on Main Menu
					options = !options;
				}

				if (this->mode == 4 && attackedThisTurn == false){ //Attack Button
					if (gamestate::phase == 1 && gamestate::cardlist[gamestate::heroStats.first.id].model.temporaryAnimation == false
						|| gamestate::phase == 2 && gamestate::cardlist[gamestate::heroStats.second.id].model.temporaryAnimation == false){
						gamestate::attacking = gamestate::phase;
						attackedThisTurn = true;
						
					}
				}
			}
		}
	}

	//Check for clicks on Options Menu
	if (options){
		int boxH = 64 * res;

		if (x > 17 && x < 127 * res){
			if (y > 17 && y < boxH){
				gamestate::frustrum3D -= 0.1;
			}else if (y > boxH && y < boxH * 2){
				gamestate::activate3D = !gamestate::activate3D;
			}
			else if (y > boxH * 2 && y < boxH * 3){
				//Window
				if (screenID > 0){
					screenID--;
					screenSizeMenu(screenID);
				}
			}
			else if (y > boxH * 3 && y < boxH * 4){
				if (md5load::animSpeed > 1){
					md5load::animSpeed -= 0.1;
				}
			}
			else if (y > boxH * 4 && y < boxH * 5){
				exit(0);
			}

		}
		else {
			if (x > 127*res && x < 254 * res){
				if (y > 17 && y < boxH){
					gamestate::frustrum3D+= 0.1;
				}
				else if (y > boxH && y < boxH * 2){
					gamestate::activate3D = !gamestate::activate3D;
				}
				else if (y > boxH * 2 && y < boxH * 3){
					if (screenID < 4){
						screenID++;
						screenSizeMenu(screenID);
					}
				}
				else if (y > boxH * 3 && y < boxH * 4){
					md5load::animSpeed += 0.1;
				}

			}
		}

		cout << x << " " << y << endl;
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
	menuTex[2] = loadTexture("../Assets/Textures/Transparent2.png");
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
	menuTex[13] = loadTexture("../Assets/Textures/Confirm.png");
	menuTex[14] = loadTexture("../Assets/Textures/Cancel.png");
	menuTex[16] = loadTexture("../Assets/Textures/Attack.png");
	menuTex[17] = loadTexture("../Assets/Textures/Hand.png");
	menuTex[18] = loadTexture("../Assets/Textures/Optionsbar.png");

	// Create a menu
	glutCreateMenu(&menutextures::screenSizeMenu);

        // Add menu items
        glutAddMenuEntry("640 × 480 (VGA) 4:3", 0);
        glutAddMenuEntry("800 x 600 (SVGA) 4:3 ", 1);
		glutAddMenuEntry("1280 x 720 (720p) 16:9 ", 2);
        glutAddMenuEntry("1920 x 1080 (1080p) 16:9", 3);
		glutAddMenuEntry("Fullscreen", 4);

        // Associate a mouse button with menu
        glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menutextures::text(void *font, const char *fmt, int x, int y, float r, float g, float b){
	//glColor3f(r, g, b);
	glRasterPos2f(x, y);
	const char *c;
	for (c = fmt; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
	//glColor3f(1, 1, 1);
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
