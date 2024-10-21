// precip_interpolation.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.1416

typedef struct//�ļ�ͷ�ṹ��
{
	int ncols; //����

	int nrows; //����

	double xllcorner; //���½ǵ�ĺ�����

	double yllcorner; //���½ǵ��������

	int cellsize; //դ���С���ֱ��ʣ�

	int NODATA_value; //�����ݵ�ֵ
}head;

typedef struct//վ��ṹ��
{
	double altitude;//վ��߳�

	int col;//վ��������դ�������е��к�

	int row;//վ��������դ�������е��к�

	double** precip;//��վ��11�������ս�ˮ��
}station;


//�����ļ�����DEM���ݣ��õ��������ݵĶ�ά������ļ�ͷ�ṹ��
double** read_DEM(char* file_name,head &h)
{
	FILE * fp1 = fopen(file_name,"r");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	char temp[100];

	//���ļ�ͷ���뵽�ṹ����
	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.ncols);
	
	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.nrows);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%lf",&h.xllcorner);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%lf",&h.yllcorner);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.cellsize);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.NODATA_value);

	double **dem = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		dem[i] = (double *)malloc(h.ncols * sizeof(double));//��̬���ٶ�ά����

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
			fscanf(fp1,"%lf",&dem[i][j]);//�������ݵ���ά������

	return dem;
}

//����ֵ�������д���ļ�
FILE * write_data(double** data,head h,char* file_name)
{
	FILE * fp1 = fopen(file_name,"w");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	//���ļ�ͷ�ṹ��д�뵽�ļ���
	fprintf(fp1,"%s","ncols         ");
	fprintf(fp1,"%d\n",h.ncols);

	fprintf(fp1,"%s","nrows         ");
	fprintf(fp1,"%d\n",h.nrows);

	fprintf(fp1,"%s","xllcorner     ");
	fprintf(fp1,"%lf\n",h.xllcorner);

	fprintf(fp1,"%s","yllcorner     ");
	fprintf(fp1,"%lf\n",h.yllcorner);

	fprintf(fp1,"%s","cellsize      ");
	fprintf(fp1,"%d\n",h.cellsize);

	fprintf(fp1,"%s","NODATA_value  ");
	fprintf(fp1,"%d\n",h.NODATA_value);

	for(int i = 0;i < h.nrows;i++)//������д���ļ�
	{
		int j;

		for(j = 0;j < h.ncols - 1;j++)
		{
			if(data[i][j] == h.NODATA_value)
				fprintf(fp1,"%.0lf",data[i][j]);//д��������ֵ

			else if(data[i][j] != h.NODATA_value)
				fprintf(fp1,"%lf",data[i][j]);//д������ֵ

			fprintf(fp1,"%c",' ');//�������Կո�ָ�
		}
		if(data[i][j] == h.NODATA_value)
			fprintf(fp1,"%.0lf\n",data[i][j]);

		else if(data[i][j] != h.NODATA_value)
			fprintf(fp1,"%lf\n",data[i][j]);//һ������д����ɣ���Ҫ����
	}
	return fp1;
}

//��ȡվ������
station* read_data(char* filename,head h)
{
	station* points = (station*)malloc(9 * sizeof(station));//����վ������

	for(int i = 0;i < 9;i++)
	{
		//���ٶ�ά������11�������ս�ˮ����ÿ�д���1�꣬ÿ�д���1��
		points[i].precip = (double**)malloc(11 * sizeof(double*));

		for(int j = 0;j < 11;j++)
		{
			if(i == 2 || i == 6 || i == 10)//��Ϊ���꣬����366��
				points[i].precip[j] = (double*)malloc(366 * sizeof(double));

			else//��Ϊƽ�꣬����365��
				points[i].precip[j] = (double*)malloc(365 * sizeof(double));
		}
	}

	FILE * fp = fopen(filename,"r");

	if (fp==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	for(int i = 0;i < 9;i++)
		fscanf(fp,"%lf",&points[i].altitude);//����վ��ĸ߳�
	
	for(int i = 0;i < 9;i++)
	{
		double x = 0;
		fscanf(fp,"%lf",&x);
		points[i].col = (x - h.xllcorner) / h.cellsize;//����վ��λ���������к�
	}
	for(int i = 0;i < 9;i++)
	{
		double y = 0;
		fscanf(fp,"%lf",&y);
		points[i].row = (y - h.yllcorner) / h.cellsize;//����վ��λ���������к�
	}
	
	for(int i = 0;i < 11;i++)
	{
		if(i == 2 || i == 6 || i == 10)//��Ϊ����
		{
			for(int j = 0;j < 366;j++)
				for(int k = 0;k < 9;k++)
					fscanf(fp,"%lf",&points[k].precip[i][j]);//�������յĽ�ˮ��
		}
		else//��Ϊƽ��
		{
			for(int j = 0;j < 365;j++)
				for(int k = 0;k < 9;k++)
					fscanf(fp,"%lf",&points[k].precip[i][j]);//�������յĽ�ˮ��
		}
	}
	return points;
}

double** IDW(double** given_point,int point_num,double** dem,head h)//������Ȩ�ز�ֵ
{
	double** all_point = (double**)malloc(h.nrows * sizeof(double*));
	for(int i = 0;i < h.nrows;i++)
		all_point[i] = (double*)malloc(h.ncols * sizeof(double));//��ֵ�����е������ֵ

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
			all_point[i][j] = 0;//��ʼ��

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(dem[i][j] == h.NODATA_value)
			{
				all_point[i][j] = h.NODATA_value;
				continue;
			}
			//����������֪���жϴ���ֵ�ĵ��Ƿ�Ϊ��֪��
			int k;

			for(k = 0;k < point_num;k++)
			{
				if(i == given_point[k][1] && j == given_point[k][2])
				{
					all_point[i][j] = given_point[k][3];//���ǣ���ֱ�ӽ���֪�������ֵ��������
					break;
				}
			}
			//���ǣ��������˵�
			if(k != point_num)
				continue;

			//�����ǣ�����в�ֵ
			else if(k == point_num)
			{
				double* ID = (double*)malloc(point_num * sizeof(double));//���������ŷ�����ƽ��

				double sum = 0;//�������㷴����ƽ��֮��

				for(int n = 0;n < point_num;n++)
				{
					ID[n] = pow(1 / sqrt(pow(given_point[n][1] - i,2) + pow(given_point[n][2] - j,2)),2);//���㷴����ƽ��

					sum += ID[n];
				}
				for(int n = 0;n < point_num;n++)
				{
					double w = ID[n] / sum;//����ÿ�����Ȩ��

					all_point[i][j] = all_point[i][j] + given_point[n][3] * w;//���м�Ȩ���ó���ˮ��
				}
			}
		}
	return all_point;
}

