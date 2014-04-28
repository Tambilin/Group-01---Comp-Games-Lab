#include "aiturn.h"

vector<int> aiturn::cardsAvailable;
vector<int> aiturn::cardsInHand;
int aiturn::botID;

void aiturn::init() {
	//Select robot
	botID = 2;
	gamestate::heroStats.second = gamestate::cardlist[botID];

	//Initialise deck
	for (int i = 4; i < sizeof(gamestate::cardlist); i++) {
		if (gamestate::cardlist[i].type != 5) {
			cardsAvailable.push_back(i);
		}
	}

	//Draw first hand
	drawCard(2);
}

int aiturn::chooseRobot() {
	return rand() % 3 + 1;
}

void aiturn::drawCard(int numToDraw) {
	for (int i = 0; i < numToDraw; i++) {
		if (!cardsAvailable.empty()) {
			int thisCard = rand() % (cardsAvailable.size() + 1);
			cardsInHand.push_back(thisCard);
			cardsAvailable.erase(cardsAvailable.begin() + (thisCard - 1));
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
			return rand() % (playableCards.size() + 1);
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
						score += thisCard.hp / 4;
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
						score += thisCard.hp / 2;
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
						score += thisCard.hp / 3;
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
	return bestCard;
}