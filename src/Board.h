#pragma once
#include "Chess.h"
#include "AITypes.h"
#include "ChessShape.h"
#include "HashTable.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
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
	int nShape[2][16];						//双方下一步能成的棋形统计
	int boardSize;							//棋盘尺寸
	Point upperLeft;						//左上角
	Point lowerRight;						//右下角
	int ply;								//当前搜索层数
	int maxPly;								//实际搜索的最大层数
	long t_VCT_Start;						//VCT开始搜索时间
	long t_VCF_Start;						//VCF开始搜索时间
	int vctNode;							//VCT节点数
	int vcfNode;							//VCF节点数
	const int MAX_VCF_DEPTH = 20;			//最大vcf深度
	const int MAX_VCT_DEPTH = 16;			//最大vct深度
	const int MAX_DEFNED_FOUR = 6;			//vct算杀时，算杀方有活三时，防守方最多能冲几个四
	const int MAX_VCT_TIME = 1000;			//VCT时间(毫秒）
	int Range4[32];							//4格内的米字范围
	int Range3[32];							//3格内的米字范围和八卦点
	int fivePoint[2];						//记录成五点
	
	Board(HashTable* _hashTable);
	void initBoard(int size);
	void move(Point p);
	void undo();
	bool check();
	void generateCand(Cand cand[], int &nCand);
	void sortCand(Cand cand[], int nCand);
	void getEmptyCand(Cand cand[], int &nCand);
	int evaluate();
	int quickWinSearch();
	int vcfSearch(int *winPoint);
	int vcfSearch(int searcher, int depth,int lastPoint,int *winPoint);
	int vcfSearch(int searcher,int depth,int lastPoint);
	int vctSearch(int searcher, int depth, int maxDepth, int lastPoint);
	int vctSearch(int searcher, int depth, int maxDepth, int lastPoint, int* winPoint);
	int vctSearch(int *winPoint);
	
	Point findPoint(Piece piece, FourShape shape);
	Point findVCFLastPoint();	
	Point findVCTLastPoint();

	//内联方法
	void vctStart() { t_VCT_Start = getTime(); vctNode = 0; }
	void vcfStart() { t_VCF_Start = getTime(); vcfNode = 0; }
	long getVCTCost() { return getTime() - t_VCT_Start; }
	long getVCFCost() { return getTime() - t_VCF_Start; }
	bool isExpand() { return nShape[opp][A] > 0; }
	bool isExtend() { return  nShape[opp][A] > 0 || (nShape[opp][B] > 0 && nShape[who][C] + nShape[who][D] + nShape[who][E] + nShape[who][F] == 0); }
	int pointPiece(int x, int y) { return board[makePoint(x, y)].piece; }
	int pointX(int index) { return index >> 5; }
	int pointY(int index) { return index & 31; }
	int makePoint(int x, int y) { return (x << 5) + y; }
	int max(int a, int b) { return a > b ? a : b; }
	int min(int a, int b) { return a < b ? a : b; }
	bool inBoard(int index) { return board[index].piece != OUTSIDE; }
	Piece oppent(Piece piece) { return piece == BLACK ? WHITE : BLACK; }
	long getTime() { return clock() * 1000 / CLOCKS_PER_SEC; }
private:
	HashTable* hashTable;
};
