/*
程序名： Amazon
*/
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <iomanip>
#include <wchar.h>
#include <ctime>
#include <fstream>
#include <Windows.h>



#define black 1;
#define white 2;
#define barrier -1;


using namespace std;

const char BLACK = 219;
const char WHITE = 32;
const int BLOCK = 3;

int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };//移动方案
int chessboard[100][100] = { 0 };
int x, y;
int tempx, tempy;
int originalx, originaly;
int barrierx, barriery;
int playerno;
int menuopt;
int turnid;
int mycolor;
int turn;
int player;
int checkid;


void starting();
void printchessboard();
void moveinsert(int a);
bool checkpiece(int a, int i, int j);
bool legalmove(int originali, int originalj, int inserti, int insertj);
void printMenu(void); //打印出菜单
bool menu(void);
void coordsplayeronetwo(int color, int originali, int originalj, int i, int j);
void barrierplace(int i, int j);
void moveinsert(int a);
void testchess(int color, int originali, int originalj, int i, int j, int barrieri, int barrierj);
void Save();
void getSave();
void barrierinsert(int a);
void newgame();
void lastgame();

void printMenu(void)
{
	cout << "Commands:" << endl << "1: Continue game" << endl << "2: New game" << endl << "3: Save round" << endl << "4: Load a previous round" << endl;
}

bool menu(void) {
	printMenu();
	cin >> menuopt;
	if (menuopt == 1) { //继续游戏
		cout << "You have chosen to continue, please enter x and y" << endl;
		return false;

	}
	else if (menuopt == 2) { //重新玩新一盘
		cout << "You have chosen to restart" << endl;
		newgame();
		return true;
	}
	else if (menuopt == 3) { //存盘
		Save();
		cout << "Saved!" << endl;
		return true;
	}

	else if (menuopt == 4) { //读盘
		lastgame();
		return true;
	}

	else if (menuopt < 1 || menuopt>4) { //输入不是四个数字中的一个
		cout << "Invalid number, please reenter" << endl;
		menu();
		return false;
	}
}
bool inMap(int x, int y)// 判断是否在棋盘内
{
	if (x < 0 || x >= 8 || y < 0 || y >= 8)
		return false;
	return true;
}// 判断是否在棋盘内

void clear(int x0, int y0, int x1, int y1, int x2, int y2, int color)//清除模拟落子，回溯专用
{
	chessboard[x2][y2] = 0;//障碍释放区
	chessboard[x1][y1] = 0;//移动后位置
	chessboard[x0][y0] = color;//移动前位置
}//清除模拟落子，回溯专用，一定要注意这里的清空顺序，非常重要！！！与刚才是逆着的

void Save() //存盘
{
	char filename[100];
	cout << "Please input the file name" << endl;
	cin >> filename; //输入文件名

	ofstream fout(filename);
	fout << player << ' ';
	fout << turnid << ' ';
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			fout << chessboard[i][j] << ' ';
		}
	}
	fout.close();
}

void getSave()
{
	char a[100];
	cout << "Which file do you want to load?" << endl;
	cin >> a; //输入要开的文件名

	ifstream fin(a);//打开文件
	if (!fin)
	{
		checkid = 1; return; //如果不存在，用checkid表示，直接执行完函数
	}
	int id;
	fin >> id;
	player = id;
	fin >> turnid;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			fin >> chessboard[i][j];
		}
	}
	cout << turnid;
	fin.close();
}


void starting() {
	memset(chessboard, 0, sizeof(chessboard));
	chessboard[0][2] = white; chessboard[0][5] = white; chessboard[2][0] = white; chessboard[2][7] = white;
	chessboard[7][2] = black; chessboard[7][5] = black; chessboard[5][0] = black; chessboard[5][7] = black;
}

