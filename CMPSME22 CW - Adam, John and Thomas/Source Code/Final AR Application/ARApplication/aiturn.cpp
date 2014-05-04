#include "aiturn.h"

vector<int> aiturn::cardsAvailable;
vector<int> aiturn::cardsInHand;
int aiturn::botID;

void aiturn::init() {
	//Select robot
	botID = chooseRobot();
	gamestate::heroStats.second = gamestate::cardlist[botID];

	//Initialise deck
	for (int i = 0; i < sizeof(gamestate::cardlist); i++) {
		if (gamestate::cardlist[i].type != 5 && gamestate::cardlist[i].type != 1) {
			cardsAvailable.push_back(gamestate::cardlist.at(i).id);
		}
	}

	//Draw first hand
	drawCard(2);
}

int aiturn::chooseRobot() {
	int bot;
	bool found = false;
	while (!found) {
		bot = rand() % 2 + 1;
		if (bot != gamestate::heroStats.first.id) {
			found = true;
		}
	}
	return bot;
}

void aiturn::drawCard(int numToDraw) {
	for (int i = 0; i < numToDraw; i++) {
		if (!cardsAvailable.empty()) {
			int index = rand() % cardsAvailable.size();
			int thisCard = cardsAvailable[index];
			cardsInHand.push_back(thisCard);
			cardsAvailable.erase(cardsAvailable.begin() + index);
		}
	}
}

int aiturn::chooseCard(int difficulty) {
	//Check for empty hand
	if (cardsInHand.empty()) {
		return -1;
	}

	int bestCard = -1;
	float score;
	float bestScore = -100;

	//Easy
	if (difficulty == 0) {
		vector<int> playableCards;
		for (int i = 0; i < cardsInHand.size(); i++) {
			card thisCard = gamestate::cardlist[cardsInHand[i]];
			if (gamestate::manaPoints.second >= thisCard.cost) {
				playableCards.push_back(cardsInHand[i]);
			}
		}
		if (!playableCards.empty()) {
			//bestCard = playableCards[rand() % playableCards.size()-1];
			bestCard = playableCards[0];
		}
	}
	//Hard
	else if (difficulty == 1) {
		//Score each card in hand based on which mech is being used
		for (int i = 0; i < cardsInHand.size(); i++) {
			card thisCard = gamestate::cardlist[cardsInHand[i]];
			if (gamestate::manaPoints.second >= thisCard.cost) {
				if (botID == 0) {
					score += thisCard.cost * -1;
					if (thisCard.type == 4)	{
						score += thisCard.cost;
					}
					if (thisCard.hp != 0) {
						if (gamestate::heroStats.second.hp + thisCard.hp > 0) {
							if (gamestate::heroStats.second.hp + thisCard.hp < (gamestate::heroStats.second.hp / 2)) {
								score += thisCard.hp;
							}
							else {
								score += thisCard.hp / 4;
							}
						}
						else {
							score -= 50;
						}
					}
					if (thisCard.attack != 0) {
						score += thisCard.attack;
					}
					if (thisCard.defence != 0) {
						score += thisCard.defence / 2;
					}
					if (thisCard.evasion != 0) {
						score += thisCard.evasion / 2;
					}
				}
				else if (botID == 1) {
					score += thisCard.cost * -1;
					if (thisCard.type == 4)	{
						score += thisCard.cost;
					}
					if (thisCard.hp != 0) {
						if (gamestate::heroStats.second.hp + thisCard.hp > 0) {
							if (gamestate::heroStats.second.hp + thisCard.hp < (gamestate::heroStats.second.hp / 2)) {
								score += thisCard.hp;
							}
							else {
								score += thisCard.hp / 2;
							}
						}
						else {
							score -= 50;
						}
					}
					if (thisCard.attack != 0) {
						score += thisCard.attack / 2;
					}
					if (thisCard.defence != 0) {
						score += thisCard.defence;
					}
					if (thisCard.evasion != 0) {
						score += thisCard.evasion / 1.5;
					}
				}
				else if (botID == 2) {
					score += thisCard.cost * -1;
					if (thisCard.type == 4)	{
						score += thisCard.cost;
					}
					if (thisCard.hp != 0) {
						if (gamestate::heroStats.second.hp + thisCard.hp > 0) {
							if (gamestate::heroStats.second.hp + thisCard.hp < (gamestate::heroStats.second.hp / 2)) {
								score += thisCard.hp;
							}
							else {
								score += thisCard.hp / 3;
							}
						}
						else {
							score -= 50;
						}
					}
					if (thisCard.attack != 0) {
						score += thisCard.attack / 1.5;
					}
					if (thisCard.defence != 0) {
						score += thisCard.defence / 2;
					}
					if (thisCard.evasion != 0) {
						score += thisCard.evasion;
					}
				}
				if (score > bestScore) {
					bestCard = cardsInHand[i];
				}
			}
		}
	}
	//Invalid difficulty selected
	else {
		return -1;
	}

	//Return card with highest score
	//if (bestCard != -1) {
	//		cardsInHand.erase(cardsInHand.begin() + bestCard);
	//}
	if (bestCard != -1) {
		for (unsigned i = 0; i < cardsInHand.size(); ++i){
			if (bestCard == cardsInHand[i]){
				cardsInHand.erase(cardsInHand.begin() + i);
			}
		}
	}
	cout << "bestcard " << bestCard << endl;
	return bestCard;
}