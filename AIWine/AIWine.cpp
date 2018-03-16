#include "AIWine.h"
#include "ChessShape.h"
#include <assert.h>

AIWine::AIWine()
{
	hashTable = new HashTable();
	board = new Board(hashTable);
	ChessShape::initShape();
}
AIWine::~AIWine()
{
	delete hashTable;
	delete board;
}
//设置棋盘尺寸
bool AIWine::setSize(int size)
{
	if (size<5 || size>MaxSize)
	{
		return false;
	}
	board->initBoard(size);
	return true;
}
//重新开始
void AIWine::restart()
{
	board->initBoard(board->boardSize);
}
//悔棋
void AIWine::turnUndo()
{
	board->undo();
}
//显示搜索的信息
void AIWine::showDepthInfo(int depth,Cand best, long td)
{
	cout << "MESSAGE depth: " << depth << "-" << board->maxPly
		<< " best:[" << pointX(best.point) - 4 << "," << pointY(best.point) - 4 << "]"
		<< " val=" << best.value << " time:" << td << "ms" << endl;
}
//落子
void AIWine::turnMove(int x, int y)
{
	int point = makePoint(x + 4, y + 4);
	board->move(point);
}
//获取最佳点
void AIWine::turnBest(int &x, int &y)
{
	Cand best;
	long t0, t1, td;
	start_time = getTime();
	terminateAI = false;
	if (board->chessCount == 0)
	{
		x = board->boardSize / 2;
		y = board->boardSize / 2;
		turnMove(x, y);
		return;
	}
	if (timeout_turn == 0) timeout_turn = 10000;
	if (timeout_match == 0) timeout_match = time_left = 1000000;
	int temp_best = rootBest.point;
	nSearched = 0;

	bool isSolved = false;
	//先算vcf
	if (board->vcfSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCF算杀成功！必胜点["<< pointX(rootBest.point) - 4 <<", "<< pointY(rootBest.point) - 4 <<"]"<< endl;
	}
	//再算vct
	if (!isSolved&&board->vctSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCT算杀成功！必胜点[" << pointX(rootBest.point) - 4 << ", " << pointY(rootBest.point) - 4 << "]" << endl;
	}
	//如果没有算到杀
	if (!isSolved)
	{
		board->generateCand(rootCand, nRootCand);
		for (int depth = MinDepth; depth <= MaxDepth; depth++)
		{
			t0 = getTime();

			best = rootSearch(depth, -10000, 10000);
			if (best.point != 0) rootBest = best;

			t1 = getTime(); td = t1 - t0;
			showDepthInfo(depth, rootBest, td);
			if (rootBest.value == 10000 || rootBest.value == -10000 || nRootCand == 1 || terminateAI || t1 + 5 * td - stopTime() >= 0) break;
		}
		assert(temp_best != rootBest.point);
	}
	
	x = pointX(rootBest.point) - 4;
	y = pointY(rootBest.point) - 4;
	assert(isValidPos(x, y));
	turnMove(x, y);
}
//根节点搜索
Cand AIWine::rootSearch(int depth, int alpha, int beta)
{
	board->ply = 0, board->maxPly = 0;
	if(depth>MinDepth)
	{
		delLoseCand(rootCand, nRootCand);
		if (nRootCand == 0)
		{
			nRootCand = 1;
			rootCand[0].value = -10000;
			return rootCand[0];
		}
	}

	if (nRootCand == 0)
	{
		board->getEmptyCand(rootCand, nRootCand);
	}
	else if (nRootCand == 1)
	{
		rootCand[0].value = 0;
		return rootCand[0];
	}
	else
	{
		sortCand(rootCand, nRootCand);
	}
	Cand best = Cand(0, alpha - 1);
	int value;
	for (int i = 0; i < nRootCand; i++)
	{
		board->move(rootCand[i].point);
		do
		{
			if (i > 0 && alpha + 1 < beta)
			{
				value = -search(depth - 1, -alpha - 1, -alpha);
				if (value <= alpha || value >= beta)
				{
					break;
				}
			}
			value = -search(depth - 1, -beta, -alpha);
		} while (0);
		board->undo();
		if (terminateAI) break;

		rootCand[i].value = value;
		if (value > best.value )
		{
			best = rootCand[i];
			alpha = value;
			if (value == 10000) return best;
		}
		
	}
	return best;

}
//搜索主函数
int AIWine::search(int depth, int alpha, int beta)
{
	static int cnt;
	if (--cnt<0)
	{
		cnt = 1000;
		if (getTime() - stopTime() > 0) terminateAI = true;
	}
	nSearched++;
	//简单胜
	int q = board->quickWinSearch();
	if (q != 0) return q > 0 ? 10000 : -10000;
	//到达叶节点
	if (depth <= 0)
	{
		int eval = board->evaluate();
		if (eval>alpha&&eval<beta){
			/*if (board->isExpand())
			{
				depth++;
			}else {*/
			int lastPoint = board->findLastPoint();
			if (lastPoint == -1)
			{
				return eval;
			}
			else
			{
				return board->vcfSearch(board->who, 0, lastPoint) > 0 ? 10000 : eval;
			}
			//}
		}else{
			return eval;
		}
	}
	if ((q = hashTable->queryRecord(depth, alpha, beta)) != HashTable::InvalidVal) return q;

	int hash_flag = HASH_ALPHA;
	Point best = 0;
	Cand cand[MaxCand];
	int nCand = 0;
	board->generateCand(cand, nCand);

	if (nCand == 0)
	{
		board->getEmptyCand(cand, nCand);
	}
	else if (nCand > 1)
	{
		sortCand(cand, nCand);
	}
	int value;
	for (int i = 0; i < nCand; i++)
	{
		board->move(cand[i].point);
		do
		{
			if (i > 0 && alpha + 1 < beta)
			{
				value = -search(depth - 1, -alpha - 1, -alpha);
				if (value <= alpha || value >= beta)
				{
					break;
				}
			}
			value = -search(depth - 1, -beta, -alpha);
		} while (0);
		board->undo();

		if (value >= beta)
		{
			if(!terminateAI) hashTable->update(beta, depth, HASH_BETA, cand[i].point);
			return beta;
		}
		if (value > alpha)
		{
			hash_flag = HASH_EXACT;
			best = cand[i].point;
			alpha = value;
		}
		if (terminateAI) break;
	}
	if (!terminateAI)
	{
		hashTable->update(alpha, depth, hash_flag, best);
	}
	return alpha;
}
//删除必败点
void AIWine::delLoseCand(Cand cand[], int &nCand)
{
	for (int i = nCand - 1; i >= 0; i--)
	{
		if (cand[i].value == -10000)
		{
			for (int j = i + 1; j < nCand; j++)
			{
				cand[j - 1] = cand[j];
			}
			nCand--;
		}
	}
}
//排序选点
void AIWine::sortCand(Cand cand[], int nCand)
{
	Cand key;
	int i, j;
	for (i = 1; i < nCand; i++)
	{
		key = cand[i];
		for (j = i; j > 0 && cand[j - 1].value < key.value; j--)
		{
			cand[j] = cand[j - 1];
		}
		cand[j] = key;
	}
}
//判断坐标是否有效
bool AIWine::isValidPos(int x, int y)
{
	int size = board->boardSize;
	return x >= 0 && x < size && y >= 0 && y < size && board->pointPiece(x + 4, y + 4) == EMPTY;
}