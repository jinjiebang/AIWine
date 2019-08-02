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
	Chess board[1024];						//��������,��¼ÿ��λ�õ������Ϣ
	Point remPoint[1024];					//��¼ÿ�����λ��
	Chess* remChess[1024];					//��¼ÿ�����chessָ��
	Point remULCand[1024];					//��¼���Ͻ�
	Point remLRCand[1024];					//��¼���½�
	int chessCount;							//������
	Piece who, opp;							//��ǰ���ӷ����Լ���һ��
	int nShape[2][16];						//˫����һ���ܳɵ�����ͳ��
	int boardSize;							//���̳ߴ�
	Point upperLeft;						//���Ͻ�
	Point lowerRight;						//���½�
	int ply;								//��ǰ��������
	int maxPly;								//ʵ��������������
	long t_VCT_Start;						//VCT��ʼ����ʱ��
	long t_VCF_Start;						//VCF��ʼ����ʱ��
	int vctNode;							//VCT�ڵ���
	int vcfNode;							//VCF�ڵ���
	const int MAX_VCF_DEPTH = 20;			//���vcf���
	const int MAX_VCT_DEPTH = 16;			//���vct���
	const int MAX_DEFNED_FOUR = 6;			//vct��ɱʱ����ɱ���л���ʱ�����ط�����ܳ弸����
	const int MAX_VCT_TIME = 1000;			//VCTʱ��(���룩
	int Range4[32];							//4���ڵ����ַ�Χ
	int Range3[32];							//3���ڵ����ַ�Χ�Ͱ��Ե�
	int fivePoint[2];						//��¼�����
	
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

	//��������
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
