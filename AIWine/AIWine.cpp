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
bool AIWine::setSize(int size)
{
	if (size<5 || size>MaxSize)
	{
		return false;
	}
	board->initBoard(size);
	return true;
}
void AIWine::restart()
{
	board->initBoard(board->boardSize);
}
void AIWine::turnUndo()
{
	board->undo();
}
void AIWine::turnMove(int x, int y)
{
	int point = makePoint(x + 4, y + 4);
	board->move(point);
}
void AIWine::turnBest(int &x, int &y)
{
	if (board->chessCount == 0)
	{
		x = board->boardSize / 2;
		y = board->boardSize / 2;
		turnMove(x, y);
		return;
	}
	nSearched = 0;
	Cand best = { 0,0 };
	for (int depth = MinDepth; depth <=MaxDepth; depth++)
	{
		best = rootSearch(depth, -10000, 10000);
		if (best.value == 10000 || nRootCand == 1) break;
	}
	assert(board->inBoard(best.point));
	x = pointX(best.point) - 4;
	y = pointY(best.point) - 4;
	turnMove(x, y);
}
Cand AIWine::rootSearch(int depth, int alpha, int beta)
{
	if(depth>MinDepth)
	{
		delLoseCand(rootCand, nRootCand);
	}
	else
	{
		board->generateCand(rootCand, nRootCand);
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
		value = -search(depth - 1, -beta, -alpha);
		board->undo();
		rootCand[i].value = value;
		if (value > best.value)
		{
			best = rootCand[i];
			alpha = value;
			if (value == 10000) return best;
		}
	}
	return best;

}
int AIWine::search(int depth, int alpha, int beta)
{
	nSearched++;
	int q = board->quickWinSearch();
	if (q != 0)
	{
		return q > 0 ? 10000 : -10000;
	}
	if (depth <= 0)
	{
		return board->evaluate();
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
		value = -search(depth - 1, -beta, -alpha);
		board->undo();
		if (value >= beta)
		{
			return beta;
		}
		if (value > alpha)
		{
			alpha = value;
		}
	}
	return alpha;
}
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
bool AIWine::isValidPos(int x, int y)
{
	int size = board->boardSize;
	return x >= 0 && x < size && y >= 0 && y < size && board->pointPiece(x + 4, y + 4) == EMPTY;
}










