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
		animationWalk = "../Assets/MD5s/Alpha/Animations/Alpha_Walk.md5anim";
		animationUpgrade = "../Assets/MD5s/Alpha/Animations/Alpha_Upgrade.md5anim";
		break;
	case 2:
		this->model.init("../Assets/MD5s/Delta/Delta_Mesh.md5mesh", "../Assets/MD5s/Delta/Animations/Delta_Idle.md5anim", "../Assets/Textures/grass.tga");
		this->model.useModelShaderTextures("../Assets/MD5s/Delta/Textures/");
		animation = "../Assets/MD5s/Delta/Animations/Delta_Punch.md5anim"; 
		animationWalk = "../Assets/MD5s/Delta/Animations/Delta_Walk.md5anim";
		animationUpgrade = "../Assets/MD5s/Delta/Animations/Delta_Upgrade.md5anim";
		break;
	case 3:
		this->model.init("../Assets/MD5s/Epsilon/Epsilon_Mesh.md5mesh", "../Assets/MD5s/Epsilon/Animations/Epsilon_Idle.md5anim", "../Assets/Textures/grass.tga");
		this->model.useModelShaderTextures("../Assets/MD5s/Epsilon/Textures/");
		animation = "../Assets/MD5s/Epsilon/Animations/Epsilon_Punch.md5anim";
		animationWalk = "../Assets/MD5s/Epsilon/Animations/Epsilon_Walk.md5anim";
		animationUpgrade = "../Assets/MD5s/Epsilon/Animations/Epsilon_Upgrade.md5anim";
		break;
	default:
		break;
	}
}

void card::loadAnimation(){
	switch (this->id)
	{
	case 1:
		this->model.loadAnimation("../Assets/MD5s/Alpha/Animations/Alpha_Idle.md5anim");
		break;
	case 2:
		this->model.loadAnimation("../Assets/MD5s/Delta/Animations/Delta_Idle.md5anim");
		break;
	case 3:
		this->model.loadAnimation("../Assets/MD5s/Epsilon/Animations/Epsilon_Idle.md5anim");
	default:
		break;
	}
}

void card::drawModel(){
	model.enableTextured(true);
	glPushMatrix();
	if (this->id == 3){ //Different sizes due to differently scaled model exports. 
		glTranslatef(0, 0, 100);
		model.draw(0, 0, 0, 0.2, 0, 0, 0, 0); //Draw Model 3
	}
	else if (this->id == 1){
		glTranslatef(0, 0, 0);
		model.draw(0, 0, 0, 0.5, 0, 0, 0, 0); //Draw Model 1
	}
	else {
		glTranslatef(0, -0, 50);
		model.draw(0, 0, 0, 0.5, 0, 0, 0, 0); //Draw Model 2
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
	if (this->id == 3){ //Different sizes due to differently scaled model exports. 
		glTranslatef(0, 0, -50);
		model.draw(x, y, z, 0.2, angle, i, j, k); //Draw Model 3
	}
	else {
		glTranslatef(0, -0, -100);
		model.draw(x, y, z, 0.5, angle, i, j, k); //Draw Model 1 or 2
	}
	glPopMatrix();
}

void card::performAnimation(){
	model.loadAnimation(animation);
}

void card::performAnimation(const char * anim){
	model.loadAnimation(anim);
}