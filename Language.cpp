#include "Language.h"

static GameLanguage gLanguage = Vietnamese;

GameLanguage GetLanguage() {
    return gLanguage;
}

void SetLanguage(GameLanguage language) {
    gLanguage = language;
}

void ToggleLanguage() {
    gLanguage = (gLanguage == Vietnamese) ? English : Vietnamese;
}

std::string CurrentLanguageName() {
    return (gLanguage == Vietnamese) ? u8"TI\u1EBENG VI\u1EC6T" : "ENGLISH";
}

std::string L(TextId id) {
    const bool vi = (gLanguage == Vietnamese);

    switch (id) {
    case MainPlayGame: return vi ? u8"CH\u01A0I" : "PLAY GAME";
    case MainLoadGame: return vi ? u8"T\u1EA2I V\u00C1N" : "LOAD GAME";
    case MainGuide: return vi ? u8"H\u01AF\u1EDANG D\u1EAAN" : "GUIDE";
    case MainSettings: return vi ? u8"C\u00C0I \u0110\u1EB6T" : "SETTINGS";
    case MainExit: return vi ? u8"THO\u00C1T" : "EXIT";
    case ModeTitle: return vi ? u8"CH\u1ECCN CH\u1EBE \u0110\u1ED8" : "SELECT MODE";
    case ModePvp: return vi ? u8"NG\u01AF\u1EDCI \u0110\u1EA4U NG\u01AF\u1EDCI" : "PLAYER VS PLAYER";
    case ModePvb: return vi ? u8"NG\u01AF\u1EDCI \u0110\u1EA4U BOT" : "PLAYER VS BOT";
    case Back: return vi ? u8"QUAY L\u1EA0I" : "BACK";
    case DifficultyTitle: return vi ? u8"CH\u1ECCN \u0110\u1ED8 KH\u00D3" : "SELECT DIFFICULTY";
    case DifficultyEasy: return vi ? u8"D\u1EC4" : "EASY";
    case DifficultyMedium: return vi ? u8"TRUNG B\u00CCNH" : "MEDIUM";
    case DifficultyHard: return vi ? u8"KH\u00D3" : "HARD";
    case SettingsTitle: return vi ? u8"C\u00C0I \u0110\u1EB6T" : "SETTINGS";
    case SettingsClearData: return vi ? u8"X\u00D3A D\u1EEE LI\u1EC6U" : "CLEAR DATA";
    case SettingsMusic: return vi ? u8"C\u00C0I \u0110\u1EB6T NH\u1EA0C" : "MUSIC SETTINGS";
    case SettingsLanguage: return vi ? u8"NG\u00D4N NG\u1EEE: " : "LANGUAGE: ";
    case SettingsAbout: return vi ? u8"GI\u1EDAI THI\u1EC6U" : "ABOUT GAME";
    case PauseExitGame: return vi ? u8"THO\u00C1T TR\u00D2 CH\u01A0I" : "EXIT GAME";
    case PauseSaveGame: return vi ? u8"L\u01AFU V\u00C1N" : "SAVE GAME";
    case PauseLoadGame: return vi ? u8"T\u1EA2I V\u00C1N" : "LOAD GAME";
    case PauseExitMenu: return vi ? u8"THO\u00C1T MENU" : "CLOSE MENU";
    case MusicOff: return vi ? u8"T\u1EAET NH\u1EA0C" : "MUSIC OFF";
    case MusicOn: return vi ? u8"B\u1EACT NH\u1EA0C" : "MUSIC ON";
    case MusicVolume: return vi ? u8"THAY \u0110\u1ED4I \u00C2M L\u01AF\u1EE2NG" : "CHANGE VOLUME";
    case MusicSfx: return vi ? u8"HI\u1EC6U \u1EE8NG \u00C2M THANH: " : "SOUND EFFECTS: ";
    case MusicExit: return vi ? u8"THO\u00C1T MENU" : "BACK";
    case VolumeTitle: return vi ? u8"\u00C2M L\u01AF\u1EE2NG" : "VOLUME";
    case VolumeHelp: return vi ? u8"A/D HO\u1EB6C M\u0168I T\u00CAN TR\u00C1I/PH\u1EA2I \u0110\u1EC2 CH\u1EC8NH, ENTER \u0110\u1EC2 L\u01AFU" : "A/D OR LEFT/RIGHT TO ADJUST, ENTER TO SAVE";
    case VolumeLabel: return vi ? u8"\u00C2M L\u01AF\u1EE2NG: " : "VOLUME: ";
    case EnterPlayerX: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (X): " : "ENTER PLAYER NAME (X): ";
    case EnterPlayerO: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (O): " : "ENTER PLAYER NAME (O): ";
    case EnterPlayerBot: return vi ? u8"NH\u1EACP T\u00CAN NG\u01AF\u1EDCI CH\u01A0I (M\u00C1Y): " : "PLAYER NAME (BOT): ";
    case DuplicateName: return vi ? u8"T\u00CAN B\u1ECA TR\u00D9NG V\u1EDAI NG\u01AF\u1EDCI CH\u01A0I 1! NH\u1EA4N PH\u00CDM B\u1EA4T K\u1EF2 \u0110\u1EC2 NH\u1EACP L\u1EA0I..." : "NAME MATCHES PLAYER 1! PRESS ANY KEY TO ENTER AGAIN...";
    case ChooseCharacterP1: return vi ? u8"NG\u01AF\u1EDCI CH\u01A0I 1 \u0110ANG CH\u1ECCN NH\u00C2N V\u1EACT" : "PLAYER 1 IS CHOOSING CHARACTER";
    case ChooseCharacterP2: return vi ? u8"NG\u01AF\u1EDCI CH\u01A0I 2 \u0110ANG CH\u1ECCN NH\u00C2N V\u1EACT" : "PLAYER 2 IS CHOOSING CHARACTER";
    case Chosen: return vi ? u8"\u0110\u00C3 CH\u1ECCN" : "CHOSEN";
    case HudPause: return vi ? u8"T\u1EA0M D\u1EEANG" : "PAUSE";
    case HudMenu: return "MENU";
    case HudTime: return vi ? u8"TH\u1EDCI GIAN: " : "TIME: ";
    case HudTurn: return vi ? u8"L\u01AF\u1EE2T \u0110I: " : "TURN: ";
    case HudStatusPaused: return vi ? u8"TR\u1EA0NG TH\u00C1I: T\u1EA0M D\u1EEANG" : "STATUS: PAUSED";
    case HudStatusPlaying: return vi ? u8"TR\u1EA0NG TH\u00C1I: \u0110ANG CH\u01A0I" : "STATUS: PLAYING";
    case BotThinking: return vi ? u8"BOT \u0110ANG SUY NGH\u0128..." : "BOT IS THINKING...";
    case ReplayQuestion: return vi ? u8"XEM L\u1EA0I V\u00C1N \u0110\u1EA4U?" : "WATCH REPLAY?";
    case PlayAgainQuestion: return vi ? u8"CH\u01A0I L\u1EA0I KH\u00D4NG?" : "PLAY AGAIN?";
    case Yes: return vi ? u8"C\u00D3" : "YES";
    case No: return vi ? u8"KH\u00D4NG" : "NO";
    case Victory: return "VICTORY";
    case Winner: return vi ? u8"NG\u01AF\u1EDCI TH\u1EAENG: " : "WINNER: ";
    case Symbol: return vi ? u8"K\u00DD HI\u1EC6U: " : "SYMBOL: ";
    case Character: return vi ? u8"NH\u00C2N V\u1EACT: " : "CHARACTER: ";
    case Draw: return "DRAW";
    case DrawMessage: return vi ? u8"HAI NG\u01AF\u1EDCI CH\u01A0I \u0110\u00C3 H\u00D2A NHAU." : "THE GAME ENDED IN A DRAW.";
    case ReplaySpeedPrompt: return vi ? u8"CH\u1ECCN T\u1ED0C \u0110\u1ED8 XEM L\u1EA0I (M\u0168I T\u00CAN HO\u1EB6C A/D)" : "SELECT REPLAY SPEED (ARROWS OR A/D)";
    case ReplayHelpBack: return vi ? u8"<- / A : TUA L\u00D9I 5S" : "<- / A : BACK 5S";
    case ReplayHelpForward: return vi ? u8"-> / D : TUA T\u1EDAI 5S" : "-> / D : FORWARD 5S";
    case ReplayHelpPause: return vi ? u8"SPACE/P: D\u1EEANG/PH\u00C1T" : "SPACE/P: PAUSE/PLAY";
    case ReplayHelpExit: return vi ? u8"ESC    : THO\u00C1T" : "ESC    : EXIT";
    case SavePrompt: return vi ? u8"NH\u1EACP T\u00CAN T\u1EC6P \u0110\u1EC2 L\u01AFU: " : "ENTER SAVE FILE NAME: ";
    case SaveDuplicate: return vi ? u8"T\u00CAN T\u1EC6P \u0110\u00C3 T\u1ED2N T\u1EA0I. VUI L\u00D2NG NH\u1EACP T\u00CAN KH\u00C1C." : "FILE ALREADY EXISTS. ENTER ANOTHER NAME.";
    case SaveSuccessTitle: return vi ? u8"L\u01AFU TH\u00C0NH C\u00D4NG" : "SAVE COMPLETE";
    case SaveSuccess: return vi ? u8"V\u00C1N \u0110\u1EA4U \u0110\u00C3 \u0110\u01AF\u1EE2C L\u01AFU AN TO\u00C0N." : "THE GAME HAS BEEN SAVED.";
    case SaveCreateErrorTitle: return vi ? u8"KH\u00D4NG TH\u1EC2 L\u01AFU" : "SAVE FAILED";
    case SaveCreateError: return vi ? u8"KH\u00D4NG TH\u1EC2 T\u1EA0O T\u1EC6P L\u01AFU." : "CANNOT CREATE THE SAVE FILE.";
    case LoadReadErrorTitle: return vi ? u8"KH\u00D4NG TH\u1EC2 T\u1EA2I" : "LOAD FAILED";
    case LoadReadError: return vi ? u8"KH\u00D4NG TH\u1EC2 \u0110\u1ECCC T\u1EC6P L\u01AFU." : "CANNOT READ THE SAVE FILE.";
    case LoadNoDataTitle: return vi ? u8"CH\u01AFA C\u00D3 D\u1EEE LI\u1EC6U" : "NO SAVE DATA";
    case LoadNoData: return vi ? u8"CH\u01AFA C\u00D3 V\u00C1N \u0110\u1EA4U N\u00C0O \u0110\u01AF\u1EE2C L\u01AFU." : "THERE ARE NO SAVED GAMES YET.";
    case LoadTitle: return vi ? u8"DANH S\u00C1CH V\u00C1N \u0110\u00C3 L\u01AFU" : "SAVED GAMES";
    case LoadHelp: return vi ? u8"(W/S: CH\u1ECCN | ENTER: T\u1EA2I V\u00C1N | X: X\u00D3A T\u1EC6P | ESC: H\u1EE6Y)" : "(W/S: SELECT | ENTER: LOAD GAME | X: DELETE FILE | ESC: CANCEL)";
    case NoticeContinue: return vi ? u8"NH\u1EA4N PH\u00CDM B\u1EA4T K\u1EF2 \u0110\u1EC2 TI\u1EBEP T\u1EE4C" : "PRESS ANY KEY TO CONTINUE";
    case ClearNoDataTitle: return vi ? u8"KH\u00D4NG C\u00D3 D\u1EEE LI\u1EC6U" : "NO DATA";
    case ClearNoData: return vi ? u8"KH\u00D4NG C\u00D3 V\u00C1N \u0110\u00C3 L\u01AFU N\u00C0O \u0110\u1EC2 X\u00D3A." : "THERE ARE NO SAVED GAMES TO DELETE.";
    case ClearConfirmPrefix: return vi ? u8"B\u1EA0N C\u00D3 CH\u1EAEC MU\u1ED0N X\u00D3A TO\u00C0N B\u1ED8 " : "DELETE ALL ";
    case ClearConfirmSuffix: return vi ? u8" T\u1EC6P L\u01AFU?" : " SAVE FILES?";
    case ClearDoneTitle: return vi ? u8"\u0110\u00C3 X\u00D3A D\u1EEE LI\u1EC6U" : "DATA DELETED";
    case ClearDone: return vi ? u8"TO\u00C0N B\u1ED8 D\u1EEE LI\u1EC6U L\u01AFU \u0110\u00C3 \u0110\u01AF\u1EE2C X\u00D3A." : "ALL SAVE DATA HAS BEEN DELETED.";
    case ClearCanceledTitle: return vi ? u8"\u0110\u00C3 H\u1EE6Y THAO T\u00C1C" : "ACTION CANCELED";
    case ClearCanceled: return vi ? u8"D\u1EEE LI\u1EC6U L\u01AFU V\u1EABN \u0110\u01AF\u1EE2C GI\u1EEF NGUY\u00CAN." : "SAVE DATA WAS KEPT UNCHANGED.";
    }

    return "";
}
