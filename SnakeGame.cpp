#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_RIGHT 'd'
#define KEY_LEFT 'a'

bool GameRunning = true;
int GameSpeed = 0; 

enum class Direction {Upward, Downward, Left, Right} dir;

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const int width = 40;
const int height = 20;

int StartFrameX = 6;
int StartFrameY = 6;

void positionxy(int x, int y);
void DrawFrame();
void ShowConsoleCursor(bool showFlag);
void GameOver(int &XVal, int &Yval, int &ScoreVal, int &HScoreVal, char &HitTypeVal);
void Menu();
void GameStart();

class Snake {
public:
	int X = 7;
	int Y = 7;
	int score = 0;
	int highscore = 0;
	int FoodX{}, FoodY{};
	int TailX[1000]{};
	int TailY[1000]{};
	int TailLength = 2;
	char HitType = 'N';

	void IncrementX() { X++; }
	void DecrementX() { X--; }
	void IncrementY() { Y++; }
	void DecrementY() { Y--; }

	void Logic();
	void SetSnakePosition();
	void GenerateFood();
	void CheckSelfHit();
	void RefreshSnake();
};

void Snake::Logic() {
	int PrevPosX = TailX[0];
	int PrevPosY = TailY[0];
	TailX[0] = X;
	TailY[0] = Y;
	int PrevPosX2, PrevPosY2; // WTF

	for (int i = 1; i < TailLength; ++i) {
		PrevPosX2 = TailX[i];
		PrevPosY2 = TailY[i];
		TailX[i] = PrevPosX;
		TailY[i] = PrevPosY;
		PrevPosX = PrevPosX2;
		PrevPosY = PrevPosY2;
	}


	if (_kbhit()) {
		char key = _getch();
		if (key == KEY_UP && dir != Direction::Downward) {
			dir = Direction::Upward;
		}
		else if (key == KEY_DOWN && dir != Direction::Upward) {
			dir = Direction::Downward;
		}
		else if (key == KEY_RIGHT && dir != Direction::Left) {
			dir = Direction::Right;
		}
		else if (key == KEY_LEFT && dir != Direction::Right) {
			dir = Direction::Left;
		}
	}
}

void Snake::SetSnakePosition() {
	switch (dir) {
		case Direction::Upward:
			DecrementY();
		break;
		case Direction::Downward:
			IncrementY();
		break;
		case Direction::Left:
			DecrementX();
		break;
		case Direction::Right:
			IncrementX();
		break;
	}
}

void Snake::GenerateFood() {
	srand(static_cast<unsigned int> (time(NULL)));
	FoodX = rand() % 33 + 7;
	FoodY = rand() % 13 + 7;
}

void Snake::CheckSelfHit() {
	for (int i = 0; i < TailLength; ++i) {
		if (X == TailX[i] && Y == TailY[i]) {
			HitType = 'S';
			GameOver(X, Y, score, highscore, HitType);
			HitType = 'N';
		}

	}
}

void Snake::RefreshSnake() {
	positionxy(X,Y);
	cout << " ";
	for (int i = 0; i < TailLength; ++i) {
		positionxy(TailX[i], TailY[i]);
		cout << " ";
	}
}

int main() {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, 650, 600, TRUE);

	DrawFrame();
	Menu();


	return 0;
}

void GameStart() {
	Snake Entity;
	Entity.GenerateFood();
	dir = Direction::Right; // Default


	ifstream inFile;
	inFile.open("Highscore.txt");
	inFile >> Entity.highscore;
	inFile.close();

	while (GameRunning) {

		ShowConsoleCursor(false);
		Entity.RefreshSnake();


		Entity.Logic();
		Entity.SetSnakePosition();
		SetConsoleTextAttribute(hConsole, 0x0E);
		positionxy(Entity.X, Entity.Y);
		cout << "O";
		positionxy(Entity.X, Entity.Y);

		for (int i = 0; i < Entity.TailLength; ++i) {
			positionxy(Entity.TailX[i], Entity.TailY[i]);
			cout << "o";
		}
		SetConsoleTextAttribute(hConsole, 0x0C);
		SetConsoleTextAttribute(hConsole, 0x0C);
		positionxy(Entity.FoodX, Entity.FoodY);
		cout << "*";
		SetConsoleTextAttribute(hConsole, 0x0F);

		if (Entity.X == Entity.FoodX && Entity.Y == Entity.FoodY) {
			Entity.score++;
			Entity.GenerateFood();
			Entity.TailLength++;
		}

		positionxy(StartFrameX, StartFrameY + height + 3);
		cout << "Score : " << Entity.score;
		positionxy(StartFrameX, StartFrameY + height + 4);
		cout << "Highscore : " << Entity.highscore;

		if (Entity.X == StartFrameX || Entity.X == StartFrameX + width || Entity.Y == StartFrameY || Entity.Y == StartFrameY + height) {
			Entity.HitType = 'W';
			GameOver(Entity.X, Entity.Y, Entity.score, Entity.highscore, Entity.HitType);
			Entity.HitType = 'N';
		}
		Entity.CheckSelfHit();

		Sleep(GameSpeed);
	}

	positionxy(StartFrameX, StartFrameY + height + 6);
	system("pause");
}

void positionxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(hConsole, coord);
}

void DrawFrame() {
	for (int i = 0; i < width; ++i) {
		positionxy(StartFrameX + i, StartFrameY);
		cout << char(219);
	}

	for (int i = 0; i < height; ++i) {
		positionxy(StartFrameX + width, StartFrameY + i);
		cout << char(219);
	}

	for (int i = 0; i < width + 1; ++i) {
		positionxy(StartFrameX + i, StartFrameY + height);
		cout << char(219);
	}

	for (int i = 0; i < height; ++i) {
		positionxy(StartFrameX, StartFrameY + i);
		cout << char(219);
	}
}

void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void GameOver(int &XVal, int &YVal, int& ScoreVal, int& HScoreVal, char &HitTypeVal) {
	
	if (HitTypeVal == 'W') {
		positionxy(XVal, YVal);
		cout << char(219);
	}

	GameRunning = false;
	SetConsoleTextAttribute(hConsole, 0x0C);
	positionxy(((StartFrameX + width) / 2) - 1, ((StartFrameY + height) / 2) + 2);
	cout << "Game Over!" << endl;
	SetConsoleTextAttribute(hConsole, 0x0F);
	if (ScoreVal > HScoreVal) {
		positionxy(StartFrameX, StartFrameY + height + 5);
		cout << "New highscore : " << ScoreVal;
		ofstream outFile;
		outFile.open("Highscore.txt");
		outFile << ScoreVal;
		outFile.close();
	}
	char pause = _getch();
	system("cls");
	DrawFrame();
	Menu();

}

void Menu() {
	positionxy(((StartFrameX + width) / 2) - 8, ((StartFrameY + height) / 2) + -2);
	cout << "W, A, S, D for controls";
	positionxy(((StartFrameX + width) / 2) - 8, ((StartFrameY + height) / 2) + 6);
	cout << "Press Enter to continue";

	int position = 1;// 0,1,2
	char option{};
	bool Entered = false;
	bool Running = true;
	positionxy(((StartFrameX + width) / 2) - 4, ((StartFrameY + height) / 2) + 2);
	cout << "Set Game Speed : ";
	while (true) {
		ShowConsoleCursor(false);
		if (_kbhit()) {
			option = _getch();
			if (option == 'd' && position < 2) {
				position++;
				
			}
			else if (option == 'a' && position > 0) {
				position--;
			}
			else if (option == 13) {
				Entered = true;
				GameRunning = true;
			}
		}
		if (position == 1) {
			positionxy(((StartFrameX + width) / 2) - 6, ((StartFrameY + height) / 2) + 4);
			cout << "25%";
			SetConsoleTextAttribute(hConsole, 0xF0);
			positionxy(((StartFrameX + width) / 2) + 2, ((StartFrameY + height) / 2) + 4);
			cout << "50%";
			SetConsoleTextAttribute(hConsole, 0x0F);
			positionxy(((StartFrameX + width) / 2) + 9, ((StartFrameY + height) / 2) + 4);
			cout << "75%";	
		

		}
		else if (position == 2) {
			positionxy(((StartFrameX + width) / 2) - 6, ((StartFrameY + height) / 2) + 4);
			cout << "25%";
			positionxy(((StartFrameX + width) / 2) + 2, ((StartFrameY + height) / 2) + 4);
			cout << "50%";
			SetConsoleTextAttribute(hConsole, 0xF0);
			positionxy(((StartFrameX + width) / 2) + 9, ((StartFrameY + height) / 2) + 4);
			cout << "75%";
			SetConsoleTextAttribute(hConsole, 0x0F);
		}
		else if (position == 0) {
			SetConsoleTextAttribute(hConsole, 0xF0);
			positionxy(((StartFrameX + width) / 2) - 6, ((StartFrameY + height) / 2) + 4);
			cout << "25%";
			SetConsoleTextAttribute(hConsole, 0x0F);
			positionxy(((StartFrameX + width) / 2) + 2, ((StartFrameY + height) / 2) + 4);
			cout << "50%";
			positionxy(((StartFrameX + width) / 2) + 9, ((StartFrameY + height) / 2) + 4);
			cout << "75%";
		}
		if (Entered) {
			if (position == 0) {
				GameSpeed = 80;
			}
			else if (position == 1) {
				GameSpeed = 35;
			}
			else if (position == 2) {
				GameSpeed = 10;
			}
			system("cls");
			DrawFrame();
			GameStart();
			break;

		}
		
	
	}

}