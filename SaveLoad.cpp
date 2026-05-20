#include "SaveLoad.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "DrawBoard.h"
#include "GamePlay.h"
#include "UserInfo.h"

#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"

using namespace std;

string TypeFileName() {
    string res = "";
    while (true) {
        char c = _getch();

        if (c == 27) { // BẤM ESC
            return "";
        }
        else if (c == 13 && res.length() > 0) {
            break;
        }
        else if (c == 8) { // Bấm Backspace
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        else if (isalnum(c) || c == '-' || c == '_') {
            res += c;
            cout << c;
        }
    }
    return res;
}
string SaveGame() {
    string filename;
    while (true) {
        GotoXY(5, 27);
        cout << "                                                                    ";

        GotoXY(5, 27);
        SetColor(12, 15);
        cout << "Nhap ten file de luu: ";

        UnhideCursor();
        filename = TypeFileName();
        HideCursor();

        if (filename == "") {
            GotoXY(5, 27); cout << "                                                                    ";
            GotoXY(5, 28); cout << "                                                                    ";
            SetColor(0, 15);
            GotoXY(_X, _Y);
            return"";
        }

        ifstream checkFile(filename + ".caro");
        if (checkFile.is_open()) {
            checkFile.close();

            GotoXY(5, 28);
            SetColor(12, 15); // Màu đỏ
            cout << "Ten file da ton tai! Vui long nhap ten khac... (Nhan phim bat ky)";
            _getch();

            GotoXY(5, 28);
            cout << "                                                                    ";
        }
        else {
            break;
        }
    }

    ofstream outFile(filename + ".caro");
    if (outFile.is_open()) {
        outFile << _TURN << " " << _X << " " << _Y << endl;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                outFile << _A[i][j].c << " ";
            }
            outFile << endl;
        }
        outFile << currentStep << endl; //lưu số bước hiện tại
        for (int i = 0; i < currentStep; i++) {
            //lưu tọa độ dòng, cột và loại quân cờ (-1 hoặc 1) của từng bước đi
            outFile << moveHistory[i].row << " " << moveHistory[i].col << " " << moveHistory[i].c << endl;
        }
        outFile.close();

        GotoXY(5, 28);
        SetColor(10, 15);
        cout << "Luu thanh cong! Nhan phim bat ky de tiep tuc...";
        return filename;
    }
    else {
        GotoXY(5, 28);
        SetColor(12, 15);
        cout << "Loi tao file! Nhan phim bat ky...";
    }

    _getch();

    GotoXY(5, 27); cout << "                                                                    ";
    GotoXY(5, 28); cout << "                                                                    ";
    SetColor(0, 15);
    GotoXY(_X, _Y);
}

vector<string> GetSaveFiles() {
    vector<SaveFileInfo> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("*.caro", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string fileName = findFileData.cFileName;
            fileName = fileName.substr(0, fileName.find_last_of("."));

            // Lấy thời gian file được lưu cuối cùng
            ULARGE_INTEGER fileTime;
            fileTime.LowPart = findFileData.ftLastWriteTime.dwLowDateTime;
            fileTime.HighPart = findFileData.ftLastWriteTime.dwHighDateTime;

            files.push_back({ fileName, fileTime });
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    // Sắp xếp file theo thời gian lưu (Giảm dần: Mới nhất lên đầu)
    sort(files.begin(), files.end(), [](const SaveFileInfo& a, const SaveFileInfo& b) {
        return a.time.QuadPart > b.time.QuadPart;
        });

    vector<string> fileList;
    for (const auto& item : files) {
        fileList.push_back(item.name);
    }

    return fileList;
}


bool LoadGame() {
    string filename = ChooseFileMenu();

    if (filename == "") {
        return false;
    }

    ifstream inFile(filename + ".caro");
    if (inFile.is_open()) {
        inFile >> _TURN >> _X >> _Y;

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                _A[i][j].x = 4 * j + LEFT + 2;
                _A[i][j].y = 2 * i + TOP + 1;
                inFile >> _A[i][j].c;
            }
        }
        moveHistory.clear(); //xóa lịch sử cũ trước khi load
        if (inFile >> currentStep) { //kiểm tra xem file save có dữ liệu lịch sử không
            for (int i = 0; i < currentStep; i++) {
                MoveNode node;
                inFile >> node.row >> node.col >> node.c;
                moveHistory.push_back(node);
            }
        }
        else {
            currentStep = 0; //tránh lỗi crash nếu đọc nhầm file save cũ (file được tạo trước khi sửa code)
        }
        inFile.close();

        system("cls");
        DrawBoard(BOARD_SIZE);
        DrawPlayerInfo();
        UpdateTurnInfo();

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                DrawCell(_A[i][j].x, _A[i][j].y, 15);
            }
        }
        DrawCell(_X, _Y, 11);

        return true;
    }
    else {
        system("cls");
        GotoXY(40, 15);
        SetColor(12, 15);
        cout << "Loi doc file! Nhan phim bat ky de thoat...";
        _getch();
        return false;
    }
}

