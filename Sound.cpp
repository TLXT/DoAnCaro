#include "Sound.h"
#include <thread>

int volumeLevel = 50;
int sfxVolumeLevel = 800;
bool isSFXOn = true;

static bool FileExists(const wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

static wstring GetExecutableDirectory() {
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return L".";

    wstring path(buffer, len);
    size_t slash = path.find_last_of(L"\\/");
    if (slash == wstring::npos) return L".";
    return path.substr(0, slash);
}

static wstring JoinPath(const wstring& dir, const wstring& file) {
    if (dir.empty() || dir == L".") return file;
    wchar_t last = dir[dir.length() - 1];
    if (last == L'\\' || last == L'/') return dir + file;
    return dir + L"\\" + file;
}

static wstring ResolveAssetPath(const wstring& fileName) {
    wstring exeDir = GetExecutableDirectory();
    vector<wstring> searchDirs = {
        L".",
        exeDir,
        JoinPath(exeDir, L".."),
        JoinPath(exeDir, L"..\\.."),
        JoinPath(exeDir, L"..\\..\\..")
    };

    for (const wstring& dir : searchDirs) {
        wstring candidate = JoinPath(dir, fileName);
        if (FileExists(candidate)) return candidate;
    }

    return L"";
}

void PlayMusic(int choice, int volume) {
    if (choice == 5) {
        mciSendStringW(L"close myMusic", NULL, 0, NULL);
        return;
    }

    vector<wstring> musicFiles = {
        L"music1.mp3",
        L"music2.mp3",
        L"music3.mp3",
        L"music4.mp3",
        L"music5.mp3"
    };
    if (choice < 0 || choice >= static_cast<int>(musicFiles.size())) return;

    mciSendStringW(L"close myMusic", NULL, 0, NULL);

    wstring fileName = ResolveAssetPath(musicFiles[choice]);
    if (fileName.empty()) return;

    wstring openCommand = L"open \"" + fileName + L"\" type mpegvideo alias myMusic";
    MCIERROR err = mciSendStringW(openCommand.c_str(), NULL, 0, NULL);
    if (err != 0) return;

    wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volume);
    mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
    mciSendStringW(L"play myMusic repeat", NULL, 0, NULL);
}

void setVolume() {
    wstring volumeCommand = L"setaudio myMusic volume to " + to_wstring(volumeLevel);
    mciSendStringW(volumeCommand.c_str(), NULL, 0, NULL);
}

bool MusicStatus() {
    wchar_t status[128] = {};
    MCIERROR err = mciSendStringW(L"status myMusic mode", status, 128, NULL);
    return err == 0 && wcscmp(status, L"playing") == 0;
}

void PlayMenuSound() {
    if (!isSFXOn) return;

    wstring fileName = ResolveAssetPath(L"menu_sound_effect.wav");
    if (fileName.empty()) return;

    PlaySoundW(fileName.c_str(), NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
}

void PlayWinSound() {
    if (!isSFXOn) return;

    std::thread([]() {
        Beep(659, 140);
        Beep(784, 140);
        Beep(988, 180);
        Beep(1319, 260);
        Beep(988, 140);
        Beep(1319, 360);
    }).detach();
}

void PlayCountdownBeep() {
    if (!isSFXOn) return;

    std::thread([]() {
        Beep(880, 90);
    }).detach();
}

void setSFXVolume(int volume) {
    sfxVolumeLevel = volume;
}
