// BFS.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include "Graph.h"


typedef struct //网络结点结构体
{
	double x,y;//结点坐标

	int neighbor_num;//相邻点的数目

	int* neighbor;//相邻点集
}vertex;

typedef struct node //队列结点结构体
{
	int data;//数据

	node* next;//指向下一结点的指针
}node;

typedef struct//队列结构体
{
	node* front; //队首指针

	node* rear; //队尾指针
}Queue;

vertex* get_network(char* filename,int &v_num)//从文件中获取无权网络
{
	vertex* network = NULL;//无权网络

	int** edge_v = NULL;//边与结点序号的关系表

	int e_num = 0;//边的数目

	FILE * fp = fopen(filename,"r");

	if (fp == NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	char *start = (char *)malloc(sizeof(char) * 10);//边的起始标记

	char *end = (char *)malloc(sizeof(char) * 10);//边的结束标记
	
	fscanf(fp,"%s",start);

	while(start[0] != 'E')//若文件没有结束
	{
		int v_id[2];//同一条边上的两个结点序号

		for(int i = 0;i < 2;i++)
		{
			double x = 0,y = 0;

			fscanf(fp,"%lf,%lf",&x,&y);//读出结点的坐标

			int n;

			for(n = 0;n < v_num;n++)//遍历所有已有结点
			{
				if(x == network[n].x && y == network[n].y)//若当前结点已记录

					break;//跳出
			}
			if(n == v_num)//若当前结点未记录
			{
				network = (vertex*)realloc(network,sizeof(vertex) * (v_num + 1));//新开辟一个空间

				network[v_num].x = x;

				network[v_num].y = y;

				network[v_num].neighbor = NULL;

				network[v_num].neighbor_num = 0;//结点信息初始化

				v_id[i] = v_num;//记录结点序号

				v_num++;//结点数增加1
			}
			else //若当前结点已记录

				v_id[i] = n;//记录结点序号
		}
		edge_v = (int**)realloc(edge_v,sizeof(int*) * (e_num + 1));//新创建一条边

		edge_v[e_num] = (int*)malloc(sizeof(int) * 2);

		edge_v[e_num][0] = v_id[0];//将边两端的结点序号存入边中

		edge_v[e_num][1] = v_id[1];

		e_num++;//边数增加1

		fscanf(fp,"%s",end);

		fscanf(fp,"%s",start);
	}
	//构建网络中各结点间的关系表
	for(int i = 0;i < e_num;i++)
	{
		//v1、v2分别为一条边的两端点
		int v1 = edge_v[i][0];

		int v2 = edge_v[i][1];

		network[v1].neighbor = (int*)realloc(network[v1].neighbor,sizeof(int) * (network[v1].neighbor_num + 1));

		network[v1].neighbor[network[v1].neighbor_num] = v2;//将v2放入v1的邻接结点表中

		network[v1].neighbor_num++;//邻接结点数量增加1

		network[v2].neighbor = (int*)realloc(network[v2].neighbor,sizeof(int) * (network[v2].neighbor_num + 1));

		network[v2].neighbor[network[v2].neighbor_num] = v1;//将v1放入v2的邻接结点表中

		network[v2].neighbor_num++;//邻接结点数量增加1
	}
	return network;
}

void draw_network(vertex* network,int v_num)//绘出无权网络
{
	setOrig(0,0);//定义原点

	setPenColor(12566463);//定义画笔颜色
	
	for(int i = 0;i < v_num;i++)//依次画出每条边
	{
		for(int j = 0;j < network[i].neighbor_num;j++)
		{
			moveTo(network[i].x * 2,network[i].y * 2);

			int n = network[i].neighbor[j];

			lineTo(network[n].x * 2,network[n].y * 2);
		}
	}
	for(int i = 0;i < v_num;i++)//依次标记各结点的序号
	{
		char id[10];

		sprintf( id, "%d", i);

		drawText(network[i].x * 2 , network[i].y * 2 , id);
	}
}

int InitQueue(Queue &Q)//创建新队列
{
	Q.front = Q.rear = (node*)malloc(sizeof(node));
	
	Q.front->next = NULL;

	return 1;
}

int EnQueue(Queue &Q,int e)//将元素e入队
{
	node* p = (node*)malloc(sizeof(node));
	
	p->data = e;

	p->next = NULL;

	Q.rear->next = p;

	Q.rear = p;

	return 1;
}

int DeQueue(Queue &Q)//出队并返回出队元素的值
{
	if(Q.front == Q.rear)

		return -9999;

	int e;

	node* p = Q.front->next;

	e = p->data;

	Q.front->next = p->next;

	p->next = NULL;

	if(p == Q.rear)

		Q.rear = Q.front;

	free(p);

	return e;
}

double** get_path(vertex* network,int v_num,int source,int destination)//根据起点和终点得出最短路径
{
	int* visited = (int*)malloc(sizeof(int) * v_num);//记录结点是否被使用过的数组

	for(int i = 0;i < v_num;i++)//初始化

		visited[i] = 0;

	int* pre = (int*)malloc(sizeof(int) * v_num);//记录上一结点的数组

	for(int i = 0;i < v_num;i++)

		pre[i] = -1;//初始化

	Queue Q;

	InitQueue(Q);//创建一个队列

	EnQueue(Q,source);//将起点加入队列

	visited[source] = 1;//将起点标记为：已访问

	while(Q.front != Q.rear)//当队列不为空时
	{
		int e = DeQueue(Q);//将队首元素取出

		for(int i = 0;i < network[e].neighbor_num;i++)//依次处理其相邻结点
		{
			if(visited[network[e].neighbor[i]] == 0)//若相邻结点未被访问过
			{
				EnQueue(Q,network[e].neighbor[i]);//将该结点加入队列

				visited[network[e].neighbor[i]] = 1;//将该结点标记为：已访问

				pre[network[e].neighbor[i]] = e;//记录上一结点
			}
		}
	}
	int num = 0;//最短路径中结点个数

	double** path = NULL;//最短路径

	int v = destination;

	while(v != source)//逆推最短路径
	{
		path = (double**)realloc(path,sizeof(double*) * (num + 1));

		path[num] = (double*)malloc(sizeof(double) * 3);

		path[num][0] = v;

		path[num][1] = network[v].x;

		path[num][2] = network[v].y;//依次记录每个点的序号和坐标

		v = pre[v];//转至上一结点

		num++;//结点个数增加1
	}
	path = (double**)realloc(path,sizeof(double*) * (num + 1));

	path[num] = (double*)malloc(sizeof(double) * 2);

	path[num][0] = v;

	path[num][1] = network[v].x;

	path[num][2] = network[v].y;//将起点加入路径

	num++;

	//以另一种颜色绘出最短路径
	setOrig(0,0);

	setPenColor(12848119);

	moveTo(path[0][1] * 2,path[0][2] * 2);

	for(int i = 1;i < num;i++)
	{
		lineTo(path[i][1] * 2,path[i][2] * 2);
	}

	return path;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//网络中的结点数
	int v_num = 0;

	vertex* network = get_network("..\\network.gen",v_num);//从文件中读取无权网络

	draw_network(network,v_num);//图形化该网络

	int source,destination;

	printf("请输入起点和终点：\n");

	scanf("%d,%d",&source,&destination);//指定起点和终点

	double** path = get_path(network,v_num,source,destination);//得出最短路径并图形化显示

	getchar();
	getchar();
	return 0;
}

