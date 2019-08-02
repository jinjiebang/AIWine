#pragma once
#include "AITypes.h"
#include "Board.h"
#include "ChessShape.h"
#include <time.h>
#include <iostream>
#include <string>


class AIWine
{
public:
	AIWine();
	~AIWine();
	static const int MaxSize = 28;
	static const int MaxDepth = 50;
	static const int MinDepth = 1;
	static const int MaxCand = 256;

	void restart();
	void turnUndo();
	bool setSize(int size);
	bool isValidPos(int x, int y);
	void turnMove(int x, int y);
	void turnBest(int &x, int &y);
	Cand rootSearch(int depth, int alpha, int beta);
	int search(int depth, int alpha, int beta,int extend);

	int nSearched;							//ÿ��˼��ʱ,�����ľ�����
	int speed;								//�����ٶ�
	Cand rootBest;							//���ڵ㷵�ص���ѵ�
	Cand rootCand[MaxCand];					//���ڵ��֧
	int nRootCand;							//���ڵ��֧��
	int timeout_turn;						//��ʱ
	int timeout_match;						//��ʱ
	int time_left;							//ʣ��ʱ��
	long start_time;						//��ʼʱ��
	bool terminateAI;						//ֹͣ������־
	bool isCheckVCT[1024];					//��¼�Ƿ����VCT
	const int delVctLoseDepth = 10;			//ɾ���ذܵ��VCT���(ÿ���������Ӷ����)
	const int checkOppVctDepth = 14;		//���Է�VCT�����(ֻ������ߵĵ�,����VCT���������)

	void delLoseCand();
	int delVctLose();
	int checkOppVct();
	void showDepthInfo(int depth, Cand best, long td);
	long stopTime() { return start_time + __min(timeout_turn, time_left / 7) - 30; }
	long getTime() { return clock() * 1000 / CLOCKS_PER_SEC; }
	int pointX(int index) { return index >> 5; }
	int pointY(int index) { return index & 31; }
	int makePoint(int x, int y) { return (x << 5) + y; }
	
private:
	Board* board;
	HashTable* hashTable;
};
