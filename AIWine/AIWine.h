#pragma once
#include "AITypes.h"
#include "Board.h"
#include "ChessShape.h"
#include <iostream>
#include <string>


class AIWine
{
public:
	AIWine();
	~AIWine();
	static const int MaxSize = 28;
	static const int MaxDepth = 8;
	static const int MinDepth = 1;
	static const int MaxCand = 256;
	void restart();
	void turnUndo();
	bool setSize(int size);
	bool isValidPos(int x, int y);
	void turnMove(int x, int y);
	void turnBest(int &x, int &y);
	Cand rootSearch(int depth, int alpha, int beta);
	int search(int depth, int alpha, int beta);

	int nSearched;							//每次思考时,搜索的局面数
	int speed;								//搜索速度
	Cand rootBest;							//根节点返回的最佳点
	Cand rootCand[MaxCand];					//根节点分支
	int nRootCand;							//根节点分支数
	int timeout_turn;
	int timeout_match;
	int time_left;

	void delLoseCand(Cand cand[],int &nCand);
	void sortCand(Cand cand[], int nCand);
	int getMaxPly() { return board->maxPly; }
	int pointX(int index) { return index >> 5; }
	int pointY(int index) { return index & 31; }
	int makePoint(int x, int y) { return (x << 5) + y; }
	string getShapeName(int index) { return ChessShape::getShapeName(index); }
	string getShape4Name(int index) { return ChessShape::getShape4Name(index); }
	string getPiece(int piece) { return piece == BLACK ? "黑" : "白"; }
	
private:
	Board *board;
	
};
