#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "bg_menu.h"
#include "DrawBackground.hpp"

using namespace std;

// 1. Hàm in chữ có hộp nền tối (Dark Box) để chữ không cắn rách ảnh
void PrintTextWithBg(int startX, int startY, string text, int textColorCode) {
    int tr = 255, tg = 255, tb = 255; 
    if (textColorCode == 11) { tr = 0; tg = 255; tb = 255; }      
    else if (textColorCode == 14) { tr = 255; tg = 255; tb = 0; } 
    else if (textColorCode == 12) { tr = 255; tg = 0; tb = 0; }   

    GotoXY(startX, startY);
    // \x1b[48;2;15;15;20m là màu nền đen tuyền để lót dưới chữ
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;15;15;20m", tr, tg, tb);
    cout << text;
    printf("\x1b[0m"); 
}

// 2. Hàm vẽ background Nửa Khối siêu tốc
// void DrawBackground() {
//     GotoXY(0, 0); 
//     string buffer = ""; // Thùng chứa gộp lệnh in để chạy nhanh như chớp
//     char pixel[100];
    
//     // Vòng lặp chỉ chạy 40 lần (vừa khít chiều cao Console)
//     for (int i = 0; i < BG_FOREST_H / 2; i++) { 
//         for (int j = 0; j < BG_FOREST_W; j++) { 
//             // Gom 2 điểm ảnh Trên/Dưới của mảng 80 dòng vào 1 ký tự
//             int r_top = BG_FOREST[i * 2][j][0];
//             int g_top = BG_FOREST[i * 2][j][1];
//             int b_top = BG_FOREST[i * 2][j][2];
            
//             int r_bot = BG_FOREST[i * 2 + 1][j][0];
//             int g_bot = BG_FOREST[i * 2 + 1][j][1];
//             int b_bot = BG_FOREST[i * 2 + 1][j][2];
            
//             // In ký tự nửa khối mã UTF-8
//             sprintf(pixel, "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80", r_top, g_top, b_top, r_bot, g_bot, b_bot);
//             buffer += pixel;
//         }
        
//         // CHỈ XUỐNG DÒNG NẾU CHƯA PHẢI LÀ DÒNG CUỐI CÙNG
//         if (i < (BG_FOREST_H / 2) - 1) {
//             buffer += "\x1b[E"; 
//         }
//     }
//     buffer += "\x1b[0m"; // Reset màu
//     cout << buffer;
// }

// Gradient màu xanh băng từ trên xuống (7 màu cho 6 dòng ASCII art)
// Từ trắng băng → xanh cyan → xanh đậm
static const int TITLE_GRAD[7][3] = {
    {220, 240, 255},  // trắng băng (dòng 0)
    {140, 220, 255},  // xanh nhạt  (dòng 1)
    { 80, 200, 255},  // cyan sáng  (dòng 2)
    { 60, 180, 240},  // cyan       (dòng 3)
    { 40, 150, 220},  // xanh vừa  (dòng 4)
    { 20, 120, 200},  // xanh đậm  (dòng 5)
};
 
// Màu outline/shadow (xanh tím băng)
static const int SHADOW_COL[3] = {20, 60, 120};
 
void DrawTitleArt(int startX, int startY) {
    // ASCII art "CARO" font doom - 6 dòng
    const char* art[] = {
        " _____   ___  ______ _____ ",
        "/  __ \\ / _ \\ | ___ \\  _  |",
        "| /  \\// /_\\ \\| |_/ / | | |",
        "|    /|  _  ||    /| | | |",
        "| \\__/\\| | | || |\\ \\\\ \\_/ /",
        " \\____/\\_| |_/\\_| \\_|\\___/ ",
    };
    int nLines = 6;
 
    for (int i = 0; i < nLines; i++) {
        // Vẽ shadow (lệch 1 xuống 1 phải, màu tối)
        GotoXY(startX + 1, startY + i + 1);
        printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;5;10;25m%s\x1b[0m",
               SHADOW_COL[0], SHADOW_COL[1], SHADOW_COL[2], art[i]);
 
        // Vẽ chữ chính với gradient
        GotoXY(startX, startY + i);
        printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;5;10;25m%s\x1b[0m",
               TITLE_GRAD[i][0], TITLE_GRAD[i][1], TITLE_GRAD[i][2], art[i]);
    }
 
    // Vẽ border băng bên dưới chữ
    // int borderY = startY + nLines + 1;
    // int artWidth = 28; // độ rộng art
    // GotoXY(startX - 1, borderY);
    // printf("\x1b[38;2;80;200;255m\x1b[48;2;5;10;25m");
    // cout << "+" ;
    // for (int k = 0; k < artWidth; k++) cout << "~";
    // cout << "+";
    // printf("\x1b[0m");
}

