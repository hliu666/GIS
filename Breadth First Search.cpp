// BFS.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include "Graph.h"


typedef struct //������ṹ��
{
	double x,y;//�������

	int neighbor_num;//���ڵ����Ŀ

	int* neighbor;//���ڵ㼯
}vertex;

typedef struct node //���н��ṹ��
{
	int data;//����

	node* next;//ָ����һ����ָ��
}node;

typedef struct//���нṹ��
{
	node* front; //����ָ��

	node* rear; //��βָ��
}Queue;

vertex* get_network(char* filename,int &v_num)//���ļ��л�ȡ��Ȩ����
{
	vertex* network = NULL;//��Ȩ����

	int** edge_v = NULL;//��������ŵĹ�ϵ��

	int e_num = 0;//�ߵ���Ŀ

	FILE * fp = fopen(filename,"r");

	if (fp == NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	char *start = (char *)malloc(sizeof(char) * 10);//�ߵ���ʼ���

	char *end = (char *)malloc(sizeof(char) * 10);//�ߵĽ������
	
	fscanf(fp,"%s",start);

	while(start[0] != 'E')//���ļ�û�н���
	{
		int v_id[2];//ͬһ�����ϵ�����������

		for(int i = 0;i < 2;i++)
		{
			double x = 0,y = 0;

			fscanf(fp,"%lf,%lf",&x,&y);//������������

			int n;

			for(n = 0;n < v_num;n++)//�����������н��
			{
				if(x == network[n].x && y == network[n].y)//����ǰ����Ѽ�¼

					break;//����
			}
			if(n == v_num)//����ǰ���δ��¼
			{
				network = (vertex*)realloc(network,sizeof(vertex) * (v_num + 1));//�¿���һ���ռ�

				network[v_num].x = x;

				network[v_num].y = y;

				network[v_num].neighbor = NULL;

				network[v_num].neighbor_num = 0;//�����Ϣ��ʼ��

				v_id[i] = v_num;//��¼������

				v_num++;//���������1
			}
			else //����ǰ����Ѽ�¼

				v_id[i] = n;//��¼������
		}
		edge_v = (int**)realloc(edge_v,sizeof(int*) * (e_num + 1));//�´���һ����

		edge_v[e_num] = (int*)malloc(sizeof(int) * 2);

		edge_v[e_num][0] = v_id[0];//�������˵Ľ����Ŵ������

		edge_v[e_num][1] = v_id[1];

		e_num++;//��������1

		fscanf(fp,"%s",end);

		fscanf(fp,"%s",start);
	}
	//���������и�����Ĺ�ϵ��
	for(int i = 0;i < e_num;i++)
	{
		//v1��v2�ֱ�Ϊһ���ߵ����˵�
		int v1 = edge_v[i][0];

		int v2 = edge_v[i][1];

		network[v1].neighbor = (int*)realloc(network[v1].neighbor,sizeof(int) * (network[v1].neighbor_num + 1));

		network[v1].neighbor[network[v1].neighbor_num] = v2;//��v2����v1���ڽӽ�����

		network[v1].neighbor_num++;//�ڽӽ����������1

		network[v2].neighbor = (int*)realloc(network[v2].neighbor,sizeof(int) * (network[v2].neighbor_num + 1));

		network[v2].neighbor[network[v2].neighbor_num] = v1;//��v1����v2���ڽӽ�����

		network[v2].neighbor_num++;//�ڽӽ����������1
	}
	return network;
}

void draw_network(vertex* network,int v_num)//�����Ȩ����
{
	setOrig(0,0);//����ԭ��

	setPenColor(12566463);//���廭����ɫ
	
	for(int i = 0;i < v_num;i++)//���λ���ÿ����
	{
		for(int j = 0;j < network[i].neighbor_num;j++)
		{
			moveTo(network[i].x * 2,network[i].y * 2);

			int n = network[i].neighbor[j];

			lineTo(network[n].x * 2,network[n].y * 2);
		}
	}
	for(int i = 0;i < v_num;i++)//���α�Ǹ��������
	{
		char id[10];

		sprintf( id, "%d", i);

		drawText(network[i].x * 2 , network[i].y * 2 , id);
	}
}

int InitQueue(Queue &Q)//�����¶���
{
	Q.front = Q.rear = (node*)malloc(sizeof(node));
	
	Q.front->next = NULL;

	return 1;
}

int EnQueue(Queue &Q,int e)//��Ԫ��e���
{
	node* p = (node*)malloc(sizeof(node));
	
	p->data = e;

	p->next = NULL;

	Q.rear->next = p;

	Q.rear = p;

	return 1;
}

int DeQueue(Queue &Q)//���Ӳ����س���Ԫ�ص�ֵ
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

double** get_path(vertex* network,int v_num,int source,int destination)//���������յ�ó����·��
{
	int* visited = (int*)malloc(sizeof(int) * v_num);//��¼����Ƿ�ʹ�ù�������

	for(int i = 0;i < v_num;i++)//��ʼ��

		visited[i] = 0;

	int* pre = (int*)malloc(sizeof(int) * v_num);//��¼��һ��������

	for(int i = 0;i < v_num;i++)

		pre[i] = -1;//��ʼ��

	Queue Q;

	InitQueue(Q);//����һ������

	EnQueue(Q,source);//�����������

	visited[source] = 1;//�������Ϊ���ѷ���

	while(Q.front != Q.rear)//�����в�Ϊ��ʱ
	{
		int e = DeQueue(Q);//������Ԫ��ȡ��

		for(int i = 0;i < network[e].neighbor_num;i++)//���δ��������ڽ��
		{
			if(visited[network[e].neighbor[i]] == 0)//�����ڽ��δ�����ʹ�
			{
				EnQueue(Q,network[e].neighbor[i]);//���ý��������

				visited[network[e].neighbor[i]] = 1;//���ý����Ϊ���ѷ���

				pre[network[e].neighbor[i]] = e;//��¼��һ���
			}
		}
	}
	int num = 0;//���·���н�����

	double** path = NULL;//���·��

	int v = destination;

	while(v != source)//�������·��
	{
		path = (double**)realloc(path,sizeof(double*) * (num + 1));

		path[num] = (double*)malloc(sizeof(double) * 3);

		path[num][0] = v;

		path[num][1] = network[v].x;

		path[num][2] = network[v].y;//���μ�¼ÿ�������ź�����

		v = pre[v];//ת����һ���

		num++;//����������1
	}
	path = (double**)realloc(path,sizeof(double*) * (num + 1));

	path[num] = (double*)malloc(sizeof(double) * 2);

	path[num][0] = v;

	path[num][1] = network[v].x;

	path[num][2] = network[v].y;//��������·��

	num++;

	//����һ����ɫ������·��
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
	//�����еĽ����
	int v_num = 0;

	vertex* network = get_network("..\\network.gen",v_num);//���ļ��ж�ȡ��Ȩ����

	draw_network(network,v_num);//ͼ�λ�������

	int source,destination;

	printf("�����������յ㣺\n");

	scanf("%d,%d",&source,&destination);//ָ�������յ�

	double** path = get_path(network,v_num,source,destination);//�ó����·����ͼ�λ���ʾ

	getchar();
	getchar();
	return 0;
}

