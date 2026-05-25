#pragma once
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <future>
#include "GameStatus.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "FinishProcess.h"
#include "Menu.h"
#include "SaveLoad.h"
#include "UserInfo.h"
#include "DrawBoard.h"
#include "CaroBot.h"
#include "GameTimer.h"
#include "Replay.h"
#include"Sound.h"

void loadmusic();
bool loadGameMenu(bool& isPlaying);
void loadSettingMenu();
void loadBotMove(bool& isPlaying);