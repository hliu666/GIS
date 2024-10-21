// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include<stdio.h>  
#include<stdlib.h>  
int routing[25];//用于记录棋盘中的访问路径  
int Track[5][5];//用于记录棋盘中哪些位置被访问过，哪些位置没有被访问过  
int variety = 0;//用于记录不同路径的种类数  
bool InChess(int x, int y) {
	return x>-1 && x<5 && y>-1 && y<5;
} //用于判断棋子的位置是在棋盘上还是在棋盘之外  
int Trans(int x, int y) {
	return 5 * y + x + 1;
}//将坐标形式的棋子位置转换为1到25的形式  
bool next(int &x, int &y, int flag) {
	switch (flag)
	{
	case 0:
		if (InChess(x + 1, y + 2) && !Track[x + 1][y + 2]) {
			x++;
			y += 2;
			return true;
		}
		break;
	case 1:
		if (InChess(x + 1, y - 2) && !Track[x + 1][y - 2]) {
			x++;
			y -= 2;
			return true;
		}
		break;
	case 2:
		if (InChess(x + 2, y - 1) && !Track[x + 2][y - 1]) {
			x += 2;
			y--;
			return true;
		}
		break;
	case 3:
		if (InChess(x + 2, y + 1) && !Track[x + 2][y + 1]) {
			x += 2;
			y++;
			return true;
		}
		break;
	case 4:
		if (InChess(x - 1, y + 2) && !Track[x - 1][y + 2]) {
			x--;
			y += 2;
			return true;
		}
		break;
	case 5:
		if (InChess(x - 1, y - 2) && !Track[x - 1][y - 2]) {
			x--;
			y -= 2;
			return true;
		}
		break;
	case 6:
		if (InChess(x - 2, y - 1) && !Track[x - 2][y - 1]) {
			x -= 2;
			y--;
			return true;
		}
		break;
	case 7:
		if (InChess(x - 2, y + 1) && !Track[x - 2][y + 1]) {
			x -= 2;
			y++;
			return true;
		}
		break;
	}
	return false;
}//将不同的走子方式进行分类，实际上也是一种排序  
void search(int x, int y, int step) {//递归函数  
	int x1 = x, y1 = y;
	Track[x][y] = 1;//每次进入递归函数就将当前的x,y写入路径，并标志为已占  
	routing[step] = Trans(x, y);
	if (step == 24) {
		variety++;//出现新路径  
		for (int i = 0;i<25;i++)
			printf("%2d ", routing[i]);//输出该路径  
		printf("\n");
	}
	for (int i = 0;i<8;i++) {
		x = x1;
		y = y1;//因为在next函数中的x和y发生了变化，因此每次要对x,y进行复原  
		if (next(x, y, i)) {
			search(x, y, step + 1);//如果找到了符合条件的走法，进行下一轮递归  
			Track[x][y] = 0;
			routing[step + 1] = 0;//在每次递归之后，要将两个数组复原  
		}
	}
}
int main() {
	for (int i = 0;i<5;i++)
		for (int j = 0;j<5;j++)
			Track[i][j] = 0;
	for (int i = 0;i<25;i++)
		routing[i] = 0;
	search(0, 0, 0);
	printf(" variety = %d", variety);
	return 0;
}