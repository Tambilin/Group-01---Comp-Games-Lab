#include "gamestate.h"

int gamestate::turnID = 1;
bool gamestate::firstSecond = false;
std::pair<int, int> gamestate::manaPoints = std::make_pair(0, 0);
std::pair<int, int> gamestate::deckSize = std::make_pair(30, 30);
std::pair<int, int> gamestate::handSize = std::make_pair(0, 0);
std::pair<card, card> gamestate::heroStats;
std::pair<card, card> gamestate::weapons;
std::unordered_map< int, card > gamestate::cardlist;
int gamestate::phase = 1;
int gamestate::lastPlayedID = -1;
float gamestate::mech1Position[6] = { 0, 0, 0, 0, 0, 0 };
float gamestate::mech2Position[6] = { 0, 0, 0, 0, 0, 0 };

gamestate::gamestate()
{  
}


gamestate::~gamestate()
{
}

void gamestate::init(){
	loadCSV("CardData.csv");
	heroStats.first = cardlist[0];
	heroStats.second = cardlist[0];
}

void gamestate::setHero(int player, int cardid){

}

void gamestate::updateState(void){

}

int gamestate::currentState(){
	return phase;
}

/* Deprecated Code */
void gamestate::loadStateData(std::string fileScene){
	////////////////Load Scene file///////////////////
	cout << "Loading Data File..." << endl;
	FILE * file;                    //Open File from filename
	char full[60000];
	sprintf(full, "%s", &fileScene);
	file = fopen(full, "r");


	if (file == NULL){      //Check File Opened Correctly
		std::cerr << "ERROR: Failed to open the scene file!" << std::endl;
		return;
		//exit(EXIT_FAILURE);
	}
	else {
		cout << "Scene File Opened...." << endl;
	}

	while (1){  //Loop for all lines of file
		char lineHeader[6000]; //Read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		//Load Total Number Of Objects/Primitives
		if (strcmp(lineHeader, "s") == 0){
			int x = 0, y = 0;
			fscanf(file, "%f%f", &x, &y);
			//menutextures::setResolutionX(x);
			//menutextures::setResolutionY(y);
		}

		//Load Standard Primitive Object
		if (strcmp(lineHeader, "c") == 0){
			card * c = new card();
			int matches = fscanf(file, "%i%s%i%i%i%i%i%i%i%i%s",
				&c->id, &c->cardname, &c->cost, &c->type, &c->hp,
				&c->attack, &c->defence, &c->evasion, &c->element, &c->draw, &c->description);
			//if (matches != 10){
			//	printf("Incorrect format of object in card file");
			//}
			//else {
				c->loadModel();
				cardlist[c->id] = *c;
			//}
		}
	}
	/////////////////////////////////////////////////////
}

void gamestate::loadCSV(std::string fileScene){
	std::ifstream  data(fileScene);

	std::string line;
	while (std::getline(data, line))
	{
		std::stringstream  lineStream(line);
		std::string        cell;
		std::string data[12];
		int id = 0;
		while (std::getline(lineStream, cell, ','))
		{
			data[id] = cell;
			id++;
		}
		if (strcmp(data[0].c_str(), "c") == 0){
			card * c = new card();
			stringstream(data[1]) >> c->id;
			stringstream(data[2]) >> c->cardname;
			stringstream(data[3]) >> c->cost;
			stringstream(data[4]) >> c->type;
			stringstream(data[5]) >> c->hp;
			stringstream(data[6]) >> c->attack;
			stringstream(data[7]) >> c->defence;
			stringstream(data[8]) >> c->evasion;
			stringstream(data[9]) >> c->element;
			stringstream(data[10]) >> c->draw;
			stringstream(data[11]) >> c->description;
			c->loadModel();
			cout << "Card Found" << c->id << endl;
			cardlist[c->id] = *c;
			cout << "Card List Found" << cardlist[c->id].attack << endl;
		}
	}
}

bool gamestate::cardActivated(int player, int cardID){
	card c = cardlist[cardID];
	card hero;
	if (player == 1){
		hero = heroStats.first;
		hero.cost = manaPoints.first;
	}
	else {
		hero = heroStats.second;
		hero.cost = manaPoints.second;
	}
	cout << "Activating card!" << endl;
		if (c.type == 1){ //WEAPON
			cout << "Weapon card!" << endl;
				if (player == 1) { 
					hero.attack -= weapons.first.attack;
					weapons.first = c; 
					hero.attack += weapons.first.attack;
				} else {
					hero.attack -= weapons.second.attack;
					weapons.second = c;
					hero.attack += weapons.second.attack;
				}
				hero.cost -= c.cost;
				loadWeapon();
		}
		else if (c.type == 2 || c.type == 3){ //UPDGRADE
			cout << "Upgrade card!" << endl;
				cout << "Stat Change!" << endl;
				hero.hp += c.attack;
				hero.attack += c.attack;
				hero.defence += c.defence;
				hero.evasion += c.evasion;
				hero.cost -= c.cost;
		}
		else if (c.type == 4){ //DOWNGRADE

		}
		else if (c.type == 5){ //STATUS/ADMIN CARDS

		}
		else {
			return false;
		}
	
	if (player == 1){
		heroStats.first = hero;
		manaPoints.first = hero.cost;
	}
	else {
		heroStats.second = hero;
		manaPoints.second = hero.cost;
	}
	return true;
}