void printchessboard() {
	system("cls");
	cout << "y/x 1     2     3     4     5     6     7     8" << endl; //输出横坐标

	for (int i = 0; i < 8; i++) {
		cout << i + 1 << setw(4); //输出列坐标
		for (int u = 0; u < BLOCK; u++) {
			if (u != 0)cout << setw(5);
			for (int j = 0; j < 8; j++) {
				int c = (i + j) & 1;
				for (int k = 0; k < BLOCK; k++) {
					if (c == 1) { //这里和else只是用来设计棋盘
						if (chessboard[i][j] == 1 && u == 1 && k == 1)cout << "<>";
						else if (chessboard[i][j] == 2 && u == 1 && k == 1)cout << "[]";
						else if (chessboard[i][j] == -1 && u == 1 && k == 1)cout << setw(2) << "X" << setw(1);
						else if (chessboard[i][j] == 0)cout << BLACK << BLACK; //black tile
						else { cout << BLACK << BLACK; }

					}
					else {
						if (chessboard[i][j] == 1 && u == 1 && k == 1)cout << "<>";
						else if (chessboard[i][j] == 2 && u == 1 && k == 1)cout << "[]";
						else if (chessboard[i][j] == -1 && u == 1 && k == 1)cout << setw(2) << "X" << setw(1);
						else if (chessboard[i][j] == 0)cout << WHITE << WHITE; //white tile;
						else { cout << WHITE << WHITE; }
					}
				}
			}
			cout << endl;
		}
	}
	cout << "Player Black is <>, Player White is [] and Barrier is X" << endl;
	cout << "For Menu options, please enter 0 into console." << endl;

}

int value(int color) //color 是想查的棋的颜色，而现在在查棋子的价值，查的是四颗棋子有多少能移到的地方。 现在不考虑放障碍物。
{
	int mark[8][8] = { 0 };
	int total = 0;//记录总共能行棋的地方的数量 
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (chessboard[i][j] != color)continue;//查在i,j 的位置有想查的棋
			for (int k = 0; k < 8; ++k)
			{
				for (int a = 1; a < 8; a++)
				{
					int xx = i + dx[k] * a;//八个方向都走
					int yy = j + dy[k] * a;
					if (chessboard[xx][yy] != 0 || xx < 0 || xx >= 8 || yy < 0 || yy >= 8) //一遇到障碍物或不在期盼范围内就停止
						break; //一旦遇到障碍就停下来
					if (mark[xx][yy] == 0)//查是否被查过
					{
						total++; mark[xx][yy] = 1; //表示那个地方已被查
					}
				}
			}
		}
	}
	return total;
}

bool legal(int x0, int y0, int x1, int y1, int x2, int y2, int color)//检查落子是否合法
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (chessboard[x0][y0] != color || chessboard[x1][y1] != 0)
		return false;
	if ((chessboard[x2][y2] != 0) && !(x2 == x0 && y2 == y0))//这个点不为0且不是移动前的位置（那就一定是有阻挡了）
		return false;
	return true;//其他情况都可以返回真值
}//检查落子是否合法

void putchess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//落子函数
{
	chessboard[x0][y0] = 0;//移动前位置
	chessboard[x1][y1] = color;//移动后位置
	chessboard[x2][y2] = barrier;//障碍释放区
}//落子函数

void determine(int color)//AI 函数， 
{
	int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];//起始能走的点，落点，障碍点
	int posCount = 0, choice = 0;
	int maxvalue = -280000;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (chessboard[i][j] != color)continue;
			for (int k = 0; k < 8; ++k)
			{
				for (int delta1 = 1; delta1 < 8; delta1++)
				{
					int xx = i + dx[k] * delta1;//八个方向都走
					int yy = j + dy[k] * delta1;
					if (chessboard[xx][yy] != 0 || !inMap(xx, yy))
						break; //一旦遇到障碍就停下来
					for (int l = 0; l < 8; ++l)
					{
						for (int delta2 = 1; delta2 < 8; delta2++)
						{
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (chessboard[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (legal(i, j, xx, yy, xxx, yyy, color))//一旦这是合法的落子
							{
								putchess(i, j, xx, yy, xxx, yyy, color); // 模拟己方落子
								beginPos[posCount][0] = i;
								beginPos[posCount][1] = j;
								possiblePos[posCount][0] = xx;
								possiblePos[posCount][1] = yy;
								obstaclePos[posCount][0] = xxx;
								obstaclePos[posCount][1] = yyy;//ij移动点，xx yy移动到的地方 xxx yyy释放障碍
								posCount++;
								int temvalue;
								if (color == 1)temvalue = value(color) - value(2);
								else if (color == 2) temvalue = value(color) - value(1);//AI现在价值减去对方现在价值
								if (temvalue > maxvalue)
								{
									maxvalue = temvalue;
									choice = posCount - 1;
								}
								clear(i, j, xx, yy, xxx, yyy, color);//清除己方落子，回溯
							}
						}
					}
				}

			}
		}
	}
	putchess(beginPos[choice][0], beginPos[choice][1], possiblePos[choice][0],
		possiblePos[choice][1], obstaclePos[choice][0], obstaclePos[choice][1], color);//做好决策，落子
}//AI决策，参数是现在AI执子颜色

