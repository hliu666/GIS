// PSO.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdlib.h"
#include <math.h>

#define CITY_NUM 8 //��������

#define TIME 3000 //����������

typedef struct //������ṹ��
{
	int num;//����������

	int** operators;//�����Ӽ���
}ss;

typedef struct//·�����нṹ��
{
	int* list;//��������б�

	double length;//·������
}path;


double** get_distance(double coordinate[CITY_NUM][2])//��ȡ��������֮��ľ���
{
	double** distance = (double**)malloc(sizeof(double*) * CITY_NUM);//���������ų��м����

	for(int i = 0;i < CITY_NUM;i++)

		distance[i] = (double*)malloc(sizeof(double) * CITY_NUM);

	for(int i = 0;i < CITY_NUM;i++)//������������֮��ľ���

		for(int j = 0;j < CITY_NUM;j++)

			distance[i][j] = sqrt(pow(coordinate[i][0] - coordinate[j][0],2) + pow(coordinate[i][1] - coordinate[j][1],2));

	return distance;
}

double get_length(double** distance,int* list)//����·���ĳ���
{
	double length = 0;

	for(int i = 0;i < CITY_NUM - 1;i++)//���ν��������м�ľ������

		length += distance[list[i]][list[i + 1]];

	return length;
}

int is_same(int* s1,int* s2)//�ж���·�������Ƿ���ͬ
{
	int result = 1;

	for(int i = 0;i < CITY_NUM;i++)//�������У�������ͬλ����ֵ��ͬ����������ͬ
	{
		if(s1[i] != s2[i])

			result = 0;
	}
	return result;
}

ss difference(path p1,path p2)//������·������֮��
{
	ss sequence; //���������

	sequence.num = 0;

	sequence.operators = NULL;//�������ʼ��

	int* temp = (int*)malloc(sizeof(int) * CITY_NUM);//��p2���Ƶ�temp��

	for(int i = 0;i < CITY_NUM;i++)

		temp[i] = p2.list[i];

	while(is_same(p1.list,temp) == 0)//����·������ͬʱ
	{
		for(int i = 0;i < CITY_NUM;i++)
		{
			int j;

			for(j = 0;j < CITY_NUM;j++)//����ÿһ��temp[i]����p1�ҳ�������λ��j
			{
				if(p1.list[j] == temp[i])

					break;
			}
			if(i != j)//���λ�ò�ͬ
			{
				int t = temp[i];//���н���

				temp[i] = temp[j];

				temp[j] = t;

				sequence.operators = (int**)realloc(sequence.operators,sizeof(int*) * (sequence.num + 1));

				sequence.operators[sequence.num] = (int*)malloc(sizeof(int) * 2);

				sequence.operators[sequence.num][0] = i;//�����ν������뽻������

				sequence.operators[sequence.num][1] = j;

				sequence.num++;//��������������1
			}
		}
	}
	return sequence;
}

ss multiple(ss sequence,double c)
{
	ss new_sequence;

	new_sequence.num = int(sequence.num * c + 0.5);//�����µĽ������н����ӵ�����

	new_sequence.operators = (int**)malloc(sizeof(int*) * new_sequence.num);

	for(int i = 0;i < new_sequence.num;i++)

		new_sequence.operators[i] = (int*)malloc(sizeof(int) * 2);

	for(int i = 0;i < new_sequence.num;i++)//��ԭ�������ǰn�и��Ƶ��µĽ�������
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

	for(int i = 0;i < CITY_NUM;i++)//��ԭ��·�����Ƶ���·����

		new_p.list[i] = p.list[i];

	new_p.length = 0;

	for(int i = 0;i < s.num;i++)//����ȡ���������ӣ�����·���е�Ԫ�ؽ��н���
	{
		int t = new_p.list[s.operators[i][0]];

		new_p.list[s.operators[i][0]] = new_p.list[s.operators[i][1]];

		new_p.list[s.operators[i][1]] = t;
	}
	new_p.length = get_length(distance,new_p.list);//���¼���·������

	return new_p;
}

ss union_s(ss s1,ss s2,double** distance)//��������������кϲ�
{
	path p1,p2;//���ø���·��

	p1.list = (int*)malloc(sizeof(int) * CITY_NUM);

	p2.list = (int*)malloc(sizeof(int) * CITY_NUM);

	for(int i = 0;i < CITY_NUM;i++)

		p1.list[i] = p2.list[i] = i;//ָ��������Ϊ˳��

	ss new_sequence;
	
	p2 = add(s1,p2,distance);//���ν����������븨��·�����

	p2 = add(s2,p2,distance);

	new_sequence = difference(p1,p2);//��ԭ·����任���·�������Ϊ���󽻻���

	return new_sequence;
}

path PSO(double coordinate[CITY_NUM][2])//����PSO�㷨����������
{
	double** distance = get_distance(coordinate);

	path p1,p2,p_id,p_gd,x_id;

	p1.list = (int*)malloc(sizeof(int) * CITY_NUM);

	p2.list = (int*)malloc(sizeof(int) * CITY_NUM);//������ʼ·��

	x_id.list = (int*)malloc(sizeof(int) * CITY_NUM);//������·��

	p_id.list = (int*)malloc(sizeof(int) * CITY_NUM);//�ֲ�����·��

	p_gd.list = (int*)malloc(sizeof(int) * CITY_NUM);//ȫ������·��

	for(int i = 0;i < CITY_NUM;i++)//����ָ����·��������

		p1.list[i] = i;

	p2.list[0] = 7; p2.list[1] = 5; p2.list[2] = 2; p2.list[3] = 6; p2.list[4] = 4; p2.list[5] = 1; p2.list[6] = 3; p2.list[7] = 0;

	p1.length = get_length(distance,p1.list);//�ֱ������·���ĳ���

	p2.length = get_length(distance,p2.list);

	//���϶̵�·����Ϊ�ֲ����Ž�P_id��ȫ�����Ž�P_gd
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

	v_id.num = 0;//�ٶȳ�ʼֵV_idΪ��

	for(int i = 0;i < TIME;i++)//���е���ֱ���ﵽ������������
	{
		ss s1 = difference(p_id,x_id);

		ss s2 = difference(p_gd,x_id);//�ֱ����ֲ���ȫ�����Ž��������·���Ĳ�s1��s2

		double c1 = rand() / 32768.0;

		double c2 = rand() / 32768.0;//ϵ��ȡ0~1�������

		ss cs1 = multiple(s1,c1);

		ss cs2 = multiple(s1,c2);//�ֱ�ϵ����s1��s2���

		ss sequence = union_s(v_id,cs1,distance);

		sequence = union_s(sequence,cs2,distance);//�����������κϲ������µ��ٶ�

		x_id = add(sequence,x_id,distance);//����һλ�õĽ����µ��ٶ���ӵõ��½�

		//�������µ�·�����ţ������½��滻���Ž�
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
		v_id.operators = sequence.operators;//�����ٶ�ֵ

		v_id.num = sequence.num;

		/*for(int i = 0;i < CITY_NUM;i++)

			printf("%d->",x_id.list[i]);

		printf("    %lf\n",x_id.length);*///����м����
	}
	return p_gd;
}

int _tmain(int argc, _TCHAR* argv[])
{
	double coordinate[CITY_NUM][2] = {30, 5, 40, 10, 40, 20, 29, 25, 19, 25, 9, 19, 9, 9, 20, 5};//�����е������

	path p = PSO(coordinate);//��PSO�㷨�������·��

	printf("���·��Ϊ��");

	for(int i = 0;i < CITY_NUM;i++)

		printf("%d  ",p.list[i]);

	getchar();

	return 0;
}