void gamestate::cardAttack(){
	if (phase == 1){
		//heroStats.first.performAnimation();
		int attack = heroStats.first.attack + weapons.first.attack;
		if (weapons.first.id != 4){
			attack -= heroStats.second.defence;
		}
		if (attack <= 0){
			attack = 0;
		}
		if ((heroStats.second.evasion + weapons.second.evasion) <= 0 || weapons.first.id == 5){
			heroStats.second.hp -= attack;
		}
		else {
			heroStats.second.evasion--;
		}
	}
	else {
		//heroStats.second.performAnimation();
		int attack = heroStats.second.attack + weapons.second.attack;
		if (weapons.second.id != 4){
			attack -= heroStats.first.defence;
		}
		if (attack <= 0){
			attack = 0;
		}
		if ((heroStats.first.evasion + weapons.first.evasion) <= 0 || weapons.second.id == 5){
			heroStats.first.hp -= attack;
		}
		else {
			heroStats.first.evasion--;
		}
	}

}

void gamestate::loadWeapon(){
	cout << "Loading Weapon" << endl;
	if (phase == 1){
		cout << "P1" << endl;
		if (heroStats.first.id == 1){
			cout << "Robot 1" << endl;
			if (weapons.first.id == 4){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Broadsword.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.first.id == 5){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Dualswords.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(DualSwords).md5anim";
			}
			else if (weapons.first.id == 6){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Sword.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Punch(Uppercut).md5anim";
			}
		}
		else if (heroStats.first.id == 2){
			cout << "Robot 2" << endl;
			if (weapons.first.id == 4){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Broadsword.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.first.id == 5){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Dualswords.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(DualSwords).md5anim";
			}
			else if (weapons.first.id == 6){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh_Sword.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Alpha_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Alpha_Punch(Uppercut).md5anim";
			}
		}
		else if (heroStats.first.id == 3){
			cout << "Robot 3" << endl;
			if (weapons.first.id == 4){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Epsilon_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.first.id == 5){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Epsilon_Attack(DualSwords).md5anim";
			}
			else if (weapons.first.id == 6){
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Epsilon_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.first.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.first.id].animation = "../Assets/Animations/Epsilon_Punch(Uppercut).md5anim";
			}
		}
	}
	else {
		cout << "P2" << endl;
		if (heroStats.second.id == 1){
			if (weapons.second.id == 4){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Broadsword.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.second.id == 5){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Dualswords.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(DualSwords).md5anim";
			}
			else if (weapons.second.id == 6){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Sword.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Punch(Uppercut).md5anim";
			}
		}
		else if (heroStats.second.id == 2){
			if (weapons.second.id == 4){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Broadsword.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.second.id == 5){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Dualswords.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(DualSwords).md5anim";
			}
			else if (weapons.second.id == 6){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh_Sword.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Alpha_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Alpha_Punch(Uppercut).md5anim";
			}
		}
		else if (heroStats.second.id == 3){
			if (weapons.second.id == 4){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Epsilon_Attack(SwordAndShield).md5anim.md5anim";
			}
			else if (weapons.second.id == 5){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Epsilon_Attack(DualSwords).md5anim";
			}
			else if (weapons.second.id == 6){
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Epsilon_Attack(SwordAndShield).md5anim.md5anim";
			}
			else {
				cardlist[heroStats.second.id].model.loadModel("../Assets/Models/Epsilon_Mesh.md5mesh");
				cardlist[heroStats.second.id].animation = "../Assets/Animations/Epsilon_Punch(Uppercut).md5anim";
			}
		}

	}
}


bool gamestate::checkCard(int mode, int id){
	if (mode == 0 || mode == 3){
		return false;
	}
	else if (mode == 1 || mode == 2){
		if (id == 1 || id == 2 || id == 3){
			return true;
		}
		else {
			//Display Message
			return false;
		}
	}
	else if (mode == 4){
		if (id == 1 || id == 2 || id == 3){
			return false;
		}
		if (phase == 1){
			if (manaPoints.first >= cardlist[id].cost){
				return true;
			}
			else {
				//Not enough points
				return false;
			}
		}
		else {
			if (manaPoints.second >= cardlist[id].cost){
				return true;
			}
			else {
				//Not enough points
				return false;
			}
		}
	}
}