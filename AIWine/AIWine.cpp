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
	/*int pointPiece = board->who;
	assert(pointPiece == BLACK || pointPiece == WHITE);
	cout << "MESSAGE 落子为：" + getPiece(pointPiece) << endl;*/
	int point = makePoint(x + 4, y + 4);
	board->move(point);
	/*cout << "MESSAGE 落子结束！"<< endl;*/
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
	board->generateCand(rootCand, nRootCand);
	/*if (nRootCand == 1)
	{
		cout <<"MESSAGE oneCand:"<< pointX(rootCand[0].point) - 4 << "," << pointY(rootCand[0].point) - 4 << endl;
	}*/
	board->ply = 0; board->maxPly = 0;
	for (int depth = MinDepth; depth <=MaxDepth; depth++)
	{
		board->limitPly = depth + 4;
		best = rootSearch(depth, -10000, 10000);
		if (best.value == 10000 || nRootCand == 1) break;
	}
	rootBest = best;
	x = pointX(best.point) - 4;
	y = pointY(best.point) - 4;
	assert(isValidPos(x, y));
	turnMove(x, y);
}
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
	/*int opp = board->opp;
	int who = board->who;
	int nShape[2][10] = { 0 };
	for (int k = 0; k < 10; k++)
	{
		nShape[opp][k] = board->nShape[opp][k];
		nShape[who][k] = board->nShape[who][k];
	}
	cout << "MESSAGE nShape:" << " who=" << getPiece(who) << " (" << nShape[who][A] << "," << nShape[who][B] << "," << nShape[who][C] << ")" << endl;
	cout << "MESSAGE nShape:" << " opp=" << getPiece(opp) << " (" << nShape[opp][A] << "," << nShape[opp][B] << "," << nShape[opp][C] << ")" << endl;*/
	Cand best = Cand(0, alpha - 1);
	int value;
	for (int i = 0; i < nRootCand; i++)
	{
		/*int point = rootCand[i].point;
		int x = board->pointX(point) - 4;
		int y = board->pointY(point) - 4;
		int shape[4] = { 0 };
		for (int k = 0; k < 4; k++)
		{
			shape[k] = board->getShape(point, who, k);
		}
		cout << "MESSAGE" << " point=[" << x << "," << y << "] piece=" << getPiece(who) << " prior=" << rootCand[i].value << endl;
		cout << "MESSAGE" << " shape:[" << getShapeName(shape[0]) << "," << getShapeName(shape[1]) << "," << getShapeName(shape[2]) << "," << getShapeName(shape[3]) << "] shape4=[" << getShape4Name(board->getShape4(point, who)) << "," << getShape4Name(board->getShape4(point, opp)) << "]" << endl;*/

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
		/*cout << "MESSAGE value=" << value << endl;*/
		
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
		int eval = board->evaluate();
		if (eval > -beta&&eval < -alpha)
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










