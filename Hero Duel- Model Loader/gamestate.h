#pragma once
#include <iostream>
#include "card.h"
using namespace std;

class gamestate
{
public:
	static int turnID;
	static bool firstSecond;
	static std::pair<int, int> manaPoints;
	static std::pair<int, int> deckSize;
	static std::pair<int, int> handSize;
	static std::pair<card, card> heroStats;
	static int phase;


	static void init();
	static void updateState(void);
	static int currentState();
	static void nextState();
	gamestate();
	~gamestate();
};

