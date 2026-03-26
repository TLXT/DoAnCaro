#include <iostream>
#include <conio.h>
#include <Windows.h>
#include<future>

#include "GameStatus.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "FinishProcess.h"
#include "Menu.h"
#include "SaveLoad.h"
#include "UserInfo.h"
#include "DrawBoard.h"
#include "CaroBot.h"


using namespace std;

int main() {
    FixConsoleWindow();
    SetConsoleWindow(1000, 600);
    HideCursor();

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;
        if (choice == 0) { // CHỌN: PLAY GAME
            int mode = PlayGameMenu();

            if (mode == 0) {
                _BOT_MODE = false;
                StartGame();
                isPlaying = true;
            }
            else if (mode == 1) {
                int diff = DifficultyMenu();
                if (diff == 3) {
                    continue;
                }

                _BOT_MODE = true;
                _BOT_DIFFICULTY = diff + 1;

                StartGame();
                isPlaying = true;
            }
            else if (mode == 2) {

                continue;
            }
        }
        else if (choice == 1) { // CHỌN: LOAD GAME
            if (LoadGame() == true) {
                isPlaying = true;
            }
        }
        else if (choice == 2) { // CHỌN: SETTINGS
            while (true) {
                int setChoice = SettingsMenu();

                if (setChoice == 0) {
                    ClearAllData();
                }
                else if (setChoice == 1) {
                    break;
                }
            }
        }
        else if (choice == 3) { // CHỌN: EXIT
            ExitGame();
            return 0;
        }

        if (isPlaying) {
            bool validEnter = true;

            while (isPlaying) {
                if (_BOT_MODE == true && _TURN == false) {

                    GotoXY(60, 18);
                    SetColor(12, 15);
                    cout << "Bot dang suy nghi...         ";

                    _POINT botMove = FindBotMove(1, _BOT_DIFFICULTY);

                    if (botMove.x != -1) {
                        DrawCell(_X, _Y, 15);
                        _X = botMove.x;
                        _Y = botMove.y;

                        CheckBoard(_X, _Y);
                        DrawCell(_X, _Y, 11);

                        GotoXY(60, 18);
                        SetColor(0, 15);
                        cout << "                             ";

                        switch (ProcessFinish(TestBoard())) {
                        case -1: case 1: case 0:
                            if (AskContinue() != 'Y') {
                                isPlaying = false;
                            }
                            else {
                                StartGame();
                            }

                        }
                    }
                    continue;
                }

                _COMMAND = toupper(_getch());
                if (_COMMAND == 'M' || _COMMAND == 77) {
                    future<int> SecondThreadchoice = async (launch::async, GameMenu);
                    int gamechoice = SecondThreadchoice.get();
                    if (gamechoice == 0) { // thoat
                        isPlaying = false;
                    }
                    else if (gamechoice == 1) {
                        string temp=SaveGame();
                        LoadGame(temp);
                    }
                    else if (gamechoice == 2) {
                        loadPresent();
                    }
                    else if (gamechoice == 4) {
                        loadPresent();
                    }
                }
                else {
                    if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                    else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                    else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
                    else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
                    else if (_COMMAND == 13) {
                        switch (CheckBoard(_X, _Y)) {
                        case -1: DrawCell(_X, _Y, 11); break;
                        case 1:  DrawCell(_X, _Y, 11); break;
                        case 0:  validEnter = false;
                        }

                        if (validEnter == true) {
                            switch (ProcessFinish(TestBoard())) {
                            case -1: case 1: case 0:
                                if (AskContinue() != 'Y') {
                                    isPlaying = false;
                                }
                                else {
                                    StartGame();
                                }
                            }
                        }
                        validEnter = true;
                    }
                }
            }
        }
    }
    return 0;
}