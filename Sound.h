#pragma once
#pragma comment(lib, "winmm.lib")
#include<iostream>
#include<windows.h>
#include<mmsystem.h>
#include<vector>
#include<string>
#include<cwchar>

using namespace std;

void PlayMusic(int choice, int volume);
void setVolume();
bool MusicStatus();

void PlayMenuSound();
void PlayWinSound();
void setSFXVolume(int volume);

extern int volumeLevel;
extern int sfxVolumeLevel;
extern bool isSFXOn;
