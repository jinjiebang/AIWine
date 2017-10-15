#include "AIWine.h"
#include "ChessShape.h"
#include <assert.h>
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

//×ª»»³É´óÐ´×ÖÄ¸
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

int gomocup()
{
	AIWine* ai= new AIWine();
	string command;
	char dot;
	int size;
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
				cout << "MESSAGE " << "node:" << ai->nSearched << " eval:" << ai->rootBest.value << " max_ply:" << ai->getMaxPly() << endl;
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
		else if (command == "END")
		{
			delete ai;
			exit(0);

		}
	}
	delete ai;
}
int main()
{
	gomocup();
	/*ChessShape::initShape();
	ChessShape::testShape();*/

}