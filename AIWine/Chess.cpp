#include "Chess.h"
#include "ChessShape.h"
void Chess::update1(int k)
{
	shape[k][0] = ChessShape::shapeTable[pattern[k][0]][pattern[k][1]];
	shape[k][1] = ChessShape::shapeTable[pattern[k][1]][pattern[k][0]];
}
void Chess::update4()
{
	shape4[0] = ChessShape::fourShapeTable[shape[0][0]][shape[1][0]][shape[2][0]][shape[3][0]];
	shape4[1] = ChessShape::fourShapeTable[shape[0][1]][shape[1][1]][shape[2][1]][shape[3][1]];
}
short Chess::prior()
{
	return ChessShape::calPrior(pattern);
}