#include "CaroBot.h"
#include <vector>
#include <random>
#include <unordered_map>
#include <algorithm>
#include <limits>

using namespace std;

const int WIN_SCORE = 100000000;

uint64_t zobristTable[BOARD_SIZE][BOARD_SIZE][3];

struct TranspositionEntry {
    uint64_t hash;
    int depth;
    int score;
    int flag;
};

const int TRANSPOSITION_TABLE_SIZE = 1000000;
unordered_map<uint64_t, TranspositionEntry> transpositionTable;

void InitZobrist() {
    static bool initialized = false;
    if (initialized) return;

    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < 3; k++) {
                zobristTable[i][j][k] = dis(gen);
            }
        }
    }
    initialized = true;
}

uint64_t ComputeBoardHash() {
    uint64_t hash = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = _A[i][j].c;
            if (piece != 0) {
                int pieceType = (piece == 1) ? 1 : 2;
                hash ^= zobristTable[i][j][pieceType];
            }
        }
    }
    return hash;
}

uint64_t UpdateHash(uint64_t currentHash, int i, int j, int piece) {
    if (piece == 0) return currentHash;
    int pieceType = (piece == 1) ? 1 : 2;
    return currentHash ^ zobristTable[i][j][pieceType];
}

void ClearTranspositionTable() { transpositionTable.clear(); }

int EvaluatePattern(int consecutive, int openEnds, bool isAI) {
    if (openEnds == 0 && consecutive < 5) return 0;
    switch (consecutive) {
    case 5: return WIN_SCORE;
    case 4:
        if (openEnds == 2) return isAI ? 5000000 : 8000000;
        if (openEnds == 1) return isAI ? 100000 : 500000;
        break;
    case 3:
        if (openEnds == 2) return isAI ? 50000 : 200000;
        if (openEnds == 1) return isAI ? 1000 : 5000;
        break;
    case 2:
        if (openEnds == 2) return isAI ? 500 : 1000;
        if (openEnds == 1) return isAI ? 50 : 100;
        break;
    case 1:
        if (openEnds == 2) return isAI ? 10 : 20;
        break;
    }
    return 0;
}

int EvaluateLine(const vector<int>& line, int player, bool isAI) {
    int score = 0, consecutive = 0, openEnds = 0;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == player) {
            consecutive++;
        }
        else if (line[i] == 0 && consecutive > 0) {
            openEnds++;
            score += EvaluatePattern(consecutive, openEnds, isAI);
            consecutive = 0; openEnds = 1;
        }
        else if (line[i] == 0) {
            openEnds = 1;
        }
        else if (consecutive > 0) {
            score += EvaluatePattern(consecutive, openEnds, isAI);
            consecutive = 0; openEnds = 0;
        }
        else {
            openEnds = 0;
        }
    }
    if (consecutive > 0) score += EvaluatePattern(consecutive, openEnds, isAI);
    return score;
}

int EvaluateBoard(int aiPiece, int playerPiece) {
    int scoreAI = 0, scorePlayer = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        vector<int> lineR, lineC;
        for (int j = 0; j < BOARD_SIZE; j++) {
            lineR.push_back(_A[i][j].c);
            lineC.push_back(_A[j][i].c);
        }
        scoreAI += EvaluateLine(lineR, aiPiece, true) + EvaluateLine(lineC, aiPiece, true);
        scorePlayer += EvaluateLine(lineR, playerPiece, false) + EvaluateLine(lineC, playerPiece, false);
    }

    for (int k = -(BOARD_SIZE - 1); k < BOARD_SIZE; k++) {
        vector<int> line1, line2;
        for (int i = 0; i < BOARD_SIZE; i++) {
            int j1 = i - k;
            if (j1 >= 0 && j1 < BOARD_SIZE) line1.push_back(_A[i][j1].c);
            int j2 = k - i;
            if (j2 >= 0 && j2 < BOARD_SIZE) line2.push_back(_A[i][j2].c);
        }
        if (line1.size() >= 5) {
            scoreAI += EvaluateLine(line1, aiPiece, true);
            scorePlayer += EvaluateLine(line1, playerPiece, false);
        }
        if (line2.size() >= 5) {
            scoreAI += EvaluateLine(line2, aiPiece, true);
            scorePlayer += EvaluateLine(line2, playerPiece, false);
        }
    }
    return scoreAI - scorePlayer;
}

bool HasNeighbor(int i, int j, int range) {
    int startI = max(0, i - range), endI = min(BOARD_SIZE - 1, i + range);
    int startJ = max(0, j - range), endJ = min(BOARD_SIZE - 1, j + range);
    for (int x = startI; x <= endI; x++)
        for (int y = startJ; y <= endJ; y++)
            if (_A[x][y].c != 0) return true;
    return false;
}