int get_month(int year,int day)//�����������������������·�
{
	int month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};//��������

	if(year == 2 || year == 6 || year == 10)//��Ϊ���꣬2��Ϊ29��
		month[1] = 29;

	for(int i = 0;i < 12;i++)
	{
		day -= month[i];//���μ�ȥ���µ�����

		if(day <= 0)
			return i;//��������
	}
}

double** precip_interpolation(double** dem,head h,station* points,double parameter[12],int year,int day,char* filename)//���н�ˮ��ֵ
{
	double** result = (double**)malloc(h.nrows * sizeof(double*));
	for(int i = 0;i < h.nrows;i++)
		result[i] = (double*)malloc(h.ncols * sizeof(double));
	
	int point_num = 0;//��¼�н�ˮֵ��վ�������

	double** given_point = NULL;

	for(int n = 0;n < 9;n++)
		if(points[n].precip[year][day] != -9999)//��վ���н�ˮ����¼
		{
			given_point = (double**)realloc(given_point,(point_num+1) * sizeof(double*));

			given_point[point_num] = (double*)malloc(4 * sizeof(double));//�¿���һ��

			//��վ����Ϣ���Ƶ�������
			given_point[point_num][0] = points[n].altitude;

			given_point[point_num][1] = points[n].row;

			given_point[point_num][2] = points[n].col;

			given_point[point_num][3] = points[n].precip[year][day];

			point_num++;
		}
			
	double alti_min = given_point[0][0];
			
	for(int n = 0;n < point_num;n++)

		if(points[n].altitude < alti_min && given_point[n][3] != 0)
		{ 
			alti_min = given_point[n][0];//�ҳ���վ���е���С�߳���Ϊ��׼��߳�
		}
	for(int n = 0;n < point_num;n++)//�����е�Ľ�ˮ��������ͬһ��׼�߳�
	{
		given_point[n][3] = given_point[n][3] / (1 + parameter[get_month(year,day)] * (given_point[n][0] - alti_min)) * (1 - parameter[get_month(year,day)] * (given_point[n][0] - alti_min));
	}
	/*for(int n = 0;n < point_num;n++)
		printf("%lf,%lf,%lf,%lf\n",given_point[n][0],given_point[n][1],given_point[n][2],given_point[n][3]);*/
			
	result = IDW(given_point,point_num,dem,h);//����ƽ���ϵķ�����Ȩ��ֵ

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(result[i][j] == h.NODATA_value)//��դ����ֵ���򲻴���
				continue;

			//����ֵ����еĽ������ٴӻ�׼��̵߳�����ʵ�ʸ߳�
			result[i][j] = result[i][j] * (1 + parameter[get_month(year,day)] * (dem[i][j] - alti_min)) / (1 - parameter[get_month(year,day)] * (dem[i][j] - alti_min));
		}

	write_data(result,h,filename);//����ֵ���д���ļ�

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	double parameter[12] = {-0.000048,-0.000222,-0.000130,0.000246,0.000221,0.000223,0.000243,0.000286,0.000244,0.000164,-0.000444,-0.000375};//���½�ˮ����ϵ��

	head h;

	double** dem = read_DEM("..\\area.txt",h);//��DEM���ݶ����ά����

	station* points = read_data("..\\sta.txt",h);//��վ�����ݶ�������

	int year,day;

	printf("����������������\n");

	scanf("%d,%d",&year,&day);

	double** area_daily = precip_interpolation(dem,h,points,parameter,year,day,"..\\result.txt");//���ݵ��յĽ�ˮ���ݽ��в�ֵ
	
	getchar();

	return 0;
}

