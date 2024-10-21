// ConsoleApplication1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include<stdio.h>  
#include<stdlib.h>  
int routing[25];//���ڼ�¼�����еķ���·��  
int Track[5][5];//���ڼ�¼��������Щλ�ñ����ʹ�����Щλ��û�б����ʹ�  
int variety = 0;//���ڼ�¼��ͬ·����������  
bool InChess(int x, int y) {
	return x>-1 && x<5 && y>-1 && y<5;
} //�����ж����ӵ�λ�����������ϻ���������֮��  
int Trans(int x, int y) {
	return 5 * y + x + 1;
}//��������ʽ������λ��ת��Ϊ1��25����ʽ  
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
}//����ͬ�����ӷ�ʽ���з��࣬ʵ����Ҳ��һ������  
void search(int x, int y, int step) {//�ݹ麯��  
	int x1 = x, y1 = y;
	Track[x][y] = 1;//ÿ�ν���ݹ麯���ͽ���ǰ��x,yд��·��������־Ϊ��ռ  
	routing[step] = Trans(x, y);
	if (step == 24) {
		variety++;//������·��  
		for (int i = 0;i<25;i++)
			printf("%2d ", routing[i]);//�����·��  
		printf("\n");
	}
	for (int i = 0;i<8;i++) {
		x = x1;
		y = y1;//��Ϊ��next�����е�x��y�����˱仯�����ÿ��Ҫ��x,y���и�ԭ  
		if (next(x, y, i)) {
			search(x, y, step + 1);//����ҵ��˷����������߷���������һ�ֵݹ�  
			Track[x][y] = 0;
			routing[step + 1] = 0;//��ÿ�εݹ�֮��Ҫ���������鸴ԭ  
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