vector<pair<int, int>> GetValidMoves() {
    vector<pair<int, int>> moves;
    bool isEmpty = true;
    for (int i = 0; i < BOARD_SIZE && isEmpty; i++)
        for (int j = 0; j < BOARD_SIZE && isEmpty; j++)
            if (_A[i][j].c != 0) isEmpty = false;

    if (isEmpty) {
        moves.push_back({ BOARD_SIZE / 2, BOARD_SIZE / 2 });
        return moves;
    }
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c == 0 && HasNeighbor(i, j, 2))
                moves.push_back({ i, j });
    return moves;
}

bool CheckWin(int player) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j <= BOARD_SIZE - 5; j++) {
            bool winR = true, winC = true;
            for (int k = 0; k < 5; k++) {
                if (_A[i][j + k].c != player) winR = false;
                if (_A[j + k][i].c != player) winC = false;
            }
            if (winR || winC) return true;
        }
    }
    for (int i = 0; i <= BOARD_SIZE - 5; i++) {
        for (int j = 0; j <= BOARD_SIZE - 5; j++) {
            bool win1 = true, win2 = true;
            for (int k = 0; k < 5; k++) {
                if (_A[i + k][j + k].c != player) win1 = false;
                if (_A[i + k][j + 4 - k].c != player) win2 = false;
            }
            if (win1 || win2) return true;
        }
    }
    return false;
}

int CountThreat(int i, int j, int player) {
    int maxScore = 0;
    int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    for (int d = 0; d < 4; d++) {
        int di = directions[d][0], dj = directions[d][1];
        int openEnds = 0, spacesBefore = 0, spacesAfter = 0;
        int consecutiveForward = 0, consecutiveBackward = 0;

        for (int k = 1; k < 5; k++) {
            int ni = i + di * k, nj = j + dj * k;
            if (ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE) {
                if (_A[ni][nj].c == player) consecutiveForward++;
                else if (_A[ni][nj].c == 0) { spacesAfter++; break; }
                else break;
            }
            else break;
        }
        for (int k = 1; k < 5; k++) {
            int ni = i - di * k, nj = j - dj * k;
            if (ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE) {
                if (_A[ni][nj].c == player) consecutiveBackward++;
                else if (_A[ni][nj].c == 0) { spacesBefore++; break; }
                else break;
            }
            else break;
        }

        int count = 1 + consecutiveForward + consecutiveBackward;
        int ni_f = i + di * (consecutiveForward + 1), nj_f = j + dj * (consecutiveForward + 1);
        if (ni_f >= 0 && ni_f < BOARD_SIZE && nj_f >= 0 && nj_f < BOARD_SIZE && _A[ni_f][nj_f].c == 0) openEnds++;
        int ni_b = i - di * (consecutiveBackward + 1), nj_b = j - dj * (consecutiveBackward + 1);
        if (ni_b >= 0 && ni_b < BOARD_SIZE && nj_b >= 0 && nj_b < BOARD_SIZE && _A[ni_b][nj_b].c == 0) openEnds++;

        int totalSpace = 1 + consecutiveForward + consecutiveBackward + spacesBefore + spacesAfter;
        if (totalSpace < 5 || openEnds == 0) continue;

        int score = 0;
        if (count >= 4) score = 10000 * openEnds;
        else if (count == 3) score = 1000 * openEnds;
        else if (count == 2) score = 100 * openEnds;
        else score = 10 * openEnds;
        maxScore = max(maxScore, score);
    }
    return maxScore;
}

pair<int, int> FindImmediateThreat(int aiPiece, int playerPiece) {
    vector<pair<int, int>> moves = GetValidMoves();
    for (auto move : moves) { _A[move.first][move.second].c = aiPiece; if (CheckWin(aiPiece)) { _A[move.first][move.second].c = 0; return move; } _A[move.first][move.second].c = 0; }
    for (auto move : moves) { _A[move.first][move.second].c = playerPiece; if (CheckWin(playerPiece)) { _A[move.first][move.second].c = 0; return move; } _A[move.first][move.second].c = 0; }
    // Tìm đòn chặn và tấn công nhanh (Lấy từ code gốc của bạn, rút gọn lại)
    pair<int, int> bestDef = { -1, -1 }, bestAtt = { -1, -1 };
    int maxDef = 0, maxAtt = 0;
    for (auto m : moves) { int s = CountThreat(m.first, m.second, playerPiece); if (s >= 10000 && s > maxDef) { maxDef = s; bestDef = m; } } if (bestDef.first != -1) return bestDef;
    for (auto m : moves) { int s = CountThreat(m.first, m.second, aiPiece); if (s >= 10000 && s > maxAtt) { maxAtt = s; bestAtt = m; } } if (bestAtt.first != -1) return bestAtt;
    for (auto m : moves) { int s = CountThreat(m.first, m.second, playerPiece); if (s >= 2000 && s > maxDef) { maxDef = s; bestDef = m; } } if (bestDef.first != -1) return bestDef;
    for (auto m : moves) { int s = CountThreat(m.first, m.second, aiPiece); if (s >= 2000 && s > maxAtt) { maxAtt = s; bestAtt = m; } } if (bestAtt.first != -1) return bestAtt;
    return { -1, -1 };
}

