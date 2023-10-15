#include <iostream>
#include <windows.h>
using namespace std;

enum ConsoleColor
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

enum Fortunes { E, C, P };
const int MAX_COUNT_CELL = 4;

class Line;   //класс линия
class Cell;   //класс клетка

int sizE;   //размер поля
int maxLvl;   //глубина дерева
int maxCountMove;
int MAXEVAL = -1000;
int MINEVAL = 1000;
Line** arrLine;
Cell** arrCell;

class Line
{
public:
	int fortunesLine{ E };
	Cell* one, * two;
	Line() : one(nullptr), two(nullptr) {}
	Line(Cell* one, Cell* two) : one(one), two(two) {};
	void Filling_line(int value);
	void Clear_line();
};

class Cell
{
public:
	int fortunesCell{ E }, temp{ 0 };
	Cell() {};
	void Filling_cell(int value)
	{
		if (++temp == MAX_COUNT_CELL)
			fortunesCell = value;
	}
	void Dad_line()
	{
		if (temp-- == MAX_COUNT_CELL)
			fortunesCell = E;
	}
};

void Line::Filling_line(int value)
{
	fortunesLine = value;
	if (one) one->Filling_cell(value);
	if (two) two->Filling_cell(value);
}
void Line::Clear_line()
{
	fortunesLine = E;
	if (one) one->Dad_line();
	if (two) two->Dad_line();
}


void Init_Field()
{
	arrCell = new Cell * [sizE];
	for (size_t i = 0; i < sizE; i++)
		arrCell[i] = new Cell[sizE];
	int tempSize = sizE * 2 + 1;
	arrLine = new Line * [tempSize];
	for (size_t i = 0; i < tempSize; i++)
	{
		int tempSizej = sizE + i % 2;
		arrLine[i] = new Line[tempSizej];
		for (size_t j = 0; j < tempSizej; j++)
		{
			Cell* one, * two;
			int ti1 = i / 2 - 1 + i % 2;
			int tj1 = j - i % 2;
			int ti2 = i / 2, tj2 = j;
			if (0 <= ti1 && ti1 < sizE && 0 <= tj1 && tj1 < sizE)
				one = &arrCell[ti1][tj1];
			else one = nullptr;
			if (0 <= ti2 && ti2 < sizE && 0 <= tj2 && tj2 < sizE)
				two = &arrCell[ti2][tj2];
			else two = nullptr;
			arrLine[i][j] = Line(one, two);
		}
	}
}


char Get_char(int value, int i, int j)
{
	char c;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((DarkGray << 0) | White));
	switch (value)
	{
	case E:
	{
		if (i % 2 && j % 2) c = ' ';
		else if (!(i % 2) && !(j % 2)) c = 'o';
		else c = (i % 2) ? '|' : '-';
		break;
	}
	case C:
	{
		SetConsoleTextAttribute(hConsole, (WORD)((DarkGray << 0) | Red));
		if (!(i % 2) && j % 2) c = '-';
		else if (i % 2 && j % 2) c = '#';
		else c = '|';
		break;
	}
	case P:
	{
		SetConsoleTextAttribute(hConsole, (WORD)((DarkGray << 0) | Blue));
		if (!(i % 2) && j % 2) c = '-';
		else if (i % 2 && j % 2) c = '#';
		else c = '|';
		break;
	}
	}
	return c;
}
void Print_Field()
{
	int tempSize = sizE * 2 + 1;
	int** buffer = new int* [tempSize];
	for (size_t i = 0; i < tempSize; i++)
		buffer[i] = new int[tempSize] { E };

	for (size_t i = 0, ti = 1; i < sizE; i++, ti += 2)
		for (size_t j = 0, tj = 1; j < sizE; j++, tj += 2)
			buffer[ti][tj] = arrCell[i][j].fortunesCell;

	for (size_t i = 0; i < tempSize; i++)
		for (size_t j = 0, tj = 0; j < sizE + i % 2; j++, tj += 2)
			buffer[i][tj + 1 - i % 2] = arrLine[i][j].fortunesLine;

	cout << "  ";
	for (size_t j = 0; j < tempSize / 2 + 1; j++)
		cout << j % 10 << '-' << j % 10 << ' ';
	cout << endl;
	for (size_t i = 0; i < tempSize; i++)
	{
		cout << i % 10 << ' ';
		for (size_t j = 0; j < tempSize; j++)
			cout << Get_char(buffer[i][j], i, j) << ' ';
		cout << endl;
	}
}

