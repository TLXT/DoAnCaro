#include "Language.h"

static GameLanguage gLanguage = GameLanguage::Vietnamese;

GameLanguage GetLanguage() {
    return gLanguage;
}

void SetLanguage(GameLanguage language) {
    gLanguage = language;
}

void ToggleLanguage() {
    gLanguage = (gLanguage == GameLanguage::Vietnamese) ? GameLanguage::English : GameLanguage::Vietnamese;
}

std::string CurrentLanguageName() {
    return (gLanguage == GameLanguage::Vietnamese) ? u8"TI\u1EBENG VI\u1EC6T" : "ENGLISH";
}

std::string L(TextId id) {
    const bool vi = (gLanguage == GameLanguage::Vietnamese);

    switch (id) {
    case TextId::MainPlayGame: return vi ? u8"CH\u01A0I" : "PLAY GAME";
    case TextId::MainLoadGame: return vi ? u8"T\u1EA2I V\u00C1N" : "LOAD GAME";
    case TextId::MainGuide: return vi ? u8"H\u01AF\u1EDANG D\u1EAAN" : "GUIDE";
    case TextId::MainSettings: return vi ? u8"C\u00C0I \u0110\u1EB6T" : "SETTINGS";
    case TextId::MainExit: return vi ? u8"THO\u00C1T" : "EXIT";
    case TextId::ModeTitle: return vi ? u8"CH\u1ECCN CH\u1EBE \u0110\u1ED8" : "SELECT MODE";
    case TextId::ModePvp: return vi ? u8"NG\u01AF\u1EDCI \u0110\u1EA4U NG\u01AF\u1EDCI" : "PLAYER VS PLAYER";
    case TextId::ModePvb: return vi ? u8"NG\u01AF\u1EDCI \u0110\u1EA4U BOT" : "PLAYER VS BOT";
    case TextId::Back: return vi ? u8"QUAY L\u1EA0I" : "BACK";
    case TextId::DifficultyTitle: return vi ? u8"CH\u1ECCN \u0110\u1ED8 KH\u00D3" : "SELECT DIFFICULTY";
    case TextId::DifficultyEasy: return vi ? u8"D\u1EC4" : "EASY";
    case TextId::DifficultyMedium: return vi ? u8"TRUNG B\u00CCNH" : "MEDIUM";
    case TextId::DifficultyHard: return vi ? u8"KH\u00D3" : "HARD";
    case TextId::SettingsTitle: return vi ? u8"C\u00C0I \u0110\u1EB6T" : "SETTINGS";
    case TextId::SettingsClearData: return vi ? u8"X\u00D3A D\u1EEE LI\u1EC6U" : "CLEAR DATA";
    case TextId::SettingsMusic: return vi ? u8"C\u00C0I \u0110\u1EB6T NH\u1EA0C" : "MUSIC SETTINGS";
    case TextId::SettingsLanguage: return vi ? u8"NG\u00D4N NG\u1EEE: " : "LANGUAGE: ";
    case TextId::SettingsAbout: return vi ? u8"GI\u1EDAI THI\u1EC6U" : "ABOUT GAME";
    case TextId::PauseExitGame: return vi ? u8"THO\u00C1T TR\u00D2 CH\u01A0I" : "EXIT GAME";
    case TextId::PauseSaveGame: return vi ? u8"L\u01AFU V\u00C1N" : "SAVE GAME";
    case TextId::PauseLoadGame: return vi ? u8"T\u1EA2I V\u00C1N" : "LOAD GAME";
    case TextId::PauseExitMenu: return vi ? u8"THO\u00C1T MENU" : "CLOSE MENU";
    case TextId::MusicOff: return vi ? u8"T\u1EAET NH\u1EA0C" : "MUSIC OFF";
    case TextId::MusicOn: return vi ? u8"B\u1EACT NH\u1EA0C" : "MUSIC ON";
    case TextId::MusicVolume: return vi ? u8"THAY \u0110\u1ED4I \u00C2M L\u01AF\u1EE2NG" : "CHANGE VOLUME";
    case TextId::MusicSfx: return vi ? u8"HI\u1EC6U \u1EE8NG \u00C2M THANH: " : "SOUND EFFECTS: ";
    case TextId::MusicExit: return vi ? u8"THO\u00C1T MENU" : "BACK";
    case TextId::VolumeTitle: return vi ? u8"\u00C2M L\u01AF\u1EE2NG" : "VOLUME";
    case TextId::VolumeHelp: return vi ? u8"A/D HO\u1EB6C M\u0168I T\u00CAN TR\u00C1I/PH\u1EA2I \u0110\u1EC2 CH\u1EC8NH, ENTER \u0110\u1EC2 L\u01AFU" : "A/D OR LEFT/RIGHT TO ADJUST, ENTER TO SAVE";
    case TextId::VolumeLabel: return vi ? u8"\u00C2M L\u01AF\u1EE2NG: " : "VOLUME: ";
    case TextId::EnterPlayerX: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (X): " : "ENTER PLAYER NAME (X): ";
    case TextId::EnterPlayerO: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (O): " : "ENTER PLAYER NAME (O): ";
    case TextId::EnterPlayerBot: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (M\u00C1Y): " : "PLAYER NAME (BOT): ";
    case TextId::DuplicateName: return vi ? u8"T\u00CAN B\u1ECA TR\u00D9NG V\u1EDAI NG\u01AF\u1EDCI CH\u01A0I 1! NH\u1EA4N PH\u00CDM B\u1EA4T K\u1EF2 \u0110\u1EC2 NH\u1EACP L\u1EA0I..." : "NAME MATCHES PLAYER 1! PRESS ANY KEY TO ENTER AGAIN...";
    case TextId::ChooseCharacterP1: return vi ? u8"NG\u01AF\u1EDCI CH\u01A0I 1 \u0110ANG CH\u1ECCN NH\u00C2N V\u1EACT" : "PLAYER 1 IS CHOOSING CHARACTER";
    case TextId::ChooseCharacterP2: return vi ? u8"NG\u01AF\u1EDCI CH\u01A0I 2 \u0110ANG CH\u1ECCN NH\u00C2N V\u1EACT" : "PLAYER 2 IS CHOOSING CHARACTER";
    case TextId::Chosen: return vi ? u8"\u0110\u00C3 CH\u1ECCN" : "CHOSEN";
    case TextId::HudPause: return vi ? u8"T\u1EA0M D\u1EEANG" : "PAUSE";
    case TextId::HudMenu: return "MENU";
    case TextId::HudTime: return vi ? u8"TH\u1EDCI GIAN: " : "TIME: ";
    case TextId::HudTurn: return vi ? u8"L\u01AF\u1EE2T \u0110I: " : "TURN: ";
    case TextId::HudStatusPaused: return vi ? u8"TR\u1EA0NG TH\u00C1I: T\u1EA0M D\u1EEANG" : "STATUS: PAUSED";
    case TextId::HudStatusPlaying: return vi ? u8"TR\u1EA0NG TH\u00C1I: \u0110ANG CH\u01A0I" : "STATUS: PLAYING";
    case TextId::BotThinking: return vi ? u8"BOT \u0110ANG SUY NGH\u0128..." : "BOT IS THINKING...";
    case TextId::ReplayQuestion: return vi ? u8"XEM L\u1EA0I V\u00C1N \u0110\u1EA4U?" : "WATCH REPLAY?";
    case TextId::PlayAgainQuestion: return vi ? u8"CH\u01A0I L\u1EA0I KH\u00D4NG?" : "PLAY AGAIN?";
    case TextId::Yes: return vi ? u8"C\u00D3" : "YES";
    case TextId::No: return vi ? u8"KH\u00D4NG" : "NO";
    case TextId::Victory: return "VICTORY";
    case TextId::Winner: return vi ? u8"NG\u01AF\u1EDCI TH\u1EAENG: " : "WINNER: ";
    case TextId::Symbol: return vi ? u8"K\u00DD HI\u1EC6U: " : "SYMBOL: ";
    case TextId::Character: return vi ? u8"NH\u00C2N V\u1EACT: " : "CHARACTER: ";
    case TextId::Draw: return "DRAW";
    case TextId::DrawMessage: return vi ? u8"HAI NG\u01AF\u1EDCI CH\u01A0I \u0110\u00C3 H\u00D2A NHAU." : "THE GAME ENDED IN A DRAW.";
    case TextId::ReplaySpeedPrompt: return vi ? u8"CH\u1ECCN T\u1ED0C \u0110\u1ED8 XEM L\u1EA0I (M\u0168I T\u00CAN HO\u1EB6C A/D)" : "SELECT REPLAY SPEED (ARROWS OR A/D)";
    case TextId::ReplayHelpBack: return vi ? u8"<- / A : TUA L\u00D9I 5S" : "<- / A : BACK 5S";
    case TextId::ReplayHelpForward: return vi ? u8"-> / D : TUA T\u1EDAI 5S" : "-> / D : FORWARD 5S";
    case TextId::ReplayHelpPause: return vi ? u8"SPACE/P: D\u1EEANG/PH\u00C1T" : "SPACE/P: PAUSE/PLAY";
    case TextId::ReplayHelpExit: return vi ? u8"ESC    : THO\u00C1T" : "ESC    : EXIT";
    case TextId::SavePrompt: return vi ? u8"NH\u1EACP T\u00CAN T\u1EC6P \u0110\u1EC2 L\u01AFU: " : "ENTER SAVE FILE NAME: ";
    case TextId::SaveDuplicate: return vi ? u8"T\u00CAN T\u1EC6P \u0110\u00C3 T\u1ED2N T\u1EA0I. VUI L\u00D2NG NH\u1EACP T\u00CAN KH\u00C1C." : "FILE ALREADY EXISTS. ENTER ANOTHER NAME.";
    case TextId::SaveSuccessTitle: return vi ? u8"L\u01AFU TH\u00C0NH C\u00D4NG" : "SAVE COMPLETE";
    case TextId::SaveSuccess: return vi ? u8"V\u00C1N \u0110\u1EA4U \u0110\u00C3 \u0110\u01AF\u1EE2C L\u01AFU AN TO\u00C0N." : "THE GAME HAS BEEN SAVED.";
    case TextId::SaveCreateErrorTitle: return vi ? u8"KH\u00D4NG TH\u1EC2 L\u01AFU" : "SAVE FAILED";
    case TextId::SaveCreateError: return vi ? u8"KH\u00D4NG TH\u1EC2 T\u1EA0O T\u1EC6P L\u01AFU." : "CANNOT CREATE THE SAVE FILE.";
    case TextId::LoadReadErrorTitle: return vi ? u8"KH\u00D4NG TH\u1EC2 T\u1EA2I" : "LOAD FAILED";
    case TextId::LoadReadError: return vi ? u8"KH\u00D4NG TH\u1EC2 \u0110\u1ECCC T\u1EC6P L\u01AFU." : "CANNOT READ THE SAVE FILE.";
    case TextId::LoadNoDataTitle: return vi ? u8"CH\u01AFA C\u00D3 D\u1EEE LI\u1EC6U" : "NO SAVE DATA";
    case TextId::LoadNoData: return vi ? u8"CH\u01AFA C\u00D3 V\u00C1N \u0110\u1EA4U N\u00C0O \u0110\u01AF\u1EE2C L\u01AFU." : "THERE ARE NO SAVED GAMES YET.";
    case TextId::LoadTitle: return vi ? u8"DANH S\u00C1CH V\u00C1N \u0110\u00C3 L\u01AFU" : "SAVED GAMES";
    case TextId::LoadHelp: return vi ? u8"(W/S: CH\u1ECCN | ENTER: T\u1EA2I V\u00C1N | X: X\u00D3A T\u1EC6P | ESC: H\u1EE6Y)" : "(W/S: SELECT | ENTER: LOAD GAME | X: DELETE FILE | ESC: CANCEL)";
    case TextId::NoticeContinue: return vi ? u8"NH\u1EA4N PH\u00CDM B\u1EA4T K\u1EF2 \u0110\u1EC2 TI\u1EBEP T\u1EE4C" : "PRESS ANY KEY TO CONTINUE";
    case TextId::ClearNoDataTitle: return vi ? u8"KH\u00D4NG C\u00D3 D\u1EEE LI\u1EC6U" : "NO DATA";
    case TextId::ClearNoData: return vi ? u8"KH\u00D4NG C\u00D3 V\u00C1N \u0110\u00C3 L\u01AFU N\u00C0O \u0110\u1EC2 X\u00D3A." : "THERE ARE NO SAVED GAMES TO DELETE.";
    case TextId::ClearConfirmPrefix: return vi ? u8"B\u1EA0N C\u00D3 CH\u1EAEC MU\u1ED0N X\u00D3A TO\u00C0N B\u1ED8 " : "DELETE ALL ";
    case TextId::ClearConfirmSuffix: return vi ? u8" T\u1EC6P L\u01AFU?" : " SAVE FILES?";
    case TextId::ClearDoneTitle: return vi ? u8"\u0110\u00C3 X\u00D3A D\u1EEE LI\u1EC6U" : "DATA DELETED";
    case TextId::ClearDone: return vi ? u8"TO\u00C0N B\u1ED8 D\u1EEE LI\u1EC6U L\u01AFU \u0110\u00C3 \u0110\u01AF\u1EE2C X\u00D3A." : "ALL SAVE DATA HAS BEEN DELETED.";
    case TextId::ClearCanceledTitle: return vi ? u8"\u0110\u00C3 H\u1EE6Y THAO T\u00C1C" : "ACTION CANCELED";
    case TextId::ClearCanceled: return vi ? u8"D\u1EEE LI\u1EC6U L\u01AFU V\u1EABN \u0110\u01AF\u1EE2C GI\u1EEF NGUY\u00CAN." : "SAVE DATA WAS KEPT UNCHANGED.";
    }

    return "";
}
