// DP.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define threshold 0.01 //�жϵ��Ƿ�����������ֵ

typedef struct //�ṹ�壺�����
{
	double x,y;
}point;

//���ļ��е����ݶ�������Ķ�ά�����У�����¼�ߵ�������ÿ���߰����ĵ���
point** Read(char* file_name,int* &points,int &line_num)
{
	FILE * fp1 = fopen(file_name,"r");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}

	char temp1[100] = {0};

	char temp2[100] = {0};

	//�����ļ�����ߵ�������ÿ���߰����ĵ���
	while(1)
	{
		fscanf(fp1,"%s",temp2);//���������ߵı��

		if(temp2[0] != 'E')//��û�б��������һ����
		{
			line_num++;//�ߵ���������1

			int i = 0;//��ǰ���а����������

			while(1)
			{
				fscanf(fp1,"%s",temp1);//��ȡһ�����ݣ�����һ����

				if(temp1[0] != 'E')//��û�б��������һ����

					i++;//�����������1

				else//���ѱ��������е�

					break;//����ѭ��
			}
			points[line_num] = i;//�����߰����ĵ��������ߺŶ�Ӧ��λ����
		}
		else//���ѱ�����������

			break;//����ѭ��
	}
	rewind(fp1);//���ļ�ָ�������ڿ�ͷ

	//�����ߵ�������ÿ���߰����ĵ�����̬����һ����ά������
	point **lines = (point **)malloc(sizeof(point *) * line_num);

	for(int i = 0; i < line_num; i++)

		lines[i] = (point *)malloc(points[i + 1] * sizeof(point));

	//�����ļ�����������뵽��ά������
	for(int i = 0; i < line_num; i++)
	{
		fscanf(fp1,"%s",temp1);

		for(int j = 0;j < points[i + 1];j++)

			fscanf(fp1,"%lf,%lf",&lines[i][j].x,&lines[i][j].y);

		fscanf(fp1,"%s",temp1);
	}
	return lines;
}

//����p�㵽left��right��ɵ��ߵľ���
double get_distance(point p,point left,point right)
{
	double temp1 = (p.x - left.x)*(right.y - left.y) + (p.y - left.y)*(left.x - right.x);

	double temp2 = sqrt(pow((left.x - right.x),2) + pow((left.y - right.y),2));

	double distance = fabs(temp1/temp2);

	return distance;
}

//�ҳ�����line�о��һ������Զ�ĵ�ı��
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

//���ݵ��ƫ��̶Ƚ���ȡ��
int deviate(FILE* &fp,point* line,int left,int right)
{
	double d_max = 0;

	int mid_point = right; //�м�����������

	//���������ϵĵ㵽���߾�������ֵ��������������Ӧ�ĵ��
	for(int i = left + 1;i < right;i++)
	{
		double d = get_distance(line[i],line[left],line[right]);

		if(d > d_max)
		{
			d_max = d;

			mid_point = i;
		}
	}
	//��������ֵС�ڵ�����ֵ����������ߴ�������
	if(d_max <= threshold)
	{
		fprintf(fp,"%lf,%lf\n",line[right].x,line[right].y); //���Ҷ˵�д���ļ���

		return 0;
	}
	//��������ֵ������ֵ���������м������㽫�߷�Ϊ�����ּ����ж�
	else if(d_max > threshold)
	{
		deviate(fp,line,left,mid_point);//�ݹ���ú���������벿��

		deviate(fp,line,mid_point,right);//�ݹ���ú��������Ұ벿��

		return 1;
	}
}

//ѹ��������
int line_condense(point** lines,int* points,int line_num)
{
	FILE * fp1 = fopen("D:/result.gen","w");//����ļ�

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	for(int i = 0;i < line_num;i++)//���δ��������
	{
		fprintf(fp1,"%d\n",i+1);//������ߵ����

		//�����߲��պ�
		if(lines[i][0].x != lines[i][points[i+1]-1].x || lines[i][0].y != lines[i][points[i+1]-1].y)
		{
			fprintf(fp1,"%lf,%lf\n",lines[i][0].x,lines[i][0].y);//����ߵĵ�һ����

			deviate(fp1,lines[i],0,points[i+1]-1);//��������ĵ�

			fprintf(fp1,"%s\n","END");//�ߵĽ�����־
		}
		//�����߱պ�
		else if(lines[i][0].x == lines[i][points[i+1]-1].x && lines[i][0].y == lines[i][points[i+1]-1].y)
		{
			int mid = farthest_point(lines[i],points[i+1]);//������һ������Զ�ĵ�

			fprintf(fp1,"%lf,%lf\n",lines[i][0].x,lines[i][0].y);//����ߵĵ�һ����

			//���պ����߷ֳ����ν��д���
			deviate(fp1,lines[i],0,mid);

			deviate(fp1,lines[i],mid,points[i+1]-1);

			fprintf(fp1,"%s\n","END");//�ߵĽ�����־
		}
	}
	fprintf(fp1,"%s\n","END");//�ļ�������־

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