void coordsplayeronetwo(int color, int originali, int originalj, int i, int j) {
	chessboard[originali][originalj] = 0;
	chessboard[i][j] = color;
	printchessboard();
}


void barrierplace(int i, int j) {
	chessboard[i][j] = -1;
	printchessboard();
}



void moveinsert(int a) { //输入
	cout << "Please type in the coordinates of the piece you want to move in the format x y" << endl;
	cin >> tempx;
	if (tempx == 0) {
		while (1) { if (menu())return; else { cin >> tempx; break; } }//叫出菜单，如果选择 continue, 那继续输入
	}
	cin >> tempy;
	originalx = tempx - 1; originaly = tempy - 1;
	if (!checkpiece(a, originaly, originalx)) { printchessboard(); cout << "Move illegal, no valid piece" << endl; moveinsert(a); } //没有此棋
	cout << "Please type in the coordinates of your move in the format x y" << endl;
	cin >> tempx;
	if (tempx == 0) {
		while (1) {
			if (menu())return; else { cin >> tempx;  break; }//叫出菜单，如果选择 continue, 那继续输入
		}
	}
	cin >> tempy;
	x = tempx - 1; y = tempy - 1;
	if (!legalmove(originaly, originalx, y, x)) {
		printchessboard();
		cout << "Move illegal! Please reenter piece and move!" << endl; moveinsert(a);
	}
	coordsplayeronetwo(a, originaly, originalx, y, x);


}

void barrierinsert(int a) {
	cout << "Please type in the coordinates of your barrier in the format x y" << endl;
	cin >> tempx;

	if (tempx == 0) {
		while (1) { if (menu())return; else { cin >> tempx; break; } }
	}
	cin >> tempy;
	barrierx = tempx - 1; barriery = tempy - 1;
	if (!legalmove(y, x, barriery, barrierx)) {
		printchessboard();
		cout << "Move illegal! Please reenter barrier coordinates" << endl; barrierinsert(a); //不能把障碍放在这里
	}
	barrierplace(barriery, barrierx);

}


bool legalmove(int originali, int originalj, int inserti, int insertj) { //originalx 与originaly 是棋本来的坐标
	if (chessboard[inserti][insertj] != 0 || inserti < 0 || inserti >= 8 || insertj < 0 || insertj >= 8) { //这里查两个基本条件：要移到的位置是否已经有棋或有障碍物和是否在期盼范围内。
		return false;
	}
	//从这里开始，for 循环内一步一步往移动方向查，直到到达要移到的位置，一有障碍物或棋就直接返回false
	else {
		if (originali == inserti) { //查同一行
			if (insertj - originalj > 0) {
				for (int j = originalj + 1; j <= insertj; j++) {
					if (chessboard[originali][j] != 0) { return false; }
				}
				return true;
			}
			else if (insertj - originalj < 0) {
				for (int j = originalj - 1; j >= insertj; j--) { if (chessboard[originali][j] != 0) { return false; } }
				return true;
			}
		}
		else if (originalj == insertj) { //查同一列
			if (inserti - originali > 0) {
				for (int i = originali + 1; i <= inserti; i++) { if (chessboard[i][originalj] != 0) return false; }
				return true;
			}
			else if (inserti - originali < 0) {
				for (int i = originali - 1; i >= inserti; i--) { if (chessboard[i][originalj] != 0) { return false; } }
				return true;
			}

		}
		else if (abs(originali - inserti) == abs(originalj - insertj)) {
			if (inserti - originali < 0 && insertj - originalj < 0) { // 左上
				for (int z = 1; z <= abs(originali - inserti); z++) {
					if (chessboard[originali - z][originalj - z] != 0) return false;
				}
				return true;
			}
			if (inserti - originali < 0 && insertj - originalj>0) {// 右上
				for (int z = 1; z <= abs(originali - inserti); z++) {
					if (chessboard[originali - z][originalj + z] != 0) return false;
				}
				return true;
			}
			if (inserti - originali > 0 && insertj - originalj < 0) {//左下
				for (int z = 1; z <= abs(originali - inserti); z++) {
					if (chessboard[originali - z][originalj - z] != 0) return false;
				}
				return true;
			}
			if (inserti - originali > 0 && insertj - originalj > 0) {//右下
				for (int z = 1; z <= abs(originali - inserti); z++) {
					if (chessboard[originali - z][originalj + z] != 0) return false;
				}
				return true;
			}

		}
	}
	return false;
}

