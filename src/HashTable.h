#pragma once
#include "AITypes.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
class HashTable
{

public:
	HashTable()
	{
		
		initHashVal();
	}
	//填充zobrist随机值
	void initHashVal()
	{
		memset(table, 0, sizeof(table));
		hashKey = 0;
		srand(time(NULL));
		for (int i = 0; i < 2048; i++)
		{
			hashVal[i] = Rand64();
		}
	}
	//查询置换表
	int queryRecord(int depth,int alpha,int beta)
	{
		//如果是pv节点,不做置换表截断
	/*	if (beta > alpha + 1) return InvalidVal;*/
		if (currentItem->key == hashKey)
		{
			if (currentItem->flag == HASH_EXACT && (currentItem->value == WinScore || currentItem->value == LoseScore))
			{
				return currentItem->value;
			}
			if (currentItem->depth >= depth)
			{
				if (currentItem->flag == HASH_EXACT)
				{
					return currentItem->value;
				}
				else if (currentItem->flag == HASH_ALPHA && currentItem->value <= alpha)
				{
					return alpha;
				}
				else if (currentItem->flag== HASH_BETA && currentItem->value >= beta)
				{
					return beta;
				}
			}
		}
		return InvalidVal;
	}
	//记录局面信息到置换表
	void   update(int _value, int _depth,int _flag,Point _best)
	{
		HashItem* c = currentItem;
		c->value = _value;
		c->depth = _depth;
		c->best = _best;
		c->flag = _flag;
		c->key = hashKey;
	}
	//更新当前局面的hashKey以及currentItem
	void move(Point p, Piece who)
	{
		hashKey ^= hashVal[p + (who << 10)];
		currentItem = &table[hashKey%HashSize];
	}
	bool present() { return currentItem->key == hashKey; }
	short   value() { return currentItem->value; } 
	short   depth() { return currentItem->depth; }
	Point best() { return currentItem->best; }
	U64 Rand64(){ return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);}
	static const int HashSize = 1 << 23;
	static const int InvalidVal = -20000;
private:
	struct HashItem
	{
		U64   key;
		int depth;
		int value;
		int flag;
		Point best;
	};
	
	HashItem* currentItem;
	HashItem table[HashSize];
	U64   hashKey;
	U64 hashVal[2048];
};