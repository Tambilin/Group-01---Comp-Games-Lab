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
		this->model.init("../Assets/MD5s/Alpha/Alpha_Mesh.md5mesh", "../Assets/MD5s/Alpha/Animations/Alpha_Idle.md5anim", "../Assets/Textures/grass.tga");
		this->model.useModelShaderTextures("../Assets/MD5s/Alpha/Textures/");
		animation = "../Assets/MD5s/Alpha/Animations/Alpha_Punch.md5anim";
		break;
	case 2:
		this->model.init("../Assets/MD5s/Delta/Delta_Mesh.md5mesh", "../Assets/MD5s/Delta/Animations/Delta_Idle.md5anim", "../Assets/Textures/grass.tga");
		this->model.useModelShaderTextures("../Assets/MD5s/Delta/Textures/");
		animation = "../Assets/MD5s/Delta/Animations/Delta_Punch.md5anim"; 
		break;
	case 3:
		this->model.init("../Assets/MD5s/Epsilon/Epsilon_Mesh.md5mesh", "../Assets/MD5s/Epsilon/Animations/Epsilon_Idle.md5anim", "../Assets/Textures/grass.tga");
		this->model.useModelShaderTextures("../Assets/MD5s/Epsilon/Textures/");
		animation = "../Assets/MD5s/Epsilon/Animations/Epsilon_Punch.md5anim";
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

void card::loadAnimation(){
	switch (this->id)
	{
	case 1:
		this->model.loadAnimation("../Assets/Animations/Alpha_Idle.md5anim");
		//animation = "../Assets/Animations/Alpha_Punch.md5anim";
		break;
	case 2:
		this->model.loadAnimation("../Assets/Animations/Alpha_Idle.md5anim");
		//animation = "../Assets/Animations/Alpha_Punch.md5anim";
		break;
	case 3:
		this->model.loadAnimation("../Assets/Animations/Epsilon_Idle.md5anim");
		//animation = "../Assets/Animations/Epsilon.md5anim";
	default:
		break;
	}
}

void card::drawModel(){
	model.enableTextured(true);
	glPushMatrix();
	if (this->id == 3){
		glTranslatef(0, 0, 100);
		model.draw(0, 0, 0, 0.2, 0, 0, 0, 0); //Draw Model
	}
	else if (this->id == 1){
		glTranslatef(0, 0, 0);
		model.draw(0, 0, 0, 0.5, 0, 0, 0, 0); //Draw Model
	}
	else {
		glTranslatef(0, -0, 50);
		model.draw(0, 0, 0, 0.5, 0, 0, 0, 0); //Draw Model
	}
	glPopMatrix();
}

void card::drawModel(int x, int y, int z, int angle, int i, int j, int k){
	if (model.temporaryAnimation && model.animationFinished){
		model.temporaryAnimation = false;
		model.animationFinished = false;
		loadAnimation();
	}

	model.enableTextured(true);
	glPushMatrix();
	if (this->id == 3){
		glTranslatef(0, 0, -50);
		model.draw(x, y, z, 0.2, angle, i, j, k); //Draw Model
	}
	else {
		glTranslatef(0, -0, -100);
		model.draw(x, y, z, 0.5, angle, i, j, k); //Draw Model
	}
	glPopMatrix();
}

void card::performAnimation(){
	model.loadAnimation(animation);
}