bool gameendplayerone() {//查游戏是否结束了，player one
	int flag = 0;
	int piece = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (chessboard[i][j] == 1) {
				if (i + 1 == 8 || chessboard[i + 1][j] != 0) flag++;
				if (i == 0 || chessboard[i - 1][j] != 0)flag++;
				if (j + 1 == 8 || chessboard[i][j + 1] != 0)flag++;
				if (j == 0 || chessboard[i][j - 1] != 0)flag++;
				if (((i + 1 == 8) && (j + 1 == 8)) || (chessboard[i + 1][j + 1] != 0)) flag++; //bottom right
				if ((i == 0 && j == 0) || chessboard[i - 1][j - 1] != 0) flag++;//top left
				if ((i == 0 && j + 1 == 8) || chessboard[i - 1][j + 1] != 0)flag++; //top right
				if ((j == 0 && i + 1 == 8) || chessboard[i + 1][j - 1] != 0) flag++; //bottom left
			}
			if (flag == 8)piece++; //8个方向都有障碍物或其它棋
			flag = 0;
		}
	}
	if (piece == 4)return true;//四个同颜色的棋都无法移动了
	else return false;
}

bool gameendplayertwo() { //查游戏是否结束了，playertwo
	int flag = 0;
	int piece = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (chessboard[i][j] == 2) {
				if (i + 1 == 8 || chessboard[i + 1][j] != 0) flag++;
				if (i == 0 || chessboard[i - 1][j] != 0)flag++;
				if (j + 1 == 8 || chessboard[i][j + 1] != 0)flag++;
				if (j == 0 || chessboard[i][j - 1] == 0)flag++;
				if (((i + 1 == 8) && (j + 1 == 8)) || (chessboard[i + 1][j + 1] != 0)) flag++; //bottom right
				if ((i == 0 && j == 0) || chessboard[i - 1][j - 1] != 0) flag++;//top left
				if ((i == 0 && j + 1 == 8) || chessboard[i - 1][j + 1] != 0)flag++; //top right
				if ((j == 0 && i + 1 == 8) || chessboard[i + 1][j - 1] != 0) flag++; //bottom left
			}
			if (flag == 8)piece++;//8个方向都有障碍物或其它棋
			flag = 0;
		}
	}
	if (piece == 4)return true; //四个同颜色的棋都无法移动了
	else return false;
}

bool checkpiece(int a, int i, int j) {
	if (chessboard[i][j] == a) {
		return true;
	}
	else return false;
}


