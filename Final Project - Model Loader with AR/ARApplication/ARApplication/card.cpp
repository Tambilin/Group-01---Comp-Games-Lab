#include "card.h"


card::card()
{
}


card::~card()
{
}

void card::loadModel(){
	switch (this->id)
	{
	case 1:
		this->model.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Idle.md5anim", "../Assets/Textures/Head.tga");
		break;
	case 2:
		this->model.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Idle.md5anim", "../Assets/Textures/grass.tga");
		break;
	case 3:
		this->model.init("../Assets/Models/Epsilon_Mesh.md5mesh", NULL, "../Assets/Textures/Feet.tga");
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}
}

void card::drawModel(){
	model.enableTextured(true);
	glPushMatrix();
	if (this->id == 3){
		glTranslatef(0, 0, 10);
		model.draw(0, 0, 0, 0.4, 0, 0, 0, 0); //Draw Model
	}
	else {
		glTranslatef(0, -0, -150);
		model.draw(0, 0, 0, 1, 0, 0, 0, 0); //Draw Model
	}
	glPopMatrix();
}

void card::drawModel(int x, int y, int z, int angle, int i, int j, int k){
	model.enableTextured(true);
	glTranslatef(0+x, -0+y, -150+z);
	glRotatef(angle , i, j, k);
	glPushMatrix();
	if (this->id == 3){
		glTranslatef(0, 0, 10);
		model.draw(0, 0, 0, 0.4, 0, 0, 0, 0); //Draw Model
	}
	else {
		glTranslatef(0, -0, -150);
		model.draw(0, 0, 0, 1, 0, 0, 0, 0); //Draw Model
	}
	glPopMatrix();
}