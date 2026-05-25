#include"Character.h"
#include "GameStatus.h"
#include"ControlConsole.h"

 int CharacterASelect =-1;
 int CharacterBSelect =-1;
void outsidedisplay(int option) {
	if (option == 0) {
		Knight a;
		a.DrawOnChoosingMenu();
	}
	else if (option == 1) {
		Assassin a ;
		a.DrawOnChoosingMenu();
	}
	else if (option == 2) {
		Vampire a;
		a.DrawOnChoosingMenu();
	}
	else if (option == 3) {
		Paladin a ;
		a.DrawOnChoosingMenu();
	}
	else if (option == 4) {
		Officer a;
		a.DrawOnChoosingMenu();
	}
}
//nhận option từ menu chọn nhân vật 
void ingamedisplay(int option,bool isPlayer1 ) {
	if (isPlayer1) {
		if (option == 0) {
			Knight a(_PLAYER1_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 1) {
			Assassin a(_PLAYER1_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 2) {
			Vampire a(_PLAYER1_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 3) {
			Paladin a(_PLAYER1_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 4) {
			Officer a(_PLAYER1_NAME);
			a.DrawOnChoosingMenu();
		}
	}
	else {
		if (option == 0) {
			Knight a(_PLAYER2_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 1) {
			Assassin a(_PLAYER2_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 2) {
			Vampire a(_PLAYER2_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 3) {
			Paladin a(_PLAYER2_NAME);
			a.DrawOnChoosingMenu();
		}
		else if (option == 4) {
			Officer a(_PLAYER2_NAME);
			a.DrawOnChoosingMenu();
		}
	}
}

void Character::DrawOnChoosingMenu(){
	GotoXY(4, 4);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<vector<int>> a = GetDisplay();
	for (const auto& row : a) {
		for (int color : row) {
			if (color == -1) {
				cout << "  ";
			}
			else {
				SetConsoleTextAttribute(h, color);
				cout << (char)219 << (char)219;
			}
		}
		SetConsoleTextAttribute(h, 8);
		cout << '\n';
	}
	SetConsoleTextAttribute(h, 8);
	GotoXY(14, 24);
	cout << GetName();
}
void Character::DrawLeftSizeInGame() {
	GotoXY(4, 4);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<vector<int>> a = GetDisplay();
	for (const auto& row : a) {
		for (int color : row) {
			if (color == -1) {
				cout << "  ";
			}
			else {
				SetConsoleTextAttribute(h, color);
				cout << (char)219 << (char)219;
			}
		}
		SetConsoleTextAttribute(h, 8);
		cout << '\n';
	}
	SetConsoleTextAttribute(h, 8);
	GotoXY(14, 24);
	cout << GetName();
}
void Character::DrawRightSizeInGame() {
	GotoXY(70, 4);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<vector<int>> a = GetDisplay();
	for (const auto& row : a) {
		for (int color : row) {
			if (color == -1) {
				cout << "  ";
			}
			else {
				SetConsoleTextAttribute(h, color);
				cout << (char)219 << (char)219;
			}
		}
		SetConsoleTextAttribute(h, 8);
		cout << '\n';
	}
	SetConsoleTextAttribute(h, 8);
	GotoXY(80, 24);
	cout << GetName();
}
