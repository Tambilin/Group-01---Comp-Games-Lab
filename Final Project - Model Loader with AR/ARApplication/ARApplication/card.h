#pragma once
#include <iostream>
#include "md5load.h"
using namespace std;

class card
{
private:

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
	md5load model;

	void loadModel();
	void drawModel();
	card();
	~card();
};

