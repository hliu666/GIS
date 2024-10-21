// DP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define threshold 0.01 //判断点是否需舍弃的阈值

typedef struct //结构体：坐标点
{
	double x,y;
}point;

//将文件中的数据读出到点的二维数组中，并记录线的总数及每条线包含的点数
point** Read(char* file_name,int* &points,int &line_num)
{
	FILE * fp1 = fopen(file_name,"r");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}

	char temp1[100] = {0};

	char temp2[100] = {0};

	//遍历文件求出线的总数及每条线包含的点数
	while(1)
	{
		fscanf(fp1,"%s",temp2);//读出各条线的编号

		if(temp2[0] != 'E')//若没有遍历到最后一条线
		{
			line_num++;//线的总数增加1

			int i = 0;//当前线中包含点的总数

			while(1)
			{
				fscanf(fp1,"%s",temp1);//读取一行数据，代表一个点

				if(temp1[0] != 'E')//若没有遍历到最后一个点

					i++;//点的总数增加1

				else//若已遍历了所有点

					break;//跳出循环
			}
			points[line_num] = i;//将此线包含的点数放入线号对应的位置中
		}
		else//若已遍历了所有线

			break;//跳出循环
	}
	rewind(fp1);//将文件指针重置于开头

	//根据线的总数及每条线包含的点数动态开辟一个二维点数组
	point **lines = (point **)malloc(sizeof(point *) * line_num);

	for(int i = 0; i < line_num; i++)

		lines[i] = (point *)malloc(points[i + 1] * sizeof(point));

	//遍历文件，将各点读入到二维数组中
	for(int i = 0; i < line_num; i++)
	{
		fscanf(fp1,"%s",temp1);

		for(int j = 0;j < points[i + 1];j++)

			fscanf(fp1,"%lf,%lf",&lines[i][j].x,&lines[i][j].y);

		fscanf(fp1,"%s",temp1);
	}
	return lines;
}

//计算p点到left和right组成的线的距离
double get_distance(point p,point left,point right)
{
	double temp1 = (p.x - left.x)*(right.y - left.y) + (p.y - left.y)*(left.x - right.x);

	double temp2 = sqrt(pow((left.x - right.x),2) + pow((left.y - right.y),2));

	double distance = fabs(temp1/temp2);

	return distance;
}

//找出点列line中距第一个点最远的点的编号
int farthest_point(point* line,int length)
{
	double d_max = 0;

	int n = 0;

	for(int i = 0;i < length;i++)
	{
		double d = sqrt(pow((line[i].x - line[0].x),2) + pow((line[i].y - line[0].y),2));

		if(d > d_max)

			n = i;
	}
	return n;
}

//根据点的偏离程度进行取舍
int deviate(FILE* &fp,point* line,int left,int right)
{
	double d_max = 0;

	int mid_point = right; //中间特征点的序号

	//计算曲线上的点到弦线距离的最大值，并求出最大距离对应的点号
	for(int i = left + 1;i < right;i++)
	{
		double d = get_distance(line[i],line[left],line[right]);

		if(d > d_max)
		{
			d_max = d;

			mid_point = i;
		}
	}
	//若最大距离值小于等于阈值，则可用弦线代替曲线
	if(d_max <= threshold)
	{
		fprintf(fp,"%lf,%lf\n",line[right].x,line[right].y); //将右端点写入文件中

		return 0;
	}
	//若最大距离值大于阈值，则需用中间特征点将线分为两部分继续判断
	else if(d_max > threshold)
	{
		deviate(fp,line,left,mid_point);//递归调用函数处理左半部分

		deviate(fp,line,mid_point,right);//递归调用函数处理右半部分

		return 1;
	}
}

//压缩线数据
int line_condense(point** lines,int* points,int line_num)
{
	FILE * fp1 = fopen("D:/result.gen","w");//结果文件

	if (fp1==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	for(int i = 0;i < line_num;i++)//依次处理各条线
	{
		fprintf(fp1,"%d\n",i+1);//先输出线的序号

		//若曲线不闭合
		if(lines[i][0].x != lines[i][points[i+1]-1].x || lines[i][0].y != lines[i][points[i+1]-1].y)
		{
			fprintf(fp1,"%lf,%lf\n",lines[i][0].x,lines[i][0].y);//输出线的第一个点

			deviate(fp1,lines[i],0,points[i+1]-1);//过滤其余的点

			fprintf(fp1,"%s\n","END");//线的结束标志
		}
		//若曲线闭合
		else if(lines[i][0].x == lines[i][points[i+1]-1].x && lines[i][0].y == lines[i][points[i+1]-1].y)
		{
			int mid = farthest_point(lines[i],points[i+1]);//求出距第一个点最远的点

			fprintf(fp1,"%lf,%lf\n",lines[i][0].x,lines[i][0].y);//输出线的第一个点

			//将闭合曲线分成两段进行处理
			deviate(fp1,lines[i],0,mid);

			deviate(fp1,lines[i],mid,points[i+1]-1);

			fprintf(fp1,"%s\n","END");//线的结束标志
		}
	}
	fprintf(fp1,"%s\n","END");//文件结束标志

	fclose(fp1);

	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char* file_name = "D:/2.CHINA_Arc.gen";

	int* points = (int*)malloc(sizeof(int) * 500);

	int line_num = 0;

	point** lines = Read(file_name,points,line_num);

	line_condense(lines,points,line_num);

	getchar();

	return 0;
}

