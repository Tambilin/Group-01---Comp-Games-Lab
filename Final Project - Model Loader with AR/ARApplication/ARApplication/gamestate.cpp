#include "gamestate.h"

int gamestate::turnID = 1;
bool gamestate::firstSecond = false;
std::pair<int, int> gamestate::manaPoints = std::make_pair(0, 0);
std::pair<int, int> gamestate::deckSize = std::make_pair(30, 30);
std::pair<int, int> gamestate::handSize = std::make_pair(0, 0);
std::pair<card, card> gamestate::heroStats;
std::unordered_map< int, card > gamestate::cardlist;
int gamestate::phase = 1;
int gamestate::lastPlayedID = -1;

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

void gamestate::cardActivated(int player, int cardID){

}