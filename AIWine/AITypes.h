#pragma once
typedef unsigned char  UCHAR;
typedef unsigned long long  U64;
typedef int Point;
const int MOV[4] = { 1,31,32,33 };
const int RANGE[16] = { -66,-64,-62,-33,-32,-31,-2,-1,1,2,31,32,33,62,64,66 };
enum Piece { EMPTY = 2, WHITE = 0, BLACK = 1, OUTSIDE = 3 };
enum Shape { NONE, BLOCK1, FLEX1, BLOCK2, FLEX2, BLOCK3, FLEX3, BLOCK4, FLEX4, FIVE };
enum HashFlag { HASH_EXACT, HASH_ALPHA, HASH_BETA };
enum FourShape { A = 8, B = 7, C = 6, D = 5, E = 4, F = 3, G = 2, H = 1, FORBID = 9 };
struct Cand
{
	Cand(int p = 0, int v = 0) : point(p), value(v) {}
	Point point;
	int value;
};