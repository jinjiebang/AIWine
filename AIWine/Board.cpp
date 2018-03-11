#include "Board.h"
#include "ChessShape.h"
#include<assert.h>
Board::Board(HashTable* _hashTable)
{
	hashTable = _hashTable;
}
//初始化棋盘
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

	//初始化米字范围
	//8个方向
	const int range[8] = { -1,-31,-32,-33,1,31,32,33 };
	int n3 = 0, n4 = 0;
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Range3[n3++] = i*range[j];
		}
	}
	for (int i = 1; i <= 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Range4[n4++] = i*range[j];
		}
	}
}
//落子
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

	//更新位编码以及棋型信息
	for (int k = 0; k < 4; k++)
	{
		Point move_p = p;
		//p在move_p的右边，高位更新
		for (UCHAR m = 16; m != 0; m <<= 1)
		{
			move_p -= MOV[k];
			int x = pointX(move_p) - 4;
			int y = pointY(move_p) - 4;
			//printf("点(%d,%d)\n", x, y);
			board[move_p].pattern[k][who] |= m;
			if (board[move_p].piece == EMPTY)
			{
				board[move_p].update1(k);
				nShape[0][board[move_p].shape4[0]]--; nShape[1][board[move_p].shape4[1]]--;
				
				board[move_p].update4();
				int a = board[move_p].shape4[0];
				int b = board[move_p].shape4[1];
				//printf("点(%d,%d)的棋型是黑：%d 白：%d\n",x,y,a,b);
				nShape[0][a]++; nShape[1][b]++;
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
			}
		}
	}
	//更新8个方向，两步以内的棋子数
	for (int i = 0; i < 16; i++)
	{
		board[p + RANGE[i]].neighbor++;
	}
	
	assert(check());
	who = oppent(who);
	opp = oppent(opp);
}
//悔棋
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
	chess->piece = EMPTY;

	who = oppent(who);
	opp = oppent(opp);

	hashTable->move(p, who);

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
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && board[i].shape4[opp] == A)
			{
				nCand = 1;
				cand[0].point = i;
				return;
			}
		}
		assert(false);
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
	//查询哈希表中的最佳点
	Point hashMove = 0;
	if (hashTable->present() && hashTable->best() != 0)
	{
		hashMove = hashTable->best();
		cand[0].point = hashMove;
		cand[0].value = 10000;
		nCand = 1;
	}
	if (nShape[opp][B] > 0)
	{
		for (int i = upperLeft; i <= lowerRight; i++)
		{
			if (board[i].isCand() && i != hashMove)
			{
				if (board[i].shape4[who] >= E || board[i].shape4[opp] >= E)
				{
					cand[nCand].value = board[i].prior(who);
					cand[nCand].point = i;
					if (cand[nCand].value >= 5) nCand++;
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
			if (cand[nCand].value >= 5) nCand++;
		}
		assert(nCand <= 256);
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
			eval[p] += ChessShape::shapeRank[c->pattern[k][p]][c->pattern[k][1 - p]];
		}
	}
	return eval[who] - eval[opp];
}
//胜利局面搜索(将在几步内赢棋)
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
					return q + 1;
				}
				
			}
		}
	}
	if (nShape[who][F] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0)
		{
			return 5;
		}
	}
	return 0;
}
//vcf搜索
int Board::vcfSearch(int searcher,int depth)
{
	int q;
	if (nShape[who][A] >= 1) return 1;
	if (nShape[opp][A] >= 2) return -2;
	//对方下一步能成五，挡在成五点
	if (nShape[opp][A] == 1)
	{
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[opp] == A)
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1);
				undo();
				if (q < 0) q--; else if (q > 0) q++;
				return q;
			}
		}

	}
	//本方能成活四,三步胜利
	if (nShape[who][B] >= 1) return 3;
	//本方有冲四活三，尝试
	if (nShape[who][C] >= 1)             // XOOO_ * _OO
	{
		//对方没有能成四的点，五步胜利
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0) return 5;
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vcfSearch(searcher, depth + 1);
				undo();
				if (q > 0) return q + 1;

			}
		}
	}
	//本方能成双活三，对方没有任何冲四或活四,五步胜利
	if (nShape[who][F] >= 1)
	{
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0) return 5;
	}
	//vcf扩展
	if (who==searcher&&depth<MAX_VCF_DEPTH && nShape[who][D] >= 1&&chessCount>=2)
	{
		if (depth == 0)//第一步搜索全部点
		{
			for (int m = upperLeft; m < lowerRight; m++)
			{
				if (board[m].isCand() && board[m].shape4[who] == D)
				{
					move(m);
					q = -vcfSearch(searcher, depth + 1);
					undo();
					if (q > 0) return q + 1;
				}
			}
		}
		else //后续算杀只考虑米字范围，防止vcf爆炸
		{
			Point whoLast = remPoint[chessCount - 2];
			for (int i = 0; i < 32; i++)
			{
				int m = whoLast + Range4[i];
				if (board[m].isCand() && board[m].shape4[who] == D)
				{
					move(m);
					q = -vcfSearch(searcher, depth + 1);
					undo();
					if (q > 0) return q + 1;
				}
			}
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
void Board::getAllPoint(Point point[], int &nPoint)
{
	nPoint = 0;
	for (int m = upperLeft; m < lowerRight; m++)
	{
		if (board[m].isCand()&&board[m].shape4[who]>=H|| board[m].shape4[opp] >= H)
		{
			point[nPoint++] = m;
		}
	}
}
int Board::vctSearch(int *winPoint)
{
	t_VCT_Start = getTime();
	vctNode = 0;
	vctStop = false;
	int result = 0;
	int depth;
	for (depth = 10; depth <= MAX_VCT_DEPTH; depth+=2)
	{
		result = vctSearch(who, 0, depth, 0,0, winPoint); 
		if (result > 0||vctStop)
		{
			break;
		}
	}
	long vctTime = getTime() - t_VCT_Start;
	cout << "MESSAGE VCT花费时间：" << vctTime << "ms 节点数：" << vctNode << " 层数：" << depth << " 超时停止：" << vctStop << endl;
	return result;
}
//VCT搜索
int Board::vctSearch(int searcher,int depth,int maxDepth,int lastThree, int lastThree2,int* winPoint)
{
	if (depth > maxDepth|| vctStop) return 0;
	static int cnt;
	if (--cnt<0)
	{
		cnt = 1000;
		if (getTime() - t_VCT_Start > 1000)
		{
			vctStop = true;
		}
	}
	vctNode++;
	int q;
	//本方能成五
	if (nShape[who][A] >= 1)
	{
		if (depth == 0) *winPoint = findPoint(who, A);
		return 1;
	}
	if (nShape[opp][A] >= 2) return -2;
	//对方下一步能成五，挡在成五点
	if (nShape[opp][A] == 1)
	{
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[opp] == A)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, lastThree, lastThree2, winPoint);
				undo();
				if (q < 0) q--; 
				else if (q > 0)
				{
					if (depth == 0) *winPoint = m;
					q++;
				}
				return q;
			}
		}

	}
	//本方能成活四,三步胜利
	if (nShape[who][B] >= 1)
	{
		if (depth == 0) *winPoint = findPoint(who, B);
		return 3;
	}
	//对方是算杀方且能活四，防守
	if (who != searcher&&nShape[opp][B] >= 1)
	{
		int max_q = -1000;
		/*int newDefendFour = defendFour;*/
		for (int m = upperLeft; m <= lowerRight; m++)
		{
			if (board[m].isCand() && (board[m].shape4[opp] >= E||board[m].shape4[who]>=E))
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, lastThree, lastThree2, winPoint);
				undo();
				if (q > 0)
				{
					if (depth == 0) *winPoint = m;
					return q + 1;//有个防守点能赢，就算赢
				}
				else if (q > max_q) max_q = q;
			}
		}
		return max_q;//q>0时，会提前返回，剩下的情况，如果等于0，返回0，否则返回最大的q值，所以直接返回max_q即可
	}
	//本方是算杀方，有冲四活三，尝试
	if (who==searcher&&nShape[who][C] >= 1)
	{
		//对方没有能成四的点，五步胜利
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0)
		{
			if (depth == 0) *winPoint = findPoint(who, C);
			return 5;
		}
		for (int m = upperLeft; m < lowerRight; m++)
		{
			if (board[m].isCand() && board[m].shape4[who] == C)
			{
				move(m);
				q = -vctSearch(searcher, depth + 1, maxDepth, lastThree, lastThree2, winPoint);
				undo();
				if (q > 0)
				{
					if (depth == 0) *winPoint = m;
					return q + 1;
				}

			}
		}
	}
	//当前方最后一步棋位置
	Point whoLast = remPoint[chessCount - 2];		
	//本方是算杀方,尝试剩下的所有冲四点（除掉冲四活三)
	if(who==searcher&&(nShape[who][D] >= 1 || nShape[who][E] >= 1))
	{
		if (depth == 0)//第一步要尝试所有点
		{
			for (int m = upperLeft; m < lowerRight; m++)
			{
				if (board[m].isCand() && (board[m].shape4[who] == D || board[m].shape4[who] == E))
				{
					move(m);
					q = -vctSearch(searcher, depth + 1, maxDepth, lastThree, lastThree2, winPoint);
					undo();
					if (q > 0)
					{
						*winPoint = m;
						return q + 1;
					}
				}
			}
		} else {
			//后续算杀只考虑米字范围，防止vct爆炸
			for (int i = 0; i < 32; i++)
			{
				int m = whoLast + Range4[i];
				if (board[m].isCand() && (board[m].shape4[who] == D || board[m].shape4[who] == E))
				{
					move(m);
					q = -vctSearch(searcher, depth + 1, maxDepth, lastThree, lastThree2, winPoint);
					undo();
					if (q > 0) return q + 1;
				}
			}
		}
		
	}
	
	
	//本方能成双活三，对方没有任何冲四或活四,五步胜利
	if (who == searcher&&nShape[who][F] >= 1)
	{
		
		if (nShape[opp][B] == 0 && nShape[opp][C] == 0 && nShape[opp][D] == 0 && nShape[opp][E] == 0)
		{
			if (depth == 0) *winPoint = findPoint(who, F);
			return 5;
		}
	}

	//尝试活三带活二或眠三以及单独的活三
	if (who== searcher&&chessCount >= 2 && (nShape[who][F] >= 1||nShape[who][G] >= 1))
	{
		if (depth == 0)
		{
			for (int m = upperLeft; m <= lowerRight; m++)
			{
				if (board[m].isCand() && board[m].shape4[who] >= G && board[m].shape4[who] <= F)
				{
					move(m);
					q = -vctSearch(searcher, depth + 1, maxDepth, m,lastThree, winPoint);
					undo();
					if (q > 0)
					{
						*winPoint = m;
						return q + 1;
					}
				}
			}
		}
		else
		{
			///后续算杀只考虑米字范围，防止vct爆炸
			if (lastThree > 0) whoLast = lastThree;
			int record[1024] = { 0 };	//记录这个点是否算过
			for (int i = 0; i < 32; i++)
			{
				int m = whoLast + Range4[i];
				if (board[m].isCand() && board[m].shape4[who] >= G && board[m].shape4[who] <= F)
				{
					record[m] = 1;
					move(m);
					q = -vctSearch(searcher, depth + 1, maxDepth, m,lastThree, winPoint);
					undo();
					if (q > 0) return q + 1;
				}
			}
			if (lastThree2 > 0)
			{
				for (int i = 0; i < 32; i++)
				{
					int m = lastThree2 + Range4[i];
					if (record[m]==0&&board[m].isCand() && board[m].shape4[who] >= G && board[m].shape4[who] <= F)
					{
						move(m);
						q = -vctSearch(searcher, depth + 1, maxDepth, m, lastThree, winPoint);
						undo();
						if (q > 0) return q + 1;
					}
				}
			}
		}
		
	}

	return 0;
}
//获取所有空的点
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
//检查棋型数量是否正确
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