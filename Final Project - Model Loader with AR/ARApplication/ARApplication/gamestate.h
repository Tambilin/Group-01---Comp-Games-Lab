#pragma once
#include <iostream>
#include <string> 
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "card.h"
#include "menutextures.h"
#include "soundeffect.h"
using namespace std;

class gamestate
{
private:
	static int oldTimeSinceStart;
public:
	static int turnID;
	static bool firstSecond;
	static std::pair<int, int> manaPoints;
	static std::pair<int, int> deckSize;
	static std::pair<int, int> handSize;
	static std::pair<card, card> heroStats;
	static std::pair<card, card> weapons;
	static std::unordered_map< int, card > cardlist;
	static int phase;
	static int lastPlayedID;
	static int winner;
	static int deltaTime;
	static bool confirmed;
	static int attacking;
	// declaration of players positions array
	static float mech1Position[6];
	static float mech2Position[6];
	static soundeffect * t;
	static double frustrum3D;
	static bool activate3D;

	static void setHero(int player, int cardid);
	static void init();
	static void updateState(void);
	static int currentState();
	static void nextState();
	static void loadStateData(std::string fileScene);
	static void loadCSV(std::string fileScene);
	static bool cardActivated(int player, int cardID);
	static void cardAttack();
	static void loadWeapon();
	static bool checkCard(int mode, int id);
	static void updateDeltaTime();
};

