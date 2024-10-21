// 狄克斯特拉算法.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "stdlib.h"

#define MAXV 9  
//最大顶点个数
//
#define INF 32767
//初始化的最大值
//
#define RAND  rand()/(RAND_MAX/2)
//通过修改RAND，我们可以决定一个节点与另外的节点相连的概率有多大,这里我们默认概率为1/2
//
#define WEIGHT (rand()*10/(RAND_MAX/2))+5
//对各个有关系的点进行随机的权重赋值，赋值范围在5到20之间
//

typedef char ElemType;

typedef struct Ver
{
	int no;         //顶点编号

	ElemType info;  //顶点其他信息

}VertexType;        //顶点类型


typedef struct MGr   //图的定义
{
	int edges[MAXV][MAXV];//邻接矩阵

	int n, e;             //分别为顶点数和边数

	VertexType vex[MAXV]; //存放顶点信息

}MGrph;                   //声明图的邻接矩阵类型 

void DispAllPath(MGrph g, int dist[], int path[], int S[], int v);
void Dijkstra(MGrph g, int v);

int main()
{
	int v = 0;
	MGrph g;
	g.n = MAXV;
	g.e = 0;
	for (int i = 0; i < MAXV; i++)   //进行初始化的过程
	{
		g.vex[i].no = i + 1;

		for (int j = i; j < MAXV; j++)
		{
			if (i == j)
			{
				g.edges[i][j] = INF;
				continue;
			}
			if (RAND)
			{
				g.edges[j][i] = g.edges[i][j] = WEIGHT;
				g.e++;
			}
			else
				g.edges[j][i] = g.edges[i][j] = INF;
		}
	}


	printf("请输入要查询的节点:");
	scanf(" %d",&v);

	Dijkstra(g, v);
	/*for (int i = 0; i < MAXV; i++)
	{
		for (int j = 0; j < MAXV; j++)
			printf("%d\t", g.edges[i][j]);
		printf("\n");
	}*/
    return 0;
}
void Dijkstra(MGrph g, int v)  //求v到其他顶点的最短路径
{
	int dist[MAXV];            //建立dist数组

	int path[MAXV];            //建立path数组

	int S[MAXV];               //建立S数组

	int mindis, i, j, u = 0;

	for (i = 0; i < g.n; i++)     //对所有的顶点数进行遍历
	{
		dist[i] = g.edges[v][i];  //距离（权重值）的初始化

		S[i] = 0;                 //S[]置空

		if (g.edges[v][i] != INF) //路径初始化

			path[i] = v;          //顶点v到顶点i有边时，置顶点i的前一个顶点为v

		else

			path[i] = -1;          //顶点v到顶点i没边时，置顶点i的前一个顶点为-1

	}
	S[v] = 1;                      //把源点编号v放入S中

	for (i = 0; i < g.n - 1; i++)  //循环向S中添加n-1个顶点
	{
		mindis = INF;              //mindis置最小长度初值32767

		for (j = 0; j < g.n; j++)  //选取不在S中且具有到顶点最小距离
			if (S[j] == 0 && dist[j] < mindis)
			{
				u = j;

				mindis = dist[j];
			}


		S[u] = 1;               //顶点u加入到S中

		for (j = 0; j < g.n; j++)
		//修改不在S中的顶点的距离
		
			if(S[j] == 0)
			   if (g.edges[u][j] < INF && dist[u] + g.edges[u][j] < dist[j])
				{

					dist[j] = dist[u] + g.edges[u][j];

					path[j] = u;

				}
	}
	DispAllPath(g, dist, path, S, v);	
}

void DispAllPath(MGrph g, int dist[], int path[], int S[], int v)
//输出从顶点出发所有最短的路径
{
	int i, j, k;

	int apath[MAXV], d;   //存放一条最短的路径（逆向）及其顶点的个数

	for (i = 0; i < g.n; i++)

	  if( S[i] == 1 && i != v )
	{
		  printf("从%d到%d最短路径长度是：%d\t 路径：",v,i,dist[i]);

		  d = 0; apath[d] = i; //添加路径的终点

		  k = path[i];
		  if (k == -1)         //没有路径的情况
			  printf("无路径\n");
		  else                 //存在路径使输出该路径
		  {
			  while (k!=v)
			  {
				  d++;

				  apath[d] = k;

				  k = path[k];
			  }
				  d++; apath[d] = v;//添加路径的起点

				  printf("%d",apath[d]);//先输出起点

				  for (j = d - 1; j >= 0; j--)//再输出其他顶点
					  printf("→%d",apath[j]);

				  printf("\n");
			  
		  }
	}

}