#pragma once
#include <string>
#include <vector>

using namespace std;

extern int CharacterASelect;
extern int CharacterBSelect;

int GetCharacterCount();
const vector<vector<int>>& GetCharacterPixels(int option);
string GetCharacterName(int option);
void outsidedisplay(int option);
void ingamedisplay(int option, bool isPlayer1);
void DrawIngameTurnBadges();
void DrawBothSprites();
void DrawCharacterPreview(int option, int x, int y, int boxW, int boxH, bool selected, bool disabled);
void DrawCharacterVictoryCard(int option, int x, int y, int boxW, int boxH);
