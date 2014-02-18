#include "gamestate.h"


gamestate::gamestate()
{
}


gamestate::~gamestate()
{
}

void gamestate::init(){
	turnID = 0;
	firstSecond = false;
	manaPoints.first = 0;
	manaPoints.second = 0;
	deckSize.first = 30;
	deckSize.second = 30;
	handSize.first = 0;
	handSize.second = 0;
	phase = 0; 
}

void gamestate::updateState(void){

}

int gamestate::currentState(){
	return phase;
}