#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 20
#define FPS 150

typedef enum Bool { false, true } Bool;
typedef enum GameType { Run, Over } GameType;
typedef enum MoveDirection { Up , Down, Right, Left } MoveDirection;
typedef struct SnakeNode { int X; int Y; struct SnakeNode *next; MoveDirection direction; Bool isHead; } SnakeNode;  //蛇的身体节点，用链表实现

void Start();
void Updata();
void End();
void CreateMap();
void UpdataMap();
void ClearScreen();
void MoveControl();
void Move(SnakeNode *snake);
void AddSnakeNode();
void AddFood();
void ShowScore(GameType type, int x, int y);
void HideCursor();
void Collide();

int score;  //全局分数
int map[HEIGHT][WIDTH];  /*地图：0 空白 1 边界（墙） 2 食物 3 蛇 4蛇尾*/
Bool isGameOver;
SnakeNode *snakeHead = NULL;

int main()
{
	Start();
	while (true)
	{
		if (isGameOver)
			break;

		Updata();
	}
	End();
	return 0;
}

void Start()  //处理初始化内容
{
	score = 0;
	isGameOver = false;

	CreateMap();
	AddSnakeNode(snakeHead);

	AddFood();
	HideCursor();
}

void Updata()  //处理更新的内容
{ 
	MoveControl();
	Collide();
	UpdataMap();
	Sleep(FPS);
}

void End()  //处理游戏结束内容
{
	system("cls");
	ShowScore(Over, 0, 0);
	system("pause");
}

void CreateMap()  //创建地图
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
			{
				map[i][j] = 1;
			}
			else
				map[i][j] = 0;
		}
	}
}

void UpdataMap()  //更新地图
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { 0, 0 };

	for (int i = 0; i < HEIGHT; i++, pos.Y++)
	{
		for (int j = 0; j < WIDTH; j++, pos.X += 2)
		{
			SetConsoleCursorPosition(handle, pos);

			if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
			{
				map[i][j] = 1;
			}

			switch (map[i][j])
			{
			case 0:
				printf("  ");
				break;
			case 1:
				printf("█");
				break;
			case 2:
				printf("◎");
				break;
			case 3:
				printf("□");
				break;
			case 4:
				printf("□");
				break;
			}
		}
		pos.X = 0;
	}
	ShowScore(Run, 0, pos.Y);
}

void MoveControl()  //控制蛇移动
{
	if (_kbhit())
	{
		switch (getch())
		{
		case 'w':
			if (snakeHead->direction != Down)
				snakeHead->direction = Up;
			break;
		case 's':
			if (snakeHead->direction != Up)
				snakeHead->direction = Down;
			break;
		case 'a':
			if (snakeHead->direction != Right)
				snakeHead->direction = Left;
			break;
		case 'd':
			if (snakeHead->direction != Left)
				snakeHead->direction = Right;
			break;
		}
	}

	Move(snakeHead);
}

void Move(SnakeNode *snake)
{
	Bool isTail = false;
	if (map[snake->Y][snake->X] == 4)
		isTail = true;
	map[snake->Y][snake->X] = 0;
	switch (snake->direction)
	{
	case Up:
		snake->Y--;
		break;
	case Down:
		snake->Y++;
		break;
	case Left:
		snake->X--;
		break;
	case Right:
		snake->X++;
		break;
	}
	if (snake->isHead)
	{
		map[snake->Y][snake->X] += 3;
		if (map[snake->Y][snake->X] == 6)
			Collide();
	}
	else
	{
		if (!isTail)
			map[snake->Y][snake->X] = 3;
		else
			map[snake->Y][snake->X] = 4;
	}

	if (snake->next)
	{
		Move(snake->next);
		snake->next->direction = snake->direction;
	}
}

void AddSnakeNode(SnakeNode *snake)  //增加蛇长度
{
	if (snake == NULL)
	{
		snakeHead = (SnakeNode *)malloc(sizeof(SnakeNode));
		snakeHead->X = WIDTH / 2;
		snakeHead->Y = HEIGHT / 2;
		snakeHead->direction = Right;
		snakeHead->isHead = true;
		snakeHead->next = NULL;
		map[snakeHead->Y][snakeHead->X] = 3;
		return;
	}

	SnakeNode *newNode = (SnakeNode *)malloc(sizeof(SnakeNode));
	while (snake->next)
		snake = snake->next;
	switch (snake->direction)
	{
	case Up:
		newNode->X = snake->X;
		newNode->Y = snake->Y + 1;
		break;
	case Down:
		newNode->X = snake->X;
		newNode->Y = snake->Y - 1;
		break;
	case Left:
		newNode->X = snake->X + 1;
		newNode->Y = snake->Y;
		break;
	case Right:
		newNode->X = snake->X - 1;
		newNode->Y = snake->Y;
		break;
	}
	newNode->direction = snake->direction;
	newNode->isHead = false;
	newNode->next = NULL;
	map[newNode->Y][newNode->X] = 4;
	map[snake->Y][snake->X] = 3;
	snake->next = newNode;
}

void AddFood()  //随机生成食物
{
	srand((unsigned)time(NULL));
	int x, y;

	while (true)
	{
		x = rand() % WIDTH;
		y = rand() % HEIGHT;

		if (!map[y][x])
		{
			map[y][x] = 2;
			break;
		}
	}
}

void ShowScore(GameType type, int x, int y)  //打印分数
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };
	SetConsoleCursorPosition(handle, position);

	switch (type)
	{
	case Run:
		printf("游戏得分：%d\n", score);
		break;
	case Over:
		printf("游戏结束\n");
		printf("游戏得分：%d\n", score);
		break;
	}
}

void HideCursor()	//隐藏光标
{
	CONSOLE_CURSOR_INFO cursor_info = { 1,0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void Collide()  //判断碰撞类型
{
	switch (map[snakeHead->Y][snakeHead->X])
	{
	case 4:
		isGameOver = true;
		break;
	case 5:
		map[snakeHead->Y][snakeHead->X] = 3;
		AddSnakeNode(snakeHead);
		AddFood();
		score++;
		break;
	case 6:
		isGameOver = true;
		break;
	case 7:
		map[snakeHead->Y][snakeHead->X] = 3;
		break;
	}
}