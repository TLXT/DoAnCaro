#pragma once
#include "GameStatus.h"
#include <vector>
using namespace std;

void initZobrist();
uint64_t computeBoardHash();
uint64_t updateHash(uint64_t currentHash, int i, int j, int piece);
void clearTranspositionTable();
int evaluatePattern(int consecutive, int openEnds, bool isAI);
int evaluateLine(const vector<int>& line, int player, bool isAI);
int evaluateBoard(int aiPiece, int playerPiece);
bool hasNeighbor(int i, int j, int range);
vector<pair<int, int>> getValidMoves();
bool checkWin(int player);
int countThreat(int i, int j, int player);
pair<int, int> findImmediateThreat(int aiPiece, int playerPiece);
int quickEval(int i, int j, int aiPiece, int playerPiece);
int minimax(int depth, int alpha, int beta, bool isMaximizing, int aiPiece, int playerPiece, uint64_t boardHash);
_POINT FindBotMove(int aiPiece, int difficultyLevel);
