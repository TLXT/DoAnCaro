#include"Sound.h"
int volumeLevel = 50;
int sfxVolumeLevel = 800;
bool isSFXOn = true;

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
	wstring command = L"open \"" + fileName + L"\" type mpegvideo alias myMusic";
	wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volume);
	//volume nhạn giá trị từ 0-1000
	//mciSendStringW(command.c_str(), NULL, 0, NULL); 
	//Tao thấy cái này hơi trùng nên cmt nó nếu ko ảnh hưởng j thì m xóa nha
	MCIERROR err = mciSendStringW(command.c_str(), NULL, 0, NULL);
	mciSendStringW(L"play myMusic repeat", NULL, 0, NULL);
	mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
}

void setVolume()
{
	wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volumeLevel);
	mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
}
bool MusicStatus() {
	wchar_t status[128];
	MCIERROR err = mciSendStringW(L"status myMusic mode", status, 128, NULL);

	if (err == 0) {
		return true;
	}
	return false;
}

void PlayMenuSound() {
	if (!isSFXOn) return;

	// SND_ASYNC: Phát không đợi file chạy hết (không làm treo game)
	// SND_FILENAME: Tham số đầu là tên file
	// SND_NODEFAULT: Nếu không tìm thấy file thì im lặng (không kêu bíp mặc định)
	PlaySound(TEXT("menu_sound_effect.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
}

void setSFXVolume(int volume) {
	sfxVolumeLevel = volume;
	string volCmd = "setaudio menuSound volume to " + to_string(sfxVolumeLevel);
	mciSendStringA(volCmd.c_str(), NULL, 0, NULL);
}