void newgame()//开始新游戏函数
{
	starting();
	printchessboard();
	turnid = 0;
	cout << "New Game" << endl;
	cout << "Which one do you want to play?" << endl << "Black(Press B)" << endl << "White(Press W)" << endl;
	char playercolor;
	cin >> playercolor; //玩家选颜色

	while (playercolor != 'b' && playercolor != 'B' && playercolor != 'W' && playercolor != 'w')
	{
		if (playercolor == '0') {
			while (1) { if (menu())continue; else { cin >> playercolor; break; } }
		}
		cout << "Illegal input. Please try again." << endl;
		cin >> playercolor;
	}
	if (playercolor == 'b' || playercolor == 'B')
	{
		int turn = 1;
		mycolor = white;//我（AI）是白方
		player = black;
		while (gameendplayerone() == 0)//如果玩家还没死，玩家就可以做出决策
		{
			system("cls");
			cout << endl;
			printchessboard();
			cout << "Your color is black." << endl;

			turnid++;//x1不是-1次数才能加一
			moveinsert(turn);
			barrierinsert(turn);
			turn = 2;
			if (gameendplayertwo() == 0)//如果AI没死，就执行AI 函数
			{
				determine(mycolor);
				turnid++;//AI判断完毕，turn次数加一
				system("cls");
				cout << endl;
				printchessboard();
			}
			else break;
			turn = 1;
		}
		if (turn == 1)cout << "You lost...";
		else cout << "You win! Congratulations!" << endl;
		cout << "Press any key to close me!" << endl;
		fflush(stdin);
		cin.get();
		return;
	}

	else
	{
		int turn = 1;
		mycolor = black;//AI是黑方
		player = white;
		chessboard[5][0] = 0; chessboard[5][6] = black; chessboard[5][3] = -1;
		turnid++;
		turn = 2;
		while (gameendplayertwo() == 0)//如果玩家还没死，玩家移动棋
		{
			system("cls");
			cout << endl;
			printchessboard();
			cout << "Your color is white." << endl;
			turnid++;
			moveinsert(turn);
			barrierinsert(turn);
			turn = 1;

			if (gameendplayerone() == 0)//如果AI还没死就可以继续玩
			{
				determine(mycolor);
				turnid++;
				system("cls");
				printchessboard();
			}
			else break;
			turn = 2;
		}
		if (turn == 2)cout << "You lost!";
		else cout << "You win! Congratulations!" << endl;
		cout << "Goodbye! Press any key to close me!" << endl;
		fflush(stdin);
		cin.get();
		return;
	}
}//开始新游戏函数

void lastgame()//上一局函数
{
	checkid = 0;
	getSave();
	system("cls");
	if (checkid == 1)
	{
		cin.get();
		cout << "No record. Press any key to start a new game.";
		cin.get();
		newgame();
		return;
	}
	if (turnid % 2 != 0)//如果turnID是2的倍数，就证明上次游戏结束后该黑方走了，玩家自然就是黑方
	{
		int turn = 1;
		mycolor = white;//我（AI）是白方
		while (gameendplayerone() == 0)//如果玩家还没死，玩家就可以做出决策
		{
			system("cls");
			cout << endl;
			printchessboard();
			cout << "Your color is black." << endl;
			turnid++;//x1不是-1次数才能加一
			moveinsert(turn);
			barrierinsert(turn);
			turn = 2;
			if (gameendplayertwo() == 0)//如果AI也没死，就做出决策
			{
				determine(mycolor);//白子做决策
				turnid++;//AI做完决策，次数加一
				system("cls");
				cout << endl;
				printchessboard();
			}
			else break;
			turn = 1;
		}
		if (turn == 1)cout << "You lost!";
		else cout << "You win! Congratulations!";
		cout << "Press any key to close me!" << endl;
		fflush(stdin);
		cin.get();
		return;
	}
	else
	{
		int turn = 2;
		mycolor = black;//我（AI）是黑方
		while (gameendplayertwo() == 0)//如果玩家还没死，玩家做决策
		{
			system("cls");
			cout << endl;
			printchessboard();
			cout << "Your color is white." << endl;
			turnid++;
			moveinsert(turn);
			barrierinsert(turn);
			turn = 1;

			if (gameendplayerone() == 0)//如果AI还没死就可以继续决策
			{
				determine(mycolor);//黑子做决策
				turnid++;
				system("cls");
				printchessboard();
			}
			else break;
			turn = 2;
		}
		if (turn == 2)cout << "You lost!";
		else cout << "You win! Congratulations!";
		cout << "Goodbye! Press any key to close me!" << endl;
		fflush(stdin);
		cin.get();
		return;
	}
}//上一局函数

void playgame()
{
	system("cls");
	starting();
	printchessboard();
}

int main() {
	system("mode con cols=80 lines=35");//窗口宽度高度
	system("color 0F0");//背景颜色
	playgame();
	newgame();


	return 0;
}