int GenericMenu(string options[], int size, string title) {
    int currentSelect = 0;

    system("cls");
    DrawMenuBackground(); 

    while (true) {
        if (title == "GAME CARO") {
        DrawTitleArt(42, 3);   // startX=76 để căn giữa màn 280 cols
                               // startY=3  để có khoảng cách với mép trên
    } else {
        // Các menu khác vẫn dùng box bình thường
        PrintTextWithBg(40, 5, "==============================", 11);
        int padding = (26 - title.length()) / 2;
        string titleLine = "||" + string(padding, ' ') + title + string(26 - title.length() - padding, ' ') + "||";
        PrintTextWithBg(40, 6, titleLine, 11);
        PrintTextWithBg(40, 7, "==============================", 11);
    }


        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                string opt = ">> " + options[i] + " <<";
                PrintTextWithBg(45, 11 + i * 2, opt, 14); // Đang trỏ: Vàng
            }
            else {
                string opt = "   " + options[i] + "   ";
                PrintTextWithBg(45, 11 + i * 2, opt, 11); // Thường: Cyan
            }
        }

        int key = toupper(_getch());
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
        }
        else if (key == 13) { 
            return currentSelect;
        }
    }
}

int MainMenu() {
    string options[4] = { "1. Play Game", "2. Load Game", "3. Settings", "4. Exit" };
    return GenericMenu(options, 4, "GAME CARO");
}

int PlayGameMenu() {
    string options[3] = { "1. Player vs Player", "2. Player vs Bot", "3. Quay lai" };
    return GenericMenu(options, 3, "CHON CHE DO");
}

int DifficultyMenu() {
    string options[4] = { "1. De (Easy)", "2. Trung binh (Medium)", "3. Kho (Hard)", "4. Quay lai" };
    return GenericMenu(options, 4, "CHON DO KHO");
}

int SettingsMenu() {
    string options[2] = { "1. Clear Data (Xoa toan bo Save)", "2. Quay lai" };
    return GenericMenu(options, 2, "CAI DAT");
}

int GameMenu() {
    string options[4] = { "1. Thoat game","2. Luu game","3. Tai game","4. Thoat menu"};
    return GenericMenu(options, 4, "MENU");
}

string TypeName() {
    string res = "";
    while (true) {
        char c = _getch();
        if (c == 13 && res.length() > 0) { 
            break;
        }
        else if (c == 8) { 
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        else if ((isalnum(c)) && res.length() < 15) {
            res += c;
            cout << c;
        }
    }
    return res;
}

void InputPlayerNames(bool isBotMode) {
    system("cls");
    DrawMenuBackground(); 
    
    PrintTextWithBg(35, 10, "Nhap ten Nguoi choi 1 (X): ", 14); 
    GotoXY(62, 10); 
    UnhideCursor();
    
    // Lấy màu nền chỗ con trỏ chuột
    int r1 = BG_MENU[10 * 2][62][0];
    int g1 = BG_MENU[10 * 2][62][1];
    int b1 = BG_MENU[10 * 2][62][2];
    printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r1, g1, b1);
    
    _PLAYER1_NAME = TypeName();
    printf("\x1b[0m");

    if (isBotMode) {
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";
    }
    else {
        PrintTextWithBg(35, 12, "Nhap ten Nguoi choi 2 (O): ", 14);
        GotoXY(62, 12);
        
        int r2 = BG_MENU[12][62 / 2][0];
        int g2 = BG_MENU[12][62 / 2][1];
        int b2 = BG_MENU[12][62 / 2][2];
        printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r2, g2, b2);
        
        _PLAYER2_NAME = TypeName();
        printf("\x1b[0m");

        while (_PLAYER1_NAME == _PLAYER2_NAME) {
            PrintTextWithBg(35, 13, "Ten trung voi Player 1! Vui long nhap lai...", 12);
            _getch();

            PrintTextWithBg(35, 13, "                                            ", 15);
            PrintTextWithBg(62, 12, "               ", 15);

            PrintTextWithBg(35, 12, "Nhap lai ten Nguoi choi 2 (O): ", 14);
            GotoXY(66, 12);
            
            int r3 = BG_MENU[12][66 / 2][0];
            int g3 = BG_MENU[12][66 / 2][1];
            int b3 = BG_MENU[12][66 / 2][2];
            printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r3, g3, b3);
            
            _PLAYER2_NAME = TypeName();
            printf("\x1b[0m");
        }
    } 
    HideCursor();
}