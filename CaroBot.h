#pragma once
#include "GameStatus.h"
#include <vector>
using namespace std;

void InitZobrist();
uint64_t ComputeBoardHash();
uint64_t UpdateHash(uint64_t currentHash, int i, int j, int piece);
void ClearTranspositionTable();
int EvaluatePattern(int consecutive, int openEnds, bool isAI);
int EvaluateLine(const vector<int>& line, int player, bool isAI);
int EvaluateBoard(int aiPiece, int playerPiece);
bool HasNeighbor(int i, int j, int range);
vector<pair<int, int>> GetValidMoves();
bool CheckWin(int player);
int CountThreat(int i, int j, int player);
pair<int, int> FindImmediateThreat(int aiPiece, int playerPiece);
int QuickEval(int i, int j, int aiPiece, int playerPiece);
int Minimax(int depth, int alpha, int beta, bool isMaximizing, int aiPiece, int playerPiece, uint64_t boardHash);
_POINT FindBotMove(int aiPiece, int difficultyLevel);
