#pragma once
#include "Chess.h"
#include "AITypes.h"
#include "ChessShape.h"
#include "HashTable.h"
#include <iostream>
#include <string>
class Board
{
public:
	Chess board[1024];						//棋盘数组,记录每个位置的相关信息
	Point remPoint[1024];					//记录每步棋的位置
	Chess* remChess[1024];					//记录每步棋的chess指针
	Point remULCand[1024];					//记录左上角
	Point remLRCand[1024];					//记录右下角
	int chessCount;							//棋子数
	Piece who, opp;							//当前下子方，以及另一方
	int nShape[2][10];						//双方下一步能成的棋形统计
	int boardSize;							//棋盘尺寸
	Point upperLeft;						//左上角
	Point lowerRight;						//右下角
	int ply;								//当前搜索层数
	int maxPly;								//最大搜索层数
	int limitPly;							//vcf限制层数
	
	Board(HashTable* _hashTable);
	void initBoard(int size);
	void move(Point p);
	void undo();
	bool check();
	void generateCand(Cand cand[], int &nCand);
	void getEmptyCand(Cand cand[], int &nCand);
	int evaluate();
	int quickWinSearch();
	int vcfSearch();

	//内联方法
	bool isExpand() { return nShape[opp][A] > 0 || nShape[opp][B] > 0 || nShape[opp][C] > 0; }
	int pointPiece(int x, int y) { return board[makePoint(x, y)].piece; }
	int pointX(int index) { return index >> 5; }
	int pointY(int index) { return index & 31; }
	int makePoint(int x, int y) { return (x << 5) + y; }
	int max(int a, int b) { return a > b ? a : b; }
	int min(int a, int b) { return a < b ? a : b; }
	bool inBoard(int index) { return board[index].piece != OUTSIDE; }
	Piece oppent(Piece piece) { return piece == BLACK ? WHITE : BLACK; }
private:
	HashTable* hashTable;
};
