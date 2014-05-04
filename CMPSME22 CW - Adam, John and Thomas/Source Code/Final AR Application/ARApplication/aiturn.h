#pragma once

#include <random>
#include <vector>
#include "gamestate.h"
#include "card.h"

using namespace std;

class aiturn
{
private:
	static int chooseRobot();
	static vector<int> cardsAvailable;
	static vector<int> cardsInHand;
public:
	static int botID;

	static void init();
	static void drawCard(int numToDraw);
	static int chooseCard(int difficulty);
};

