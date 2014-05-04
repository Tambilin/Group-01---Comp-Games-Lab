#pragma once
#include <iostream>
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

	card();
	~card();
};

