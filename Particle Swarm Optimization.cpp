// PSO.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdlib.h"
#include <math.h>

#define CITY_NUM 8 //城市数量

#define TIME 3000 //最大迭代次数

typedef struct //交换序结构体
{
	int num;//交换子数量

	int** operators;//交换子集合
}ss;

typedef struct//路径序列结构体
{
	int* list;//城市序号列表

	double length;//路径长度
}path;


double** get_distance(double coordinate[CITY_NUM][2])//获取两两城市之间的距离
{
	double** distance = (double**)malloc(sizeof(double*) * CITY_NUM);//开辟数组存放城市间距离

	for(int i = 0;i < CITY_NUM;i++)

		distance[i] = (double*)malloc(sizeof(double) * CITY_NUM);

	for(int i = 0;i < CITY_NUM;i++)//计算两两城市之间的距离

		for(int j = 0;j < CITY_NUM;j++)

			distance[i][j] = sqrt(pow(coordinate[i][0] - coordinate[j][0],2) + pow(coordinate[i][1] - coordinate[j][1],2));

	return distance;
}

double get_length(double** distance,int* list)//计算路径的长度
{
	double length = 0;

	for(int i = 0;i < CITY_NUM - 1;i++)//依次将两两城市间的距离相加

		length += distance[list[i]][list[i + 1]];

	return length;
}

int is_same(int* s1,int* s2)//判断两路径序列是否相同
{
	int result = 1;

	for(int i = 0;i < CITY_NUM;i++)//遍历序列，若有相同位置数值不同的现象，则不相同
	{
		if(s1[i] != s2[i])

			result = 0;
	}
	return result;
}

ss difference(path p1,path p2)//计算两路径序列之差
{
	ss sequence; //结果交换序

	sequence.num = 0;

	sequence.operators = NULL;//交换序初始化

	int* temp = (int*)malloc(sizeof(int) * CITY_NUM);//将p2复制到temp中

	for(int i = 0;i < CITY_NUM;i++)

		temp[i] = p2.list[i];

	while(is_same(p1.list,temp) == 0)//当两路径不相同时
	{
		for(int i = 0;i < CITY_NUM;i++)
		{
			int j;

			for(j = 0;j < CITY_NUM;j++)//对于每一个temp[i]遍历p1找出该数的位置j
			{
				if(p1.list[j] == temp[i])

					break;
			}
			if(i != j)//如果位置不同
			{
				int t = temp[i];//进行交换

				temp[i] = temp[j];

				temp[j] = t;

				sequence.operators = (int**)realloc(sequence.operators,sizeof(int*) * (sequence.num + 1));

				sequence.operators[sequence.num] = (int*)malloc(sizeof(int) * 2);

				sequence.operators[sequence.num][0] = i;//将本次交换记入交换序中

				sequence.operators[sequence.num][1] = j;

				sequence.num++;//交换子数量增加1
			}
		}
	}
	return sequence;
}

ss multiple(ss sequence,double c)
{
	ss new_sequence;

	new_sequence.num = int(sequence.num * c + 0.5);//计算新的交换序中交换子的数量

	new_sequence.operators = (int**)malloc(sizeof(int*) * new_sequence.num);

	for(int i = 0;i < new_sequence.num;i++)

		new_sequence.operators[i] = (int*)malloc(sizeof(int) * 2);

	for(int i = 0;i < new_sequence.num;i++)//将原交换序的前n行复制到新的交换序中
	{
		new_sequence.operators[i][0] = sequence.operators[i][0];

		new_sequence.operators[i][1] = sequence.operators[i][1];
	}
	return new_sequence;
}

path add(ss s,path p,double** distance)
{
	path new_p;

	new_p.list = (int*)malloc(sizeof(int) * CITY_NUM);

	for(int i = 0;i < CITY_NUM;i++)//将原有路径复制到新路径中

		new_p.list[i] = p.list[i];

	new_p.length = 0;

	for(int i = 0;i < s.num;i++)//依次取出各交换子，并对路径中的元素进行交换
	{
		int t = new_p.list[s.operators[i][0]];

		new_p.list[s.operators[i][0]] = new_p.list[s.operators[i][1]];

		new_p.list[s.operators[i][1]] = t;
	}
	new_p.length = get_length(distance,new_p.list);//重新计算路径长度

	return new_p;
}

