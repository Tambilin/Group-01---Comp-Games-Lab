#pragma once
#include <iostream>
using namespace std;

class card
{
private:
	int type;
	int hp;
	int attack;
	int defence;
	int evasion;
	int element;
	int draw;
	int mana;
	std::string description;

public:
	card();
	~card();
};