int QuickEval(int i, int j, int aiPiece, int playerPiece) {
    int score = 0;
    score -= (abs(i - BOARD_SIZE / 2) + abs(j - BOARD_SIZE / 2)) * 10;
    score += CountThreat(i, j, aiPiece) * 1000 + CountThreat(i, j, playerPiece) * 800;
    return score;
}

int Minimax(int depth, int alpha, int beta, bool isMaximizing, int aiPiece, int playerPiece, uint64_t boardHash) {
    auto it = transpositionTable.find(boardHash);
    if (it != transpositionTable.end() && it->second.depth >= depth) {
        if (it->second.flag == 0) return it->second.score;
        else if (it->second.flag == 1) alpha = max(alpha, it->second.score);
        else if (it->second.flag == 2) beta = min(beta, it->second.score);
        if (alpha >= beta) return it->second.score;
    }

    if (CheckWin(aiPiece)) return WIN_SCORE - depth;
    if (CheckWin(playerPiece)) return -WIN_SCORE + depth;
    if (depth == 0) return EvaluateBoard(aiPiece, playerPiece);

    vector<pair<int, int>> moves = GetValidMoves();
    if (moves.empty()) return 0;

    vector<pair<int, pair<int, int>>> scoredMoves;
    for (auto move : moves) scoredMoves.push_back({ QuickEval(move.first, move.second, aiPiece, playerPiece), move });
    sort(scoredMoves.begin(), scoredMoves.end(), greater<pair<int, pair<int, int>>>());

    int originalAlpha = alpha, bestScore = isMaximizing ? numeric_limits<int>::min() : numeric_limits<int>::max();
    for (auto& sm : scoredMoves) {
        auto move = sm.second;
        _A[move.first][move.second].c = isMaximizing ? aiPiece : playerPiece;
        uint64_t newHash = UpdateHash(boardHash, move.first, move.second, _A[move.first][move.second].c);

        int eval = Minimax(depth - 1, alpha, beta, !isMaximizing, aiPiece, playerPiece, newHash);
        _A[move.first][move.second].c = 0; // Trả lại bảng cờ (Backtracking)

        if (isMaximizing) { bestScore = max(bestScore, eval); alpha = max(alpha, eval); }
        else { bestScore = min(bestScore, eval); beta = min(beta, eval); }
        if (beta <= alpha) break;
    }

    TranspositionEntry entry = { boardHash, depth, bestScore, (bestScore <= originalAlpha) ? 2 : ((bestScore >= beta) ? 1 : 0) };
    if (transpositionTable.size() < TRANSPOSITION_TABLE_SIZE) transpositionTable[boardHash] = entry;
    return bestScore;
}

_POINT FindBotMove(int aiPiece, int difficultyLevel) {
    int playerPiece = (aiPiece == 1) ? -1 : 1;
    InitZobrist();

    pair<int, int> threatMove = FindImmediateThreat(aiPiece, playerPiece);
    if (threatMove.first != -1) return { _A[threatMove.first][threatMove.second].x, _A[threatMove.first][threatMove.second].y, 0 };

    int depth = (difficultyLevel == 1) ? 1 : 3;
    vector<pair<int, int>> moves = GetValidMoves();
    if (moves.empty()) return { -1, -1, 0 };

    vector<pair<int, pair<int, int>>> scoredMoves;
    for (auto move : moves) scoredMoves.push_back({ QuickEval(move.first, move.second, aiPiece, playerPiece), move });
    sort(scoredMoves.begin(), scoredMoves.end(), greater<pair<int, pair<int, int>>>());

    pair<int, int> bestMove = scoredMoves[0].second;
    int bestScore = numeric_limits<int>::min(), alpha = numeric_limits<int>::min(), beta = numeric_limits<int>::max();
    uint64_t currentHash = ComputeBoardHash();
    ClearTranspositionTable();

    int maxMovesToConsider = (difficultyLevel == 3) ? min(20, (int)scoredMoves.size()) : min(15, (int)scoredMoves.size());
    for (int i = 0; i < maxMovesToConsider; i++) {
        auto move = scoredMoves[i].second;
        _A[move.first][move.second].c = aiPiece;
        uint64_t newHash = UpdateHash(currentHash, move.first, move.second, aiPiece);

        int score = Minimax(depth - 1, alpha, beta, false, aiPiece, playerPiece, newHash);
        _A[move.first][move.second].c = 0;

        if (score > bestScore) { bestScore = score; bestMove = move; }
        alpha = max(alpha, bestScore);
    }

    _POINT result;
    result.x = _A[bestMove.first][bestMove.second].x;
    result.y = _A[bestMove.first][bestMove.second].y;
    return result;
}