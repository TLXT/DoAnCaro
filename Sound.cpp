#include"Sound.h"
void PlayMusic(int choice, int volume) {
	if (choice == 5) {
		mciSendStringW(L"close myMusic", NULL, 0, NULL);
		return;
	}
	vector<string> musicFiles = {
		"music1.mp3",
		"music2.mp3",
		"music3.mp3",
		"music4.mp3",
		"music5.mp3"
	};
	mciSendStringW(L"close myMusic", NULL, 0, NULL);
	wstring fileName(musicFiles[choice].begin(), musicFiles[choice].end());
	wstring command = L"open \"" + fileName + L"\" type mpegvideo alias Mymusic";
	wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volume);
	//volume nhạn giá trị từ 500-1000
	mciSendStringW(command.c_str(), NULL, 0, NULL);
	MCIERROR err = mciSendStringW(command.c_str(), NULL, 0, NULL);
	mciSendStringW(L"play myMusic repeat", NULL, 0, NULL);
	mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
}

void setVolume(int choice)
{
	int volume = choice * 10; 
	PlayMusic(0, volume); 
	wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volume);
	mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
}