ss union_s(ss s1,ss s2,double** distance)//将两个交换序进行合并
{
	path p1,p2;//设置辅助路径

	p1.list = (int*)malloc(sizeof(int) * CITY_NUM);

	p2.list = (int*)malloc(sizeof(int) * CITY_NUM);

	for(int i = 0;i < CITY_NUM;i++)

		p1.list[i] = p2.list[i] = i;//指定其序列为顺序

	ss new_sequence;
	
	p2 = add(s1,p2,distance);//依次将两交换序与辅助路径相加

	p2 = add(s2,p2,distance);

	new_sequence = difference(p1,p2);//用原路径与变换后的路径相减作为所求交换序

	return new_sequence;
}

path PSO(double coordinate[CITY_NUM][2])//基于PSO算法计算旅行商
{
	double** distance = get_distance(coordinate);

	path p1,p2,p_id,p_gd,x_id;

	p1.list = (int*)malloc(sizeof(int) * CITY_NUM);

	p2.list = (int*)malloc(sizeof(int) * CITY_NUM);//两条初始路径

	x_id.list = (int*)malloc(sizeof(int) * CITY_NUM);//待调整路径

	p_id.list = (int*)malloc(sizeof(int) * CITY_NUM);//局部最优路径

	p_gd.list = (int*)malloc(sizeof(int) * CITY_NUM);//全局最优路径

	for(int i = 0;i < CITY_NUM;i++)//任意指定两路径的序列

		p1.list[i] = i;

	p2.list[0] = 7; p2.list[1] = 5; p2.list[2] = 2; p2.list[3] = 6; p2.list[4] = 4; p2.list[5] = 1; p2.list[6] = 3; p2.list[7] = 0;

	p1.length = get_length(distance,p1.list);//分别计算两路径的长度

	p2.length = get_length(distance,p2.list);

	//将较短的路径作为局部最优解P_id和全局最优解P_gd
	if(p1.length > p2.length)
	{
		p_id.list = p_gd.list = p2.list;

		p_id.length = p_gd.length = p2.length;

		x_id.list = p1.list;

		x_id.length = p1.length;
	}
	else if(p1.length <= p2.length)
	{
		p_id.list = p_gd.list = p1.list;

		p_id.length = p_gd.length = p1.length;

		x_id.list = p2.list;

		x_id.length = p2.length;
	}
	ss v_id;

	v_id.operators = NULL;

	v_id.num = 0;//速度初始值V_id为空

	for(int i = 0;i < TIME;i++)//进行迭代直到达到迭代次数上限
	{
		ss s1 = difference(p_id,x_id);

		ss s2 = difference(p_gd,x_id);//分别计算局部和全局最优解与待调整路径的差s1和s2

		double c1 = rand() / 32768.0;

		double c2 = rand() / 32768.0;//系数取0~1的随机数

		ss cs1 = multiple(s1,c1);

		ss cs2 = multiple(s1,c2);//分别将系数与s1、s2相乘

		ss sequence = union_s(v_id,cs1,distance);

		sequence = union_s(sequence,cs2,distance);//将交换序依次合并计算新的速度

		x_id = add(sequence,x_id,distance);//将上一位置的解与新的速度相加得到新解

		//若发现新的路径更优，则用新解替换最优解
		if(x_id.length < p_id.length)
		{
			p_id.list = x_id.list;

			p_id.length = x_id.length;
		}
		if(x_id.length < p_gd.length)
		{
			p_gd.list = x_id.list;

			p_gd.length = x_id.length;
		}
		v_id.operators = sequence.operators;//更新速度值

		v_id.num = sequence.num;

		/*for(int i = 0;i < CITY_NUM;i++)

			printf("%d->",x_id.list[i]);

		printf("    %lf\n",x_id.length);*///输出中间过程
	}
	return p_gd;
}

int _tmain(int argc, _TCHAR* argv[])
{
	double coordinate[CITY_NUM][2] = {30, 5, 40, 10, 40, 20, 29, 25, 19, 25, 9, 19, 9, 9, 20, 5};//各城市的坐标点

	path p = PSO(coordinate);//用PSO算法计算最短路径

	printf("最短路径为：");

	for(int i = 0;i < CITY_NUM;i++)

		printf("%d  ",p.list[i]);

	getchar();

	return 0;
}

