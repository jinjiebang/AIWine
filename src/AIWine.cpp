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
//删除会被VCT胜的点,
int AIWine::delVctLose()
{
	int lastPoint, result;
	int maxLoseSteps = 0;
	int bestPoint = rootCand[0].point;
	bool isAllLose = true;

	board->vctStart();
	for (int i = 0; i < nRootCand; i++)
	{
		Cand& c = rootCand[i];
		board->move(c.point);
		lastPoint = board->findVCTStartPoint();
		
		if (lastPoint != -1 && (result = board->vctSearch(board->who, 0, delVctLoseDepth, lastPoint)) > 0)//对方VCT胜利
		{
			c.value = LoseScore;
			// 如果都是必败，选择拖延步数最长的为最佳点
			if (isAllLose && result > maxLoseSteps)
			{
				maxLoseSteps = result;
				bestPoint = c.point;
			}
		}
		else
		{
			if (isAllLose)
			{
				isAllLose = false;
				bestPoint = c.point;
			}
		}
		board->undo();
	}
	delLoseCand();
	return bestPoint;
}
//检查对方VCT,从分数最高的点开始算VCT,直到找到一个不败的点
int AIWine::checkOppVct()
{
	int lastPoint, result, index = 0, maxLoseSteps = 0;
	bool findVct, isAllLose = true;
	int bestPoint = rootCand[0].point;

	board->vctStart();
	do
	{
		findVct = false;
		Cand& c = rootCand[index++];
		if (!isCheckVCT[c.point])
		{
			isCheckVCT[c.point] = true;
			board->move(c.point);
			//对手VCT胜利
			if ((lastPoint = board->findVCTStartPoint()) != -1 && (result = board->vctSearch(board->who, 0, checkOppVctDepth, lastPoint)) > 0)
			{
				c.value = LoseScore;
				findVct = true;
				// 如果都是必败，选择拖延步数最长的为最佳点
				if (isAllLose && result > maxLoseSteps)
				{
					maxLoseSteps = result;
					bestPoint = c.point;
				}
			}
			else
			{
				if (isAllLose)
				{
					isAllLose = false;
					bestPoint = c.point;
				}
			}
			board->undo();
		}
		
	} while (findVct && index < nRootCand);
	delLoseCand();
	return bestPoint;
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
	if (timeout_match == 0) timeout_match = time_left = 10000000;
	nSearched = 0;

	bool isSolved = false;
	//先算vcf
	if (board->vcfSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCF success! win point["<< pointX(rootBest.point) - 4 <<", "<< pointY(rootBest.point) - 4 <<"]"<< endl;
	}
	//再算vct
	if (!isSolved&&board->vctSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCT success! win point[" << pointX(rootBest.point) - 4 << ", " << pointY(rootBest.point) - 4 << "]" << endl;
	}
	//算对方的VCT,删除对方能VCT胜的点
	if (!isSolved)
	{
		board->generateCand(rootCand, nRootCand);
		board->sortCand(rootCand, nRootCand);
		int bestPoint = delVctLose();
		if (nRootCand == 0)
		{
			isSolved = true;
			rootBest.point = bestPoint;
			cout << "MESSAGE opp VCT win!"<< endl;
		}
		else if (nRootCand == 1)
		{
			//这里没有把isSolved设为true,是想通过搜索来给出一个评价分
			cout << "MESSAGE find one cand by VCT" << endl;
		}
	}
	if (!isSolved)
	{
		board->ply = 0, board->maxPly = 0;
		int lastBest = rootBest.point;
		memset(isCheckVCT, 0, sizeof(isCheckVCT));
		for (int depth = MinDepth; depth <= MaxDepth; depth++)
		{
			t0 = getTime();

			best = rootSearch(depth, LoseScore, WinScore);
			if (best.point != 0) rootBest = best;

			t1 = getTime(); td = t1 - t0;
			if (nRootCand > 1) delLoseCand();
			showDepthInfo(depth, rootBest, td);
			if (rootBest.value == WinScore || rootBest.value == LoseScore || nRootCand == 1 || terminateAI || t1 + 5 * td - stopTime() >= 0) break;
			
			board->sortCand(rootCand, nRootCand);
			if (depth > MinDepth && isCheckVCT[rootBest.point]) continue;
			best.point = checkOppVct();
			cout << "MESSAGE VCT cost time:" << board->getVCTCost() << "ms" << endl;
			if (nRootCand == 0)
			{
				rootBest.point = best.point;
				cout << "MESSAGE opp vct win!" << endl;
				break;
			}
			else if (nRootCand == 1)
			{
				cout << "MESSAGE find one cand by VCT!" << endl;
				rootBest = rootCand[0];
				break;
			}
		}
		assert(lastBest != rootBest.point);
	}
	
	x = pointX(rootBest.point) - 4;
	y = pointY(rootBest.point) - 4;
	assert(isValidPos(x, y));
	turnMove(x, y);
}
//根节点搜索
Cand AIWine::rootSearch(int depth, int alpha, int beta)
{
	Cand best = Cand(0, alpha - 1);
	int value;
	for (int i = 0; i < nRootCand; i++)
	{
		board->move(rootCand[i].point);
		do
		{
			if (i > 0 && alpha + 1 < beta)
			{
				value = -search(depth - 1, -alpha - 1, -alpha,0);
				if (value <= alpha || value >= beta)
				{
					break;
				}
			}
			value = -search(depth - 1, -beta, -alpha,0);
		} while (0);
		board->undo();
		if (terminateAI) break;

		rootCand[i].value = value;
		if (value > best.value )
		{
			best = rootCand[i];
			alpha = value;
			if (value == WinScore) return best;
		}
		
	}
	return best;

}
//搜索主函数
int AIWine::search(int depth, int alpha, int beta,int extend)
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
	if (q != 0) return q > 0 ? WinScore : LoseScore;
	//累计两次冲四或在对方没有冲四点的情况下活三，延伸一层
	if (board->isExtend()) 
	{
		extend++;
		if (extend == 2)
		{
			extend = 0;
			depth++;
		}
	}
	//到达叶节点
	if (depth <= 0)
	{
		if (board->isExpand())
		{
			depth++;
		}
		else
		{
			int lastPoint;
			int eval = board->evaluate();
			if (eval < beta)
			{
				if ((lastPoint = board->findVCFStartPoint()) != -1 && board->vcfSearch(board->who, 0, lastPoint) > 0) return WinScore;
			}
			return eval;
		}
	}
	if ((q = hashTable->queryRecord(depth, alpha, beta)) != HashTable::InvalidVal) return q;

	int hash_flag = HASH_ALPHA;
	Cand cand[MaxCand];
	int nCand = 0;
	board->generateCand(cand, nCand);

	if (nCand == 0)
	{
		board->getEmptyCand(cand, nCand);
	}
	else if (nCand > 1)
	{
		board->sortCand(cand, nCand);
	}
	int value;
	Point best = cand[0].point;
	for (int i = 0; i < nCand; i++)
	{
		board->move(cand[i].point);
		do
		{
			if (i > 0 && alpha + 1 < beta)
			{
				value = -search(depth - 1, -alpha - 1, -alpha, extend);
				if (value <= alpha || value >= beta)
				{
					break;
				}
			}
			value = -search(depth - 1, -beta, -alpha, extend);
		} while (0);
		board->undo();

		if (value >= beta)
		{
			if(!terminateAI && beta != alpha + 1) hashTable->update(beta, depth, HASH_BETA, cand[i].point);
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
	if (!terminateAI && beta != alpha + 1)
	{
		hashTable->update(alpha, depth, hash_flag, best);
	}
	return alpha;
}
//删除必败点
void AIWine::delLoseCand()
{
	for (int i = nRootCand - 1; i >= 0; i--)
	{
		if (rootCand[i].value == LoseScore)
		{
			for (int j = i + 1; j < nRootCand; j++)
			{
				rootCand[j - 1] = rootCand[j];
			}
			nRootCand--;
		}
	}
}

//判断坐标是否有效
bool AIWine::isValidPos(int x, int y)
{
	int size = board->boardSize;
	return x >= 0 && x < size && y >= 0 && y < size && board->pointPiece(x + 4, y + 4) == EMPTY;
}