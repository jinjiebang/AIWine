#include "AIWine.h"
#include "ChessShape.h"
#include <assert.h>

AIWine::AIWine()
{
	board = new Board();
	ChessShape::initShape();
}
AIWine::~AIWine()
{
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
		<< " [" << pointX(best.point) - 4 << "," << pointY(best.point) - 4 << "]"
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
	nSearched = 0;
	board->generateCand(rootCand, nRootCand);
	for (int depth = MinDepth; depth <=MaxDepth; depth++)
	{
		board->ply = 0, board->maxPly = 0, board->limitPly = depth + 8;
		t0 = getTime();

		best = rootSearch(depth, -10000, 10000);
		if (best.point != 0) rootBest = best;

		t1 = getTime(); td = t1 - t0;
		showDepthInfo(depth, rootBest, td);
		if (rootBest.value == 10000 || nRootCand == 1 || terminateAI || t1 + 5 * td - stopTime() >= 0) break;
	}
	x = pointX(rootBest.point) - 4;
	y = pointY(rootBest.point) - 4;
	assert(isValidPos(x, y));
	turnMove(x, y);
}
//根节点搜索
Cand AIWine::rootSearch(int depth, int alpha, int beta)
{
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
	int q = board->quickWinSearch();
	if (q != 0)
	{
		return q > 0 ? 10000 : -10000;
	}
	if (depth <= 0)
	{
		int eval = board->evaluate();
		if (eval>alpha && eval<beta)
		{
			if (board->isExpand())
			{
				depth++;
			}
			else
			{
				/*q = board->vcfSearch();
				if (q > 0) return 10000;
				else return eval;*/
				return eval;
			}
		}
		else
		{
			return eval;

		}
	}
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
			return beta;
		}
		if (value > alpha)
		{
			alpha = value;
		}
		if (terminateAI) break;
	}
	return alpha;
}
//删除必败点
void AIWine::delLoseCand(Cand cand[], int &nCand)
{
	for (int i = 0; i < nCand; i++)
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