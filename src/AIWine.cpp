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
//�������̳ߴ�
bool AIWine::setSize(int size)
{
	if (size<5 || size>MaxSize)
	{
		return false;
	}
	board->initBoard(size);
	return true;
}
//���¿�ʼ
void AIWine::restart()
{
	board->initBoard(board->boardSize);
}
//����
void AIWine::turnUndo()
{
	board->undo();
}
//��ʾ��������Ϣ
void AIWine::showDepthInfo(int depth,Cand best, long td)
{
	cout << "MESSAGE depth: " << depth << "-" << board->maxPly
		<< " best:[" << pointX(best.point) - 4 << "," << pointY(best.point) - 4 << "]"
		<< " val=" << best.value << " time:" << td << "ms" << endl;
}
//����
void AIWine::turnMove(int x, int y)
{
	int point = makePoint(x + 4, y + 4);
	board->move(point);
}
//ɾ���ᱻVCTʤ�ĵ�,
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
		lastPoint = board->findVCTLastPoint();
		
		if (lastPoint != -1 && (result = board->vctSearch(board->who, 0, delVctLoseDepth, lastPoint)) > 0)//�Է�VCTʤ��
		{
			c.value = LoseScore;
			// ������Ǳذܣ�ѡ�����Ӳ������Ϊ��ѵ�
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
//���Է�VCT,�ӷ�����ߵĵ㿪ʼ��VCT,ֱ���ҵ�һ�����ܵĵ�
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
			//����VCTʤ��
			if ((lastPoint = board->findVCTLastPoint()) != -1 && (result = board->vctSearch(board->who, 0, checkOppVctDepth, lastPoint)) > 0)
			{
				c.value = LoseScore;
				findVct = true;
				// ������Ǳذܣ�ѡ�����Ӳ������Ϊ��ѵ�
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
//��ȡ��ѵ�
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
	//����vcf
	if (board->vcfSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCF��ɱ�ɹ�����ʤ��["<< pointX(rootBest.point) - 4 <<", "<< pointY(rootBest.point) - 4 <<"]"<< endl;
	}
	//����vct
	if (!isSolved&&board->vctSearch(&rootBest.point) > 0)
	{
		isSolved = true;
		cout << "MESSAGE VCT��ɱ�ɹ�����ʤ��[" << pointX(rootBest.point) - 4 << ", " << pointY(rootBest.point) - 4 << "]" << endl;
	}
	//��Է���VCT,ɾ���Է���VCTʤ�ĵ�
	if (!isSolved)
	{
		board->generateCand(rootCand, nRootCand);
		board->sortCand(rootCand, nRootCand);
		int bestPoint = delVctLose();
		if (nRootCand == 0)
		{
			isSolved = true;
			rootBest.point = bestPoint;
			cout << "MESSAGE �Է�VCT��ʤ!"<< endl;
		}
		else if (nRootCand == 1)
		{
			//����û�а�isSolved��Ϊtrue,����ͨ������������һ�����۷�
			cout << "MESSAGE ͨ��VCT������Ψһ����!" << endl;
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
			cout << "MESSAGE ÿ���������VCT����ʱ��" << board->getVCTCost() << "ms" << endl;
			if (nRootCand == 0)
			{
				rootBest.point = best.point;
				cout << "MESSAGE �Է�VCT��ʤ!" << endl;
				break;
			}
			else if (nRootCand == 1)
			{
				cout << "MESSAGE ͨ��VCT������Ψһ����!" << endl;
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
//���ڵ�����
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
//����������
int AIWine::search(int depth, int alpha, int beta,int extend)
{
	static int cnt;
	if (--cnt<0)
	{
		cnt = 1000;
		if (getTime() - stopTime() > 0) terminateAI = true;
	}
	nSearched++;
	//��ʤ
	int q = board->quickWinSearch();
	if (q != 0) return q > 0 ? WinScore : LoseScore;
	//�ۼ����γ��Ļ��ڶԷ�û�г��ĵ������»���������һ��
	if (board->isExtend()) 
	{
		extend++;
		if (extend == 2)
		{
			extend = 0;
			depth++;
		}
	}
	//����Ҷ�ڵ�
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
				if ((lastPoint = board->findVCFLastPoint()) != -1 && board->vcfSearch(board->who, 0, lastPoint) > 0) return WinScore;
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
//ɾ���ذܵ�
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

//�ж������Ƿ���Ч
bool AIWine::isValidPos(int x, int y)
{
	int size = board->boardSize;
	return x >= 0 && x < size && y >= 0 && y < size && board->pointPiece(x + 4, y + 4) == EMPTY;
}