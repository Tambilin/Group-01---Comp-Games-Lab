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
		this->model.init("../Assets/Models/Alpha_Mesh_Broadsword.md5mesh", "../Assets/Animations/Alpha_Attack(BroadSword).md5anim", "../Assets/Textures/Head.tga");
		break;
	case 2:
		this->model.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk.md5anim", "../Assets/Textures/grass.tga");
		break;
	case 3:
		this->model.init("../Assets/Models/Alpha_Mesh.md5mesh", "../Assets/Animations/Alpha_Walk.md5anim", "../Assets/Textures/grass.tga");
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
	glTranslatef(0, -0, -150);
	glPushMatrix();
	model.draw(0, 0, 0, 1, 0, 0, 0, 0); //Draw Model
	glPopMatrix();
}