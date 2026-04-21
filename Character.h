#pragma once
#include<vector>
#include<iostream>

using namespace std;

int CharacterASelect = -1;
int CharacterBSelect = -1;

//class Character {
//protected:
//	vector<vector<int>> display;
//	string name;
//public:
//	void DrawOnChoosingMenu();
//	void DrawInGamingPhrase();
//};
//class Knight : public  Character {
//public:
//	Knight() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Knight";
//	}
//	Knight(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//class Assassin :public Character {
//public:
//	Assassin() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Assassin";
//	}
//	Assassin(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//class Vampire :public Character {
//public:
//	Vampire() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Vampire";
//	}
//	Vampire(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//class Elf :public Character {
//public:
//	Elf() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Elf";
//	}
//	Elf(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//class Werewolf :public Character {
//public:
//	Werewolf() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Werewolf";
//	}
//	Werewolf(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//class Berserker : public Character {
//public:
//	Berserker() {
//		display = {
//			{0, 1, 0},
//			{1, 1, 1},
//			{0, 1, 0}
//		};
//		name = "Berserker";
//	}
//	Berserker(string n) {
//		display = {
//				{0, 1, 0},
//				{1, 1, 1},
//				{0, 1, 0}
//		};
//		name = n;
//	}
//};
//void inGameDisplay(int option) {};