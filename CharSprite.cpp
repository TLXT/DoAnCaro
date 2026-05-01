#include "CharSprite.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <iostream>
#include <conio.h>
using namespace std;

// ============================================================
//  CharSprite.cpp
// ============================================================

// Danh sách nhân vật có thể chọn
const CharOption CHAR_LIST[] = {
    { "Chien Binh (X)", SPRITE_WARRIOR },
    { "Phap Su  (O)",   SPRITE_WIZARD  },
    // Thêm nhân vật mới tại đây sau khi chạy Python:
    // { "Ten nhan vat", TEN_SPRITE },
};
const int CHAR_COUNT = sizeof(CHAR_LIST) / sizeof(CHAR_LIST[0]);

int _PLAYER1_CHAR = 0;  // Mặc định nhân vật 1 = WARRIOR
int _PLAYER2_CHAR = 1;  // Mặc định nhân vật 2 = WIZARD

// ============================================================
// DrawSprite: Vẽ sprite tại (startX, startY) trên console
// Mỗi pixel = 2 ký tự space với màu nền (bgColor)
// Pixel có màu SPRITE_TRANSPARENT thì bỏ qua (giữ nền)
// ============================================================
void DrawSprite(const int sprite[][SPRITE_W], int h, int startX, int startY) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            int colorIdx = sprite[y][x];
            GotoXY(startX + x * 2, startY + y);
            if (colorIdx == SPRITE_TRANSPARENT) {
                // Giữ màu nền mặc định (trắng)
                SetColor(0, 15);
                cout << "  ";
            }
            else {
                // Vẽ 2 ký tự space với màu nền = colorIdx
                SetColor(colorIdx, colorIdx);
                cout << "  ";
            }
        }
    }
    SetColor(0, 15);  // Reset màu
}

// ============================================================
// ClearSprite: Xóa vùng sprite (tô trắng)
// ============================================================
void ClearSprite(int startX, int startY, int w, int h) {
    for (int y = 0; y < h; y++) {
        GotoXY(startX, startY + y);
        SetColor(0, 15);
        for (int x = 0; x < w * 2; x++) cout << " ";
    }
}

// ============================================================
// DrawBothSprites: Vẽ cả 2 nhân vật lên màn hình game
// Gọi sau StartGame() để hiển thị sprite 2 bên bàn cờ
// ============================================================
void DrawBothSprites() {
    const int* p1Sprite = reinterpret_cast<const int*>(CHAR_LIST[_PLAYER1_CHAR].sprite);
    const int* p2Sprite = reinterpret_cast<const int*>(CHAR_LIST[_PLAYER2_CHAR].sprite);

    DrawSprite(CHAR_LIST[_PLAYER1_CHAR].sprite, SPRITE_H, LEFT_SPRITE_X, SPRITE_START_Y);
    DrawSprite(CHAR_LIST[_PLAYER2_CHAR].sprite, SPRITE_H, RIGHT_SPRITE_X, SPRITE_START_Y);

    SetColor(0, 15);
    // Vẽ tên nhân vật bên dưới sprite
    GotoXY(LEFT_SPRITE_X, SPRITE_START_Y + SPRITE_H + 1);
    cout << CHAR_LIST[_PLAYER1_CHAR].name;
    GotoXY(RIGHT_SPRITE_X, SPRITE_START_Y + SPRITE_H + 1);
    cout << CHAR_LIST[_PLAYER2_CHAR].name;
}

// ============================================================
// CharacterSelectMenu: Menu chọn nhân vật trước khi chơi
// Hiển thị preview sprite bên trái/phải khi di chuyển chọn
// ============================================================
void CharacterSelectMenu() {
    // --- Chọn nhân vật cho Player 1 ---
    system("cls");
    system("color F0");
    int sel1 = 0;

    while (true) {
        system("cls");
        SetColor(12, 15);
        GotoXY(40, 2); cout << "=== CHON NHAN VAT CHO PLAYER 1 (X) ===";
        GotoXY(40, 3); cout << "  (W/S de chon, Enter de xac nhan)   ";

        // Hiển thị danh sách
        for (int i = 0; i < CHAR_COUNT; i++) {
            GotoXY(42, 6 + i * 2);
            if (i == sel1) {
                SetColor(0, 11);
                cout << ">> " << CHAR_LIST[i].name << " <<";
            }
            else {
                SetColor(0, 15);
                cout << "   " << CHAR_LIST[i].name << "   ";
            }
        }

        // Preview sprite nhân vật đang chọn
        SetColor(0, 15);
        GotoXY(10, 5);  cout << "Preview:";
        DrawSprite(CHAR_LIST[sel1].sprite, SPRITE_H, 10, 6);

        SetColor(0, 15);
        int key = toupper(_getch());
        if (key == 0 || key == 224) key = _getch(); // phím mũi tên

        if (key == 'W' || key == 72) {
            sel1--;
            if (sel1 < 0) sel1 = CHAR_COUNT - 1;
        }
        else if (key == 'S' || key == 80) {
            sel1++;
            if (sel1 >= CHAR_COUNT) sel1 = 0;
        }
        else if (key == 13) {
            _PLAYER1_CHAR = sel1;
            break;
        }
    }

    // --- Chọn nhân vật cho Player 2 (hoặc Bot) ---
    // Nếu Bot Mode thì tự chọn nhân vật cho bot
    extern bool _BOT_MODE;
    if (_BOT_MODE) {
        // Bot tự dùng nhân vật còn lại
        _PLAYER2_CHAR = (_PLAYER1_CHAR == 0) ? 1 : 0;
        return;
    }

    int sel2 = (_PLAYER1_CHAR == 0) ? 1 : 0;  // Gợi ý nhân vật khác

    while (true) {
        system("cls");
        SetColor(1, 15);
        GotoXY(40, 2); cout << "=== CHON NHAN VAT CHO PLAYER 2 (O) ===";
        GotoXY(40, 3); cout << "  (W/S de chon, Enter de xac nhan)   ";

        for (int i = 0; i < CHAR_COUNT; i++) {
            GotoXY(42, 6 + i * 2);
            if (i == sel2) {
                SetColor(0, 11);
                cout << ">> " << CHAR_LIST[i].name << " <<";
            }
            else {
                SetColor(0, 15);
                cout << "   " << CHAR_LIST[i].name << "   ";
            }
        }

        SetColor(0, 15);
        GotoXY(10, 5); cout << "Preview:";
        DrawSprite(CHAR_LIST[sel2].sprite, SPRITE_H, 10, 6);

        int key = toupper(_getch());
        if (key == 0 || key == 224) key = _getch();

        if (key == 'W' || key == 72) {
            sel2--;
            if (sel2 < 0) sel2 = CHAR_COUNT - 1;
        }
        else if (key == 'S' || key == 80) {
            sel2++;
            if (sel2 >= CHAR_COUNT) sel2 = 0;
        }
        else if (key == 13) {
            _PLAYER2_CHAR = sel2;
            break;
        }
    }
}
