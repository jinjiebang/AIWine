#include "AIWine.h"
#include "ChessShape.h"
#include <assert.h>
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

void simpleUI();
//转换成大写字母
void toupper(string &str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		char &c = str[i];
		if (c >= 'a' && c <= 'z')
		{
			c += 'A' - 'a';
		}
	}
}

void gomocup()
{
	AIWine* ai= new AIWine();
	string command;
	char dot;
	int size;
	bool isEnded = false;
	bool isSimpleUi = false;
	while (1)
	{
		cin >> command;
		toupper(command);

		if (command == "START")
		{
			cin >> size;
			if (ai->setSize(size))
			{
				cout << "OK" << endl;
			}
			else
			{
				cout << "ERROR" << endl;
			}
		}
		else if (command == "RESTART")
		{
			ai->restart();
			cout << "OK" << endl;
		}
		else if (command == "TAKEBACK")
		{
			ai->turnUndo();
			cout << "OK" << endl;
		}
		else if (command == "BEGIN")
		{
			int x, y;
			ai->turnBest(x, y);
			cout << x << "," << y << endl;
		}
		else if (command == "TURN")
		{
			int x, y;
			cin >> x >> dot >> y;
			if (ai->isValidPos(x, y))
			{
				ai->turnMove(x, y);
				ai->turnBest(x, y);
				cout << "MESSAGE " << "node:" << ai->nSearched / 1000 << "k eval:" << ai->rootBest.value << endl;
				cout << x << "," << y << endl;
			}
			else
			{
				cout << "ERROR" << endl;
			}
		}
		else if (command == "BOARD")
		{
			int x, y, c;
			stringstream ss;
			ai->restart();

			cin >> command;
			toupper(command);
			while (command != "DONE")
			{
				ss.clear();
				ss << command;
				ss >> x >> dot >> y >> dot >> c;
				if (ai->isValidPos(x,y))
				{
					ai->turnMove(x, y);
				}
				else
				{
					cout << "ERROR" << endl;
				}
				cin >> command;
				toupper(command);
			}
			if (c == 2)
			{
				ai->turnBest(x, y);
				cout << "MESSAGE " << "node:" << ai->nSearched << " eval:" << ai->rootBest.value << endl;
				cout << x << "," << y << endl;
			}
		}
		else if (command == "INFO")
		{
			int value;
			string key;
			cin >> key;
			toupper(key);

			if (key == "TIMEOUT_TURN")
			{
				cin >> value;
				if (value != 0) ai->timeout_turn = value;
			}
			else if (key == "TIMEOUT_MATCH")
			{
				cin >> value;
				if (value != 0) ai->timeout_match = value;
			}
			else if (key == "TIME_LEFT")
			{
				cin >> value;
				if (value != 0) ai->time_left = value;
			}
			else if (key == "MAX_MEMORY")
			{
				cin >> value;
				// TODO
			}
			else if (key == "GAME_TYPE")
			{
				cin >> value;
				// TODO
			}
			else if (key == "RULE")
			{
				cin >> value;
				// TODO
			}
			else if (key == "FOLDER")
			{
				string t;
				cin >> t;
			}
		}
		else if (command == "ABOUT")
		{
			cout << "name = \"Wine\", version =\"2.1\", author = \"JinJieWang\", country = \"China\"" << endl;
		}
		else if (command == "END")
		{
			isEnded = true;
			break;
		} 
		else if (command == "SIMPLE_UI") {
			isSimpleUi = true;
			break;
		}
	}
	delete ai;
	if (isEnded) {
		exit(0);
	} else if (isSimpleUi) {
		simpleUI();
	}
}
void drawBoard(int board[15][15])
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			//行首输出行号
			if (j == 0)
			{
				if (i + 1 < 10)
				{
					cout << " ";
				}
				cout << i + 1;
			}
			//输出棋盘内容
			switch (board[i][j])
			{
			case 0:
				if (i == 0)
				{
					if (j == 0)
					{
						cout << "┌";
					}
					else if (j == 14)
					{
						cout << "┐";
					}
					else
					{
						cout << "┬";
					}
				}
				else if (i == 14)
				{
					if (j == 0)
					{
						cout << "└";
					}
					else if (j == 14)
					{
						cout << "┘";
					}
					else
					{
						cout << "┴";
					}
				}
				else
				{
					if (j == 0)
					{
						cout << "├";
					}
					else if (j == 14)
					{
						cout << "┤";
					}
					else
					{
						cout << "┼";
					}
				}
				break;
			case 1:
				cout << "●";
				break;
			case 2:
				cout << "○";
				break;

			}
			cout<<" ";
		}
		//行末换行
		cout << endl;
	}
	cout << "  A B C D E F G H I J K L M N O" << endl;

}
void simpleUI()
{
	bool exit = false;
	int color = 1;
	int board[15][15] = { 0 };
	AIWine* ai = new AIWine();
	ai->setSize(15);
	do
	{
		cout << "请输入下棋坐标(如h8):" << endl;
		string inputStr("");
		cin >> inputStr;
		int x, y;
		if (inputStr.length() == 2)
		{
			x = inputStr[0] - 'a';
			y = inputStr[1] - '1';
		}
		else if (inputStr.length() == 3)
		{
			x = inputStr[0] - 'a';
			y = (inputStr[1] - '0') * 10 + inputStr[2] - '1';
		}
		ai->turnMove(x, y);
		board[y][x] = color;
		color = 3 - color;
		drawBoard(board);
		// ai 思考下一步
		int nextX, nextY;
		ai->turnBest(nextX, nextY);
		board[nextY][nextX] = color;
		color = 3- color;
		drawBoard(board);
	} while (!exit);
	delete ai;
}
int main()
{
	gomocup();
	return 0;
}