int Evaluation_function()   //оценочная функция
{
	int appraisal = 0;
	for (size_t i = 0; i < sizE; ++i)
		for (size_t j = 0; j < sizE; ++j)
			if (arrCell[i][j].fortunesCell == C) ++appraisal;
			else if (arrCell[i][j].fortunesCell == P) appraisal -= 2;
	return appraisal;
}

int MinMax(int lvl, int alpha, int beta, int curentCountMove, bool Cmove)   //алгоритм минимакса с альфа и бета отсечениями	
{
	if (!lvl-- || curentCountMove == maxCountMove)
		return Evaluation_function();
	if (Cmove)
	{
		int maxEval = -1000;
		int tx = 0, ty = 0;
		for (size_t i = 0; i < sizE * 2 + 1; ++i)
		{
			for (size_t j = 0; j < sizE + i % 2; ++j)
			{
				if (!arrLine[i][j].fortunesLine)
				{
					arrLine[i][j].Filling_line(C);
					int eval = MinMax(lvl, alpha, beta, curentCountMove + 1, false);
					if (eval >= maxEval) maxEval = eval, tx = j, ty = i;
					arrLine[i][j].Clear_line();
					alpha = alpha > eval ? alpha : eval;
				}
				if (beta <= alpha) break;
			}
		}
		if (maxLvl == lvl + 1) arrLine[ty][tx].Filling_line(C);
		return maxEval;
	}
	else
	{
		int minEval = 1000;
		for (size_t i = 0; i < sizE * 2 + 1; ++i)
		{
			for (size_t j = 0; j < sizE + i % 2; ++j)
			{
				if (!arrLine[i][j].fortunesLine)
				{
					arrLine[i][j].Filling_line(P);
					int eval = MinMax(lvl, alpha, beta, curentCountMove + 1, true);
					if (eval <= minEval) minEval = eval;
					arrLine[i][j].Clear_line();
					beta = beta < eval ? beta : eval;
				}
				if (beta <= alpha) break;
			}
		}
		return minEval;
	}
}

void Get_comand(int value)
{
	int x, y;
	do
	{
		cout << "(x, y): ";
		while (!(std::cin >> x >> y) || (std::cin.peek() != '\n')) {
			std::cin.clear();
			while (std::cin.get() != '\n');
			std::cout << "Input error, repeat: " << std::endl;
		}
	} while (arrLine[y][x].fortunesLine);
	arrLine[y][x].Filling_line(value);
}

bool Winer()   //функция определения победителя
{
	int appraisal = 0;
	for (size_t i = 0; i < sizE; ++i)
		for (size_t j = 0; j < sizE; ++j)
			if (arrCell[i][j].fortunesCell == C) ++appraisal;
			else if (arrCell[i][j].fortunesCell == P) --appraisal;
	return (appraisal > 0);
}


int main()
{
	sizE = 4;
	maxLvl = 5;
	maxCountMove = (sizE + 1) * sizE * 2;
	int curentCountMove = 0;

	Init_Field();
	while (maxCountMove != curentCountMove)
	{
		Print_Field();
		cout << "Player: ";
		Get_comand(P);
		MinMax(maxLvl, MAXEVAL, MINEVAL, curentCountMove, true);
		++++curentCountMove;
		system("cls");
	}
	if (Winer()) cout << "Winner Computer!" << endl;
	else cout << "Winner Player!" << endl;
}