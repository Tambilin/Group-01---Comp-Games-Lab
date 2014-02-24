#include "gamestate.h"

int gamestate::turnID = 0;
bool gamestate::firstSecond = false;
std::pair<int, int> gamestate::manaPoints = std::make_pair(0, 0);
std::pair<int, int> gamestate::deckSize = std::make_pair(30, 30);
std::pair<int, int> gamestate::handSize = std::make_pair(0, 0);
std::pair<card, card> gamestate::heroStats;
int gamestate::phase = 0;

gamestate::gamestate()
{
}


gamestate::~gamestate()
{
}

void gamestate::init(){

}

void gamestate::updateState(void){

}

int gamestate::currentState(){
	return phase;
}