#include "Board.h"
#include "ChessShape.h"
#include<assert.h>
#include<vector>
Board::Board(HashTable* _hashTable)
{
	hashTable = _hashTable;
}
//��ʼ������
void Board::initBoard(int size)
{
	memset(board, 0, sizeof(board));

	boardSize = size;
	for (int i = 0; i < 1024; i++)
	{
		if (pointX(i) >= 4 && pointX(i) < boardSize + 4 && pointY(i) >= 4 && pointY(i) < boardSize + 4)
		{
			board[i].piece = EMPTY;
		}
		else
		{
			board[i].piece = OUTSIDE;
		}
	}
	for (int i = 0; i < 1024; i++)
	{
		if (inBoard(i))
		{
			for (int k = 0; k < 4; k++)
			{
				int ii = i - MOV[k];
				for (UCHAR p = 8; p != 0; p >>= 1)
				{
					if (board[ii].piece == OUTSIDE) board[i].pattern[k][0] |= p;
					if (board[ii].piece == OUTSIDE) board[i].pattern[k][1] |= p;
					ii -= MOV[k];
				}
				ii = i + MOV[k];
				for (UCHAR p = 16; p != 0; p <<= 1)
				{
					if (board[ii].piece == OUTSIDE) board[i].pattern[k][0] |= p;
					if (board[ii].piece == OUTSIDE) board[i].pattern[k][1] |= p;
					ii += MOV[k];
				}
			}
		}
	}
	for (int i = 0; i < 1024; i++)
	{
		if (inBoard(i))
		{
			board[i].update1(0);
			board[i].update1(1);
			board[i].update1(2);
			board[i].update1(3);
			board[i].update4();
		}
	}
	chessCount = 0;
	who = BLACK;
	opp = WHITE;
	upperLeft = makePoint(boardSize + 3, boardSize + 3);
	lowerRight = makePoint(4,4);
	memset(nShape, 0, sizeof(nShape));

	//��ʼ�����ַ�Χ
	//8������
	const int range[8] = { -1,-31,-32,-33,1,31,32,33 };
	//8�����Ե�
	const int horse[8] = { -65,-63,-34,-30,30,34,63,65 };
	int n3 = 0, n4 = 0;
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Range3[n3++] = i*range[j];
		}
	}
	for (int i = 0; i < 8; i++)
	{
		Range3[n3++] = horse[i];
	}
	for (int i = 1; i <= 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Range4[n4++] = i*range[j];
		}
	}
}
//����
void Board::move(Point p)
{
	assert(check());
	ply++;
	if (ply > maxPly) maxPly = ply;
	nShape[0][board[p].shape4[0]]--;
	nShape[1][board[p].shape4[1]]--;

	hashTable->move(p, who);
	
	board[p].piece = who;
	remChess[chessCount] = &board[p];
	remPoint[chessCount] = p;
	remULCand[chessCount] = upperLeft;
	remLRCand[chessCount] = lowerRight;
	chessCount++;

	int x1 = pointX(upperLeft), y1 = pointY(upperLeft);
	int x2 = pointX(lowerRight), y2 = pointY(lowerRight);

	if (pointX(p) - 2 < x1) x1 = max(pointX(p) - 2, 4);
	if (pointY(p) - 2 < y1) y1 = max(pointY(p) - 2, 4);
	if (pointX(p) + 2 > x2) x2 = min(pointX(p) + 2, boardSize + 3);
	if (pointY(p) + 2 > y2) y2 = min(pointY(p) + 2, boardSize + 3);
	upperLeft = makePoint(x1, y1);
	lowerRight = makePoint(x2, y2);

	//����λ�����Լ�������Ϣ
	for (int k = 0; k < 4; k++)
	{
		Point move_p = p;
		//p��move_p���ұߣ���λ����
		for (UCHAR m = 16; m != 0; m <<= 1)
		{
			move_p -= MOV[k];
			int x = pointX(move_p) - 4;
			int y = pointY(move_p) - 4;
			//printf("��(%d,%d)\n", x, y);
			board[move_p].pattern[k][who] |= m;
			if (board[move_p].piece == EMPTY)
			{
				UCHAR &s0 = board[move_p].shape4[0];
				UCHAR &s1 = board[move_p].shape4[1];
				board[move_p].update1(k);
				nShape[0][s0]--; nShape[1][s1]--;
				board[move_p].update4();
				nShape[0][s0]++; nShape[1][s1]++;
				if (s0 == A) fivePoint[0] = move_p;
				if (s1 == A) fivePoint[1] = move_p;
			}
		}
		move_p = p;
		//p��move_p����ߣ���λ����
		for (UCHAR m = 8; m != 0; m >>= 1)
		{
			move_p += MOV[k];
			board[move_p].pattern[k][who] |= m;
			if (board[move_p].piece == EMPTY)
			{
				UCHAR &s0 = board[move_p].shape4[0];
				UCHAR &s1 = board[move_p].shape4[1];
				board[move_p].update1(k);
				nShape[0][s0]--; nShape[1][s1]--;
				board[move_p].update4();
				nShape[0][s0]++; nShape[1][s1]++;
				if (s0 == A) fivePoint[0] = move_p;
				if (s1 == A) fivePoint[1] = move_p;
			}
		}
	}
	//����8�������������ڵ�������
	for (int r:RANGE)
	{
		board[p + r].neighbor++;
	}
	
	assert(check());
	who = oppent(who);
	opp = oppent(opp);
}
//����
void Board::undo()
{
	assert(check());
	ply--;
	chessCount--;
	Point p = remPoint[chessCount];
	upperLeft = remULCand[chessCount];
	lowerRight = remLRCand[chessCount];

	Chess* chess = remChess[chessCount];
	chess->update1(0);
	chess->update1(1);
	chess->update1(2);
	chess->update1(3);
	chess->update4();

	nShape[0][chess->shape4[0]]++;
	nShape[1][chess->shape4[1]]++;
	if (chess->shape4[0] == A) fivePoint[0] = p;
	if (chess->shape4[1] == A) fivePoint[1] = p;
	chess->piece = EMPTY;

	who = oppent(who);
	opp = oppent(opp);

	hashTable->move(p, who);

	//����λ�����Լ�������Ϣ
	for (int k = 0; k < 4; k++)
	{
		Point move_p = p;
		//p��move_p���ұߣ�����move_p�ĸ�λ
		for (UCHAR m = 16; m != 0 ; m <<= 1)
		{
			move_p -= MOV[k];
			board[move_p].pattern[k][who] ^= m;
			if (board[move_p].piece == EMPTY)
			{
				UCHAR &s0 = board[move_p].shape4[0];
				UCHAR &s1 = board[move_p].shape4[1];
				board[move_p].update1(k);
				nShape[0][s0]--; nShape[1][s1]--;
				board[move_p].update4();
				nShape[0][s0]++; nShape[1][s1]++;
				if (s0 == A) fivePoint[0] = move_p;
				if (s1 == A) fivePoint[1] = move_p;
			}
		}
		move_p = p;
		//p��move_p�ĵ���ߣ�����move_p�ĵ�λ
		for (UCHAR m = 8; m != 0; m >>= 1)
		{
			move_p += MOV[k];
			board[move_p].pattern[k][who] ^= m;
			if (board[move_p].piece == EMPTY)
			{
				UCHAR &s0 = board[move_p].shape4[0];
				UCHAR &s1 = board[move_p].shape4[1];
				board[move_p].update1(k);
				nShape[0][s0]--; nShape[1][s1]--;
				board[move_p].update4();
				nShape[0][s0]++; nShape[1][s1]++;
				if (s0 == A) fivePoint[0] = move_p;
				if (s1 == A) fivePoint[1] = move_p;
			}
		}
	}
	//����8�������������ڵ�������
	for (int r:RANGE)
	{
		board[p + r].neighbor--;
	}
	assert(check());
}
//�������з�֧
void Board::generateCand(Cand cand[], int& nCand)
{
	nCand = 0;
	if (nShape[who][A] > 0) 
	{ 
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && board[i].shape4[who] == A)
			{
				nCand = 1;
				cand[0].point = i;
				return;
			}
		}
		assert(false);
	}
	
	if (nShape[opp][A] > 0)
	{
		nCand = 1;
		cand[0].point = fivePoint[opp];
		return;
	}
	if (nShape[who][B] > 0)
	{
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && board[i].shape4[who] == B)
			{
				nCand = 1;
				cand[0].point = i;
				return;
			}
		}
		assert(false);
	}
	//��ѯ��ϣ���е���ѵ�
	Point hashMove = 0;
	if (hashTable->present() && hashTable->depth() >= 0 && hashTable->best() != 0)
	{
		hashMove = hashTable->best();
		cand[0].point = hashMove;
		cand[0].value = WinScore;
		nCand = 1;
	}
	if (nShape[opp][B] > 0)
	{
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && i != hashMove)
			{
				if (board[i].shape4[who] >= F || board[i].shape4[opp] >= F)
				{
					cand[nCand].value = board[i].prior(who);
					cand[nCand++].point = i;
				}
			}
		}
		assert(nCand > 0 && nCand <= 256);
		return;
	}

	for (int i = upperLeft; i <= lowerRight; i++)
	{
		if (board[i].isCand() && i != hashMove)
		{
			cand[nCand].value = board[i].prior(who);
			cand[nCand].point = i;
			if (cand[nCand].value > 1) nCand++;
		}
		assert(nCand <= 256);
	}
}
//��������
int Board::evaluate()
{
	int p;
	int eval[2] = { 0 };
	Chess *c;
	for (int i = 0; i < chessCount; i++)
	{
		c = remChess[i];
		p = c->piece;
		assert(p == BLACK || p == WHITE);
		for (int k = 0; k < 4; k++)
		{
			eval[p] += ChessShape::shapeRank[c->pattern[k][p]][c->pattern[k][1 - p]];
		}
	}
	return eval[who] - eval[opp] + 70;
}

//ʤ����������(���ڼ�����Ӯ��)
int Board::quickWinSearch()
{
	int q;
	if (nShape[who][A] >= 1) return 1;   
	if (nShape[opp][A] >= 2) return -2;  
	if (nShape[opp][A] == 1)             
	{
		move(fivePoint[opp]);
		q = -quickWinSearch();
		undo();
		if (q < 0) q--; else if (q > 0) q++;
		return q;
			
	}
	if (nShape[who][B] >= 1) return 3;   
	if (nShape[who][C] >= 1)             // XOOO_ * _OO
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0) return 5;
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand()&&board[m].shape4[who] == C)
			{
				
				move(m);
				q = -quickWinSearch();
				undo();
				if (q > 0)
				{
					return q + 1;
				}
				
			}
		}
	}
	if (nShape[who][G] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			return 5;
		}
	}
	return 0;
}


Point Board::findPoint(Piece piece, FourShape shape)
{
	for (int m = upperLeft; m < lowerRight; m++)
	{
		if (board[m].isCand() && board[m].shape4[piece] == shape)
		{
			return m;
		}
	}
	return -1;
}
Point Board::findVCFLastPoint()
{
	if (chessCount < 2) return -1;
	for (int i = chessCount - 2; i >= 0; i -= 2)
	{
		Chess *c = remChess[i];
		c->updateShape();
		if (c->shape[0][who] >= BLOCK3 || c->shape[1][who] >= BLOCK3 || c->shape[2][who] >= BLOCK3 || c->shape[3][who] >= BLOCK3)
		{
			return remPoint[i];
		}
	}
	return -1;
}
Point Board::findVCTLastPoint()
{
	if (chessCount < 2) return -1;
	for (int i = chessCount - 2; i >= 0; i -= 2)
	{
		Chess *c = remChess[i];
		c->updateShape();
		if (c->shape[0][who] >= FLEX2 || c->shape[1][who] >= FLEX2 || c->shape[2][who] >= FLEX2 || c->shape[3][who] >= FLEX2)
		{
			return remPoint[i];
		}
	}
	return -1;
}
int Board::vctSearch(int *winPoint)
{
	vctStart();
	int lastPoint,depth, result;
	if ((lastPoint = findVCTLastPoint()) != -1)
	{
		for (depth = 10; depth <= MAX_VCT_DEPTH; depth += 2)
		{
			result = vctSearch(who, 0, depth, lastPoint, winPoint);
			if (result > 0 || (getTime() - t_VCT_Start) * 4 >= MAX_VCT_TIME)
			{
				break;
			}
		}
		long vctTime = getVCTCost();
		cout << "MESSAGE VCT����ʱ�䣺" << vctTime << "ms �ڵ�����" << vctNode << " ������" << __min(depth, MAX_VCT_DEPTH) << endl;
		return result;
	}
	return 0;
}
int Board::vcfSearch(int *winPoint)
{
	int lastPoint, result;
	if ((lastPoint = findVCFLastPoint()) != -1)
	{
		vcfStart();
		result = vcfSearch(who, 0, lastPoint, winPoint);
		long time = getVCFCost();
		cout << "MESSAGE VCF����ʱ�䣺" << time << "ms �ڵ�����" << vcfNode << endl;
		return result;
	}
	return 0;
}
//VCT����
int Board::vctSearch(int searcher, int depth, int maxDepth, int lastPoint)
{
	vctNode++;
	int q;
	//�����ܳ���
	if (nShape[who][A] >= 1) return 1;
	if (nShape[opp][A] >= 2) return -2;
	//�Է���һ���ܳ��壬���ڳ����
	if (nShape[opp][A] == 1)
	{
		move(fivePoint[opp]);
		q = -vctSearch(searcher, depth + 1, maxDepth, lastPoint);
		undo();
		if (q < 0) q--;
		else if (q > 0) q++;
		return q;
	}
	//�����ܳɻ���,����ʤ��
	if (nShape[who][B] >= 1) return 3;
	//���������Ȳ�����չ
	if (depth > maxDepth) return 0;
	//�Է�����ɱ�����ܻ��ģ�����
	if (who != searcher&&nShape[opp][B] >= 1)
	{
		int max_q = -1000;
		int nCand;
		Cand cand[64];
		generateCand(cand, nCand);
		if (nCand > 1) sortCand(cand, nCand);
		for (int i = 0; i < nCand; i++)
		{
			int m = cand[i].point;
			move(m);
			q = -vctSearch(searcher, depth + 1, maxDepth, lastPoint);
			undo();
			if (q > 0)	//�и����ص���Ӯ������Ӯ
			{
				return q + 1;
			}
			else if (q == 0) return 0;
			else if (q > max_q) max_q = q;
		}
		return max_q;//q>0ʱ������ǰ���أ�ʣ�µ�������������0������0�����򷵻�����qֵ������ֱ�ӷ���max_q����
	}
	//��������ɱ�����г��Ļ���������
	if (who == searcher&&nShape[who][C] >= 1)
	{
		//�Է�û���ܳ��ĵĵ㣬�岽ʤ��
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0) return q + 1;
			}
		}
	}
	//��������ɱ��,����ʣ�µ����г��ĵ㣨�������Ļ���)
	if (who == searcher&&nShape[who][D] + nShape[who][E] >= 1)
	{
		//������ɱֻ�������ַ�Χ����ֹvct��ը
		for (int r : Range4)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == D|| board[m].shape4[who] == E))
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0) return q + 1;
			}
		}
	}
	//����������˫����
	if (who == searcher&&nShape[who][G]>0)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == G)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0) return q + 1;
			}
		}
	}

	//���Ի�������������
	if (who == searcher && nShape[who][H] + nShape[who][I] >= 1)
	{
		//������ɱֻ�������ַ�Χ����ֹvct��ը
		for (int r : Range3)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == H|| board[m].shape4[who] == I))
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0) return q + 1;
			}
		}
	}

	return 0;
}
//VCT����
int Board::vctSearch(int searcher, int depth, int maxDepth, int lastPoint, int* winPoint)
{
	vctNode++;
	int q;
	//�����ܳ���
	if (nShape[who][A] >= 1)
	{
		*winPoint = findPoint(who, A);
		return 1;
	}
	if (nShape[opp][A] >= 2) return -2;
	//�Է���һ���ܳ��壬���ڳ����
	if (nShape[opp][A] == 1)
	{
		move(fivePoint[opp]);
		q = -vctSearch(searcher, depth + 1, maxDepth, lastPoint);
		undo();
		if (q < 0) q--;
		else if (q > 0)
		{
			*winPoint = fivePoint[opp];
			q++;
		}
		return q;
	}
	//�����ܳɻ���,����ʤ��
	if (nShape[who][B] >= 1)
	{
		*winPoint = findPoint(who, B);
		return 3;
	}
	//���������ȣ�������չ
	if (depth > maxDepth) return 0;
	//�Է�����ɱ�����ܻ��ģ�����
	if (who != searcher&&nShape[opp][B] >= 1)
	{
		int max_q = -1000;
		int nCand;
		Cand cand[64];
		generateCand(cand, nCand);
		if (nCand > 1) sortCand(cand, nCand);
		for (int i = 0; i < nCand; i++)
		{
			int m = cand[i].point;
			move(m);
			q = -vctSearch(searcher, depth + 1, maxDepth, lastPoint);
			undo();
			if (q > 0)	//�и����ص���Ӯ������Ӯ
			{
				*winPoint = m;
				return q + 1;
			}
			else if (q == 0) return 0;
			else if (q > max_q) max_q = q;
		}
		return max_q;//q>0ʱ������ǰ���أ�ʣ�µ�������������0������0�����򷵻�����qֵ������ֱ�ӷ���max_q����
	}
	//��������ɱ�����г��Ļ���������
	if (who == searcher&&nShape[who][C] >= 1)
	{
		//�Է�û���ܳ��ĵĵ㣬�岽ʤ��
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			*winPoint = findPoint(who, C);
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}

			}
		}
	}
	
	//��������ɱ��,����ʣ�µ����г��ĵ㣨�������Ļ���)
	if (who == searcher&&nShape[who][D] + nShape[who][E] >= 1)
	{
		//������ɱֻ�������ַ�Χ����ֹvct��ը
		for (int r : Range4)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == D || board[m].shape4[who] == E))
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}

			}
		}
	}
	//����������˫����
	if (who == searcher&&nShape[who][G]>0)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			*winPoint = findPoint(who, G);
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == G)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}
			}
		}
	}

	//���Ի�������������
	if (who == searcher && nShape[who][H] + nShape[who][I] >= 1)
	{
		//������ɱֻ�������ַ�Χ����ֹvct��ը
		for (int r : Range3)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == H || board[m].shape4[who] == I))
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}
			}
		}
	}

	return 0;
}

