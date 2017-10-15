#include "Board.h"
#include "ChessShape.h"
#include<assert.h>
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
	memset(remShape, 0, sizeof(remShape));
}
//落子
void Board::move(Point p)
{
	assert(check());
	ply++;
	if (ply > maxPly) maxPly = ply;
	nShape[0][board[p].shape4[0]]--;
	nShape[1][board[p].shape4[1]]--;

	int pointPiece = who;
	
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

	//更新位编码以及棋型信息
	for (int k = 0; k < 4; k++)
	{
		Point move_p = p;
		//p在move_p的右边，高位更新
		for (UCHAR m = 16; m != 0; m <<= 1)
		{
			move_p -= MOV[k];
			board[move_p].pattern[k][who] |= m;
			if (board[move_p].piece == EMPTY)
			{
				board[move_p].update1(k);
				nShape[0][board[move_p].shape4[0]]--; nShape[1][board[move_p].shape4[1]]--;
				board[move_p].update4();
				nShape[0][board[move_p].shape4[0]]++; nShape[1][board[move_p].shape4[1]]++;
				//remShape[0][board[move_p].shape4[0]] = move_p; remShape[1][board[move_p].shape4[1]] = move_p;
			}
		}
		move_p = p;
		//p在move_p的左边，低位更新
		for (UCHAR m = 8; m != 0; m >>= 1)
		{
			move_p += MOV[k];
			board[move_p].pattern[k][who] |= m;
			if (board[move_p].piece == EMPTY)
			{
				board[move_p].update1(k);
				nShape[0][board[move_p].shape4[0]]--; nShape[1][board[move_p].shape4[1]]--;
				board[move_p].update4();
				nShape[0][board[move_p].shape4[0]]++; nShape[1][board[move_p].shape4[1]]++;
				//remShape[0][board[move_p].shape4[0]] = move_p; remShape[1][board[move_p].shape4[1]] = move_p;
			}
		}
	}
	//更新8个方向，两步以内的棋子数
	for (int i = 0; i < 16; i++)
	{
		board[p + RANGE[i]].neighbor++;
	}
	
	
	/*bool flag = check();
	if (flag == false)
	{
		cout << "MESSAGE mowho为" + getPiece(pointPiece) << endl;
		string whoName = getPiece(who) + ":";
		string oppName = getPiece(opp) + ":";
		cout << "MESSAGE" << " who:" << whoName << " opp:" << oppName << endl;
		int p2 = makePoint(8, 13);
		cout << "MESSAGE" << " failPoint:" << pointX(p) - 4 << "," << pointY(p) - 4 << endl;
		cout << "MESSAGE" << " shape4:" <<whoName<< getShape4Name(board[p].shape4[who])<< oppName << getShape4Name(board[p].shape4[opp])<<"|"<<whoName<< getShape4Name(board[p2].shape4[who]) << oppName << getShape4Name(board[p2].shape4[opp]) << endl;
		cout << "MESSAGE" << " nshape:" << nShape[who][A] << "," << nShape[opp][A] << endl;
	}*/
	assert(check());
	who = oppent(who);
	opp = oppent(opp);
}
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
	assert(chess->piece == BLACK || chess->piece == WHITE);
	chess->piece = EMPTY;

	who = oppent(who);
	opp = oppent(opp);

	//更新位编码以及棋型信息
	for (int k = 0; k < 4; k++)
	{
		Point move_p = p;
		//p在move_p的右边，更新move_p的高位
		for (UCHAR m = 16; m != 0 ; m <<= 1)
		{
			move_p -= MOV[k];
			board[move_p].pattern[k][who] ^= m;
			if (board[move_p].piece == EMPTY)
			{
				board[move_p].update1(k);
				nShape[0][board[move_p].shape4[0]]--; nShape[1][board[move_p].shape4[1]]--;
				board[move_p].update4();
				nShape[0][board[move_p].shape4[0]]++; nShape[1][board[move_p].shape4[1]]++;
				//remShape[0][board[move_p].shape4[0]] = move_p; remShape[1][board[move_p].shape4[1]] = move_p;
			}
		}
		move_p = p;
		//p在move_p的的左边，更新move_p的低位
		for (UCHAR m = 8; m != 0; m >>= 1)
		{
			move_p += MOV[k];
			board[move_p].pattern[k][who] ^= m;
			if (board[move_p].piece == EMPTY)
			{
				board[move_p].update1(k);
				nShape[0][board[move_p].shape4[0]]--; nShape[1][board[move_p].shape4[1]]--;
				board[move_p].update4();
				nShape[0][board[move_p].shape4[0]]++; nShape[1][board[move_p].shape4[1]]++;
				//remShape[0][board[move_p].shape4[0]] = move_p; remShape[1][board[move_p].shape4[1]] = move_p;
			}
		}
	}
	//更新8个方向，两步以内的棋子数
	for (int i = 0; i < 16; i++)
	{
		board[p + RANGE[i]].neighbor--;
	}
	assert(check());
}
//生成所有分支
void Board::generateCand(Cand cand[], int& nCand)
{
	
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
		/*nCand = 1;
		cand[0].point = remShape[who][A];*/
		return;
	}
	
	if (nShape[opp][A] > 0)
	{
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && board[i].shape4[opp] == A)
			{
				nCand = 1;
				cand[0].point = i;
				return;
			}
		}
		/*nCand = 1;
		cand[0].point = remShape[opp][A];*/
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
		/*nCand = 1;
		cand[0].point = remShape[who][B];*/
		return;
	}
	if (nShape[opp][B] > 0)
	{
		nCand = 0;
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand()&&(board[i].shape4[who] >= E || board[i].shape4[opp] >= E))
			{
				cand[nCand].value = board[i].prior();
				cand[nCand].point = i;
				if (cand[nCand].value >= 5) nCand++;
				assert(nCand <= 256);
			}
		}
		return;
	}

	nCand = 0;
	for (int i = upperLeft; i <= lowerRight; i++)
	{
		if (board[i].isCand())
		{
			cand[nCand].value = board[i].prior();
			cand[nCand].point = i;
			if (cand[nCand].value > 0) nCand++;
			assert(nCand <= 256);
		}
	}
}
//局面评估
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
			eval[p] += ChessShape::shapePrior[c->pattern[k][p]][c->pattern[k][1 - p]];
		}
	}
	return eval[who] - eval[opp];
}
int Board::quickWinSearch()
{
	int q;
	if (nShape[who][A] >= 1) return 1;   
	if (nShape[opp][A] >= 2) return -2;  
	if (nShape[opp][A] == 1)             
	{
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand()&& board[m].shape4[opp] == A)
			{
				/*int m = remShape[opp][A];*/
				move(m);
				q = -quickWinSearch();
				undo();
				if (q < 0) q--; else if (q > 0) q++;
				return q;
			}
		}
			
	}
	if (nShape[who][B] >= 1) return 3;   
	if (nShape[who][C] >= 1)             // XOOO_ * _OO
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0) return 5;
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand()&&board[m].shape4[who] == C)
			{
				
				move(m);
				q = -quickWinSearch();
				undo();
				if (q > 0)
				{
					//cout << "MESSAGE" << " ply=" << ply << " who=" << who  << " point=" << pointX(m) - 4 << "," << pointY(m) - 4 << " 四三胜" << endl;
					return q + 1;
				}
				
			}
		}
	}
	if (nShape[who][F] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0)
		{
			/*int lastPoint = remPoint[chessCount - 1];
			cout << "MESSAGE" << " ply=" << ply << " who=" << who << " lastPoint=" << pointX(lastPoint) - 4 << "," << pointY(lastPoint) - 4 << " 三三胜" << endl;*/

			return 5;
		}
	}
	return 0;
}
int Board::vcfSearch()
{
	int q;
	if (nShape[who][A] >= 1) return 1;
	if (nShape[opp][A] >= 2) return -2;
	if (nShape[opp][A] == 1)
	{
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[opp] == A)
			{
				/*int m = remShape[opp][A];*/
				move(m);
				q = -quickWinSearch();
				undo();
				if (q < 0) q--; else if (q > 0) q++;
				return q;
			}
		}

	}
	if (nShape[who][B] >= 1) return 3;
	if (nShape[who][C] >= 1)             // XOOO_ * _OO
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0) return 5;
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -quickWinSearch();
				undo();
				if (q > 0) return q + 1;

			}
		}
	}
	if (nShape[who][F] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0) return 5;
	}
	//vcf扩展
	if (ply < LimitPly && nShape[who][D] >= 1)
	{
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == D)
			{
				move(m);
				q = -quickWinSearch();
				undo();
				if (q > 0) return q + 1;
			}
		}
	}
		
	return 0;
}
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
				//cout << "MESSAGE" << " checkfail piece=" << getPiece(i) << " shape=" << getShape4Name(j) << endl;
				return false;
			}
	return true;
}