string ChooseFileMenu() {
    while (true) {
        vector<string> files = GetSaveFiles();
        if (files.empty()) {
            system("cls");
            DrawFrame(40, 12, 40, 5);
            GotoXY(45, 14); SetColor(12, 15); cout << "Chua co du lieu Save!";
            _getch();
            return "";
        }

        int currentSelect = 0;
        int maxDisplay = 5; // Chỉ hiển thị tối đa 5 file cùng lúc để không vỡ khung
        int startIndex = 0; // Vị trí file bắt đầu hiển thị (dùng để cuộn)

        int lastSelect = -1;
        int lastStartIndex = -1;
        bool isLooping = true;

        int consoleW = 120;
        string title = "DANH SACH CAC VAN DA LUU";
        int frameW = title.length() + 8;
        int btnCols = BTN_NORMAL_W * 2;
        int startX = (consoleW - btnCols) / 2;
        int startY_Base = 10;

        int bgNorm = BTN_NORMAL[BTN_NORMAL_H / 2][BTN_NORMAL_W / 2];
        int bgHov = BTN_HOVER[BTN_HOVER_H / 2][BTN_HOVER_W / 2];

        while (isLooping) {
            // NẾU BỊ CUỘN TRANG (thay đổi startIndex): Xóa và vẽ lại toàn bộ khung
            if (startIndex != lastStartIndex) {
                system("cls");
                DrawFrame((consoleW - frameW) / 2, 2, frameW, 5);
                GotoXY((consoleW - frameW) / 2 + 4, 4);
                SetColor(12, 15); cout << title;

                GotoXY((consoleW - 65) / 2, startY_Base + maxDisplay * (BTN_NORMAL_H + 1) + 2);
                SetColor(8, 15);
                // cout << "(W/S: Chon | Enter: Tai | X: Xoa | ESC: Huy | Tong: " << files.size() << " files)";

                lastStartIndex = startIndex;
                lastSelect = -1; // Ép vẽ lại toàn bộ nút
            }

            // NẾU CÓ SỰ DỊCH CHUYỂN NÚT BẤM (Cập nhật hiển thị Hover)
            if (currentSelect != lastSelect) {
                int endIdx = min(startIndex + maxDisplay, (int)files.size());

                for (int i = startIndex; i < endIdx; i++) {
                    int rowOffset = i - startIndex; // Tính toán xem nút sẽ nằm ở dòng thứ mấy trên màn hình
                    int startY = startY_Base + rowOffset * (BTN_NORMAL_H + 1);

                    if (i == currentSelect) {
                        DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, startX, startY);
                        GotoXY(startX + (btnCols - files[i].length()) / 2, startY + BTN_HOVER_H / 2);
                        SetColor(0, bgHov); cout << files[i];
                    }
                    else {
                        DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, startX, startY);
                        GotoXY(startX + (btnCols - files[i].length()) / 2, startY + BTN_NORMAL_H / 2);
                        SetColor(0, bgNorm); cout << files[i];
                    }
                }
                lastSelect = currentSelect;
            }

            // Xử lý phím
            SetColor(0, 15);
            int key = toupper(_getch());
            if (key == 0 || key == 224) key = toupper(_getch());

            if (key == 27) return ""; // ESC
            else if (key == 'W' || key == 72) {
                currentSelect--;
                if (currentSelect < 0) currentSelect = files.size() - 1;
            }
            else if (key == 'S' || key == 80) {
                currentSelect++;
                if (currentSelect >= files.size()) currentSelect = 0;
            }
            else if (key == 13) return files[currentSelect]; // Enter
            else if (key == 'X') {
                string fileToDelete = files[currentSelect] + ".caro";
                DeleteFileA(fileToDelete.c_str());
                isLooping = false; // Bấm xóa sẽ kích hoạt reset lại file
            }

            // LOGIC CUỘN TRANG
            // Nếu con trỏ chuột nhảy lên trên khỏi giới hạn hiển thị
            if (currentSelect < startIndex) {
                startIndex = currentSelect;
            }
            // Nếu con trỏ chuột nhảy xuống dưới khỏi giới hạn hiển thị
            else if (currentSelect >= startIndex + maxDisplay) {
                startIndex = currentSelect - maxDisplay + 1;
            }
        }
    }
}

void ClearAllData() {
    vector<string> files = GetSaveFiles();
    if (files.empty()) {
        system("cls");
        DrawFrame(40, 12, 40, 5);
        GotoXY(45, 14); SetColor(12, 15); cout << "Khong co du lieu luu nao de xoa!";
        _getch();
        return;
    }

    string prompt = "Ban co chac muon xoa TOAN BO " + to_string(files.size()) + " file luu?";
    bool confirm = GraphicalYesNo(prompt, 10, true, BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);

    system("cls");
    DrawFrame(35, 12, 50, 5);
    if (confirm) {
        for (string file : files) {
            string fullPath = file + ".caro";
            DeleteFileA(fullPath.c_str());
        }
        GotoXY(40, 14); SetColor(10, 15); cout << "Da xoa thanh cong toan bo du lieu!";
    }
    else {
        GotoXY(42, 14); SetColor(8, 15); cout << "Da huy thao tac xoa.";
    }
    _getch();
}
bool loadPresent() {
    system("cls");
    DrawBoard(BOARD_SIZE);
    DrawPlayerInfo();
    UpdateTurnInfo();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            DrawCell(_A[i][j].x, _A[i][j].y, 15);
    DrawCell(_X, _Y, 11);
    return true;
}