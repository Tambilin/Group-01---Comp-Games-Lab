#pragma once
#include <iostream>
#include "md5load.h"
using namespace std;

class card
{
private:
	bool attacking;
public:
	int id;
	std::string cardname;
	int cost;
	int type;
	int hp;
	int attack;
	int defence;
	int evasion;
	int element;
	int draw;
	std::string description;
	const char * animation;
	const char * animationDodge;
	const char * animationUpgrade;
	const char * animationVictory;
	const char * animationWalk;
	md5load model;

	void loadModel();
	void loadAnimation();
	void drawModel();
	void drawModel(int x, int y, int z, int angle, int i, int j, int k);
	void performAnimation();
	card();
	~card();
};