//vcf����
int Board::vcfSearch(int searcher, int depth, int lastPoint)
{
	int q;
	if (nShape[who][A] >= 1) return 1;
	if (nShape[opp][A] >= 2) return -2;
	//�Է���һ���ܳ��壬���ڳ����
	if (nShape[opp][A] == 1)
	{
		move(fivePoint[opp]);
		q = -vcfSearch(searcher, depth + 1, lastPoint);
		undo();
		if (q < 0) q--; else if (q > 0) q++;
		return q;
	}
	//�����ܳɻ���,����ʤ��
	if (nShape[who][B] >= 1) return 3;
	//�����г��Ļ���������
	if (who == searcher&&nShape[who][C] >= 1)
	{
		//�Է�û���ܳ��ĵĵ㣬�岽ʤ��
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0) return 5;
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1, m);
				undo();
				if (q > 0) return q + 1;

			}
		}
	}
	//�����г��ĺ���������(������������߶��е�һ��)
	if (who == searcher&&depth<MAX_VCF_DEPTH && nShape[who][D] + nShape[who][E] >= 1)
	{
		for (int r : Range4)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == D || board[m].shape4[who] == E))
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1, m);
				undo();
				if (q > 0) return q + 1;
			}
		}
	}
	//�����ܳ�˫�������Է�û���κγ��Ļ����,�岽ʤ��
	if (who == searcher&&nShape[who][G] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0) return 5;
	}
	return 0;
}

//vcf����
int Board::vcfSearch(int searcher, int depth, int lastPoint, int *winPoint)
{
	int q;
	vcfNode++;
	if (nShape[who][A] >= 1)
	{
		*winPoint = findPoint(who, A);
		return 1;
	}
	if (nShape[opp][A] >= 2) return -2;
	//�Է���һ���ܳ��壬���ڳ����
	if (nShape[opp][A] == 1)
	{
		move(fivePoint[opp]);
		q = -vcfSearch(searcher, depth + 1, lastPoint);
		undo();
		if (q < 0) q--;
		else if (q > 0)
		{
			*winPoint = fivePoint[opp];
			q++;
		}
		return q;
	}
	//�����ܳɻ���,����ʤ��
	if (nShape[who][B] >= 1)
	{
		*winPoint = findPoint(who, B);
		return 3;
	}
	//�����г��Ļ���������
	if (who == searcher&&nShape[who][C] >= 1)
	{
		//�Է�û���ܳ��ĵĵ㣬�岽ʤ��
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			*winPoint = findPoint(who, C);
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}
			}
		}
	}
	//�����г��ĺ���������(������������߶��е�һ��)
	if (who == searcher&&depth<MAX_VCF_DEPTH && nShape[who][D] + nShape[who][E] >= 1)
	{
		for (int r : Range4)
		{
			int m = lastPoint + r;
			if (board[m].isCand() && (board[m].shape4[who] == D || board[m].shape4[who] == E))
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1, m);
				undo();
				if (q > 0)
				{
					*winPoint = m;
					return q + 1;
				}
			}
		}
	}
	//�����ܳ�˫�������Է�û���κγ��Ļ����,�岽ʤ��
	if (who == searcher&&nShape[who][G] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0 && nShape[opp][F] == 0)
		{
			*winPoint = findPoint(who, G);
			return 5;
		}
	}
	return 0;
}


//��ȡ���пյĵ�
void Board::getEmptyCand(Cand cand[], int &nCand)
{
	for (int m = upperLeft; m < lowerRight; m++)
	{
		if (board[m].piece==EMPTY)
		{
			cand[nCand++] = { m,0 };
		}
	}
}
//������������Ƿ���ȷ
bool Board::check()
{
	int n[2][10] = { 0 };
	for (int m = 0; m < 1024; m++)
	{
		if (board[m].piece==EMPTY)
		{
			n[0][board[m].shape4[0]]++;
			n[1][board[m].shape4[1]]++;
		}
	}
	for (int i = 0; i < 2; i++)
		for (int j = 1; j < 10; j++)
			if (n[i][j] != nShape[i][j])
			{
				return false;
			}
	return true;
}
//����ѡ��
void Board::sortCand(Cand cand[], int nCand)
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
