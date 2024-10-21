// �ҿ�˹�����㷨.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "stdlib.h"

#define MAXV 9  
//��󶥵����
//
#define INF 32767
//��ʼ�������ֵ
//
#define RAND  rand()/(RAND_MAX/2)
//ͨ���޸�RAND�����ǿ��Ծ���һ���ڵ�������Ľڵ������ĸ����ж��,��������Ĭ�ϸ���Ϊ1/2
//
#define WEIGHT (rand()*10/(RAND_MAX/2))+5
//�Ը����й�ϵ�ĵ���������Ȩ�ظ�ֵ����ֵ��Χ��5��20֮��
//

typedef char ElemType;

typedef struct Ver
{
	int no;         //������

	ElemType info;  //����������Ϣ

}VertexType;        //��������


typedef struct MGr   //ͼ�Ķ���
{
	int edges[MAXV][MAXV];//�ڽӾ���

	int n, e;             //�ֱ�Ϊ�������ͱ���

	VertexType vex[MAXV]; //��Ŷ�����Ϣ

}MGrph;                   //����ͼ���ڽӾ������� 

void DispAllPath(MGrph g, int dist[], int path[], int S[], int v);
void Dijkstra(MGrph g, int v);

int main()
{
	int v = 0;
	MGrph g;
	g.n = MAXV;
	g.e = 0;
	for (int i = 0; i < MAXV; i++)   //���г�ʼ���Ĺ���
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


	printf("������Ҫ��ѯ�Ľڵ�:");
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
void Dijkstra(MGrph g, int v)  //��v��������������·��
{
	int dist[MAXV];            //����dist����

	int path[MAXV];            //����path����

	int S[MAXV];               //����S����

	int mindis, i, j, u = 0;

	for (i = 0; i < g.n; i++)     //�����еĶ��������б���
	{
		dist[i] = g.edges[v][i];  //���루Ȩ��ֵ���ĳ�ʼ��

		S[i] = 0;                 //S[]�ÿ�

		if (g.edges[v][i] != INF) //·����ʼ��

			path[i] = v;          //����v������i�б�ʱ���ö���i��ǰһ������Ϊv

		else

			path[i] = -1;          //����v������iû��ʱ���ö���i��ǰһ������Ϊ-1

	}
	S[v] = 1;                      //��Դ����v����S��

	for (i = 0; i < g.n - 1; i++)  //ѭ����S�����n-1������
	{
		mindis = INF;              //mindis����С���ȳ�ֵ32767

		for (j = 0; j < g.n; j++)  //ѡȡ����S���Ҿ��е�������С����
			if (S[j] == 0 && dist[j] < mindis)
			{
				u = j;

				mindis = dist[j];
			}


		S[u] = 1;               //����u���뵽S��

		for (j = 0; j < g.n; j++)
		//�޸Ĳ���S�еĶ���ľ���
		
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
//����Ӷ������������̵�·��
{
	int i, j, k;

	int apath[MAXV], d;   //���һ����̵�·�������򣩼��䶥��ĸ���

	for (i = 0; i < g.n; i++)

	  if( S[i] == 1 && i != v )
	{
		  printf("��%d��%d���·�������ǣ�%d\t ·����",v,i,dist[i]);

		  d = 0; apath[d] = i; //���·�����յ�

		  k = path[i];
		  if (k == -1)         //û��·�������
			  printf("��·��\n");
		  else                 //����·��ʹ�����·��
		  {
			  while (k!=v)
			  {
				  d++;

				  apath[d] = k;

				  k = path[k];
			  }
				  d++; apath[d] = v;//���·�������

				  printf("%d",apath[d]);//��������

				  for (j = d - 1; j >= 0; j--)//�������������
					  printf("��%d",apath[j]);

				  printf("\n");
			  
		  }
	}

}