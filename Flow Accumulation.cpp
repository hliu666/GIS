// flow_accumulation.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926

#define RainTime 3600 //����ʱ��Ϊ1Сʱ

#define TotalTime 10800 //�ܹ��̳���ʱ��Ϊ3Сʱ

#define Interval 1 //ʱ����Ϊ5��

typedef struct//�ļ�ͷ�ṹ��
{
	int ncols; //����

	int nrows; //����

	double xllcorner; //���½ǵ�ĺ�����

	double yllcorner; //���½ǵ��������

	int cellsize; //դ���С���ֱ��ʣ�

	int NODATA_value; //�����ݵ�ֵ
}head;

typedef struct//դ���ṹ��
{
	int order;//���߳����������

	int x,y;//դ��������

	int* last;//��ǰդ��������դ�����
}grid;


//�����ļ������ļ����õ��������ݵĶ�ά������ļ�ͷ�ṹ��
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

//����ά����p_list�е������ļ�ͷh�е���Ϣд���ļ�
FILE * write_DEM(double** data,head h,char* file_name)
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

double** get_slope(double** dem,head h)//���ݵ�����DEM���ݼ����¶Ⱥ�����
{
	double **slope = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		slope[i] = (double *)malloc(h.ncols * sizeof(double));//���ٶ�ά�������¶�

	double b,c;
	double z1,z2,z3,z4,z6,z7,z8,z9;

	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(dem[i][j] == h.NODATA_value)

				slope[i][j] = h.NODATA_value;//��ֵ���������¶Ȼ�����

			//���Դ������Ϊ���ĵ�3��3�����У���ȡt1-t9��ֵ����t1-t9û��ֵ���ڴ��������⣬��������դ��ֵ������м���
			else if(dem[i][j] != h.NODATA_value)
			{
				if(i - 1 < 0 || j - 1 < 0 || dem[i - 1][j - 1] == h.NODATA_value)
					z1 = dem[i][j];
				else
					z1 = dem[i - 1][j - 1];
				if(i - 1 < 0 || dem[i - 1][j] == h.NODATA_value)
					z2 = dem[i][j];
				else
					z2 = dem[i - 1][j];
				if(i - 1 < 0 || j + 1 >= h.ncols || dem[i - 1][j + 1] == h.NODATA_value)
					z3 = dem[i][j];
				else
					z3 = dem[i - 1][j + 1];
				if(j - 1 < 0 || dem[i][j - 1] == h.NODATA_value)
					z4 = dem[i][j];
				else
					z4 = dem[i][j - 1];
				if(j + 1 >= h.ncols || dem[i][j + 1] == h.NODATA_value)
					z6 = dem[i][j];
				else
					z6 = dem[i][j + 1];
				if(i + 1 >= h.nrows || j - 1 < 0 || dem[i + 1][j - 1] == h.NODATA_value)
					z7 = dem[i][j];
				else
					z7 = dem[i + 1][j - 1];
				if(i + 1 >= h.nrows || dem[i + 1][j] == h.NODATA_value)
					z8 = dem[i][j];
				else
					z8 = dem[i + 1][j];
				if(i + 1 >= h.nrows || j + 1 >= h.ncols || dem[i + 1][j + 1] == h.NODATA_value)
					z9 = dem[i][j];
				else
					z9 = dem[i + 1][j + 1];
				
				b = (z3 + 2 * z6 + z9 - z1 - 2 * z4 -z7) / (float)(8 * h.cellsize);//x�����ϵ��¶�

				c = (z1 + 2 * z2 + z3 - z7 - 2 * z8 - z9) / (float)(8 * h.cellsize);//y�����ϵ��¶�

				slope[i][j] = atan(sqrt(pow(b,2) + pow(c,2))) / PI * 180;//�����¶ȣ���λΪ�ȣ�
			}
		}

	return slope;
}

int** get_flowdir(double** dem,head h)//��D8�㷨����DEM���ݼ�������
{
	double **fill = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		fill[i] = (double *)malloc(h.ncols * sizeof(double));//���ٶ�ά���������ݺ������
	
	int **flowdir = (int **)malloc(sizeof(int *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		flowdir[i] = (int *)malloc(h.ncols * sizeof(int));//���ٶ�ά��������������

	//�ȶ�ԭʼDEM���ݽ�������
	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(dem[i][j] == h.NODATA_value)

				fill[i][j] = h.NODATA_value;//��ֵ������������

			else if(dem[i][j] != h.NODATA_value)
			{
				double min = 9999;

				//�����Դ������Ϊ���ĵ�3��3����
				for(int m = -1;m < 2;m++)
					for(int n = -1;n < 2;n++)
					{
						if(i + m < 0 || i + m >= h.nrows || j + n < 0 || j + n >= h.ncols)//�����ڴ���Χ֮�⣬��ȥ
							continue;

						else if(m == 0 && n == 0)//���ĵ㱾������
							continue;

						else if(dem[i + m][j + n] != h.NODATA_value && dem[i + m][j + n] < min)//������դ�����ҳ���Сֵ

							min = dem[i + m][j + n];
					}
				if(min > dem[i][j])//������դ���е���Сֵ��������դ��ֵ������Сֵ��������դ��

					fill[i][j] = min;

				else if(min <= dem[i][j])//������դ��ֵ��������դ���е���Сֵ���򱣳�ԭֵ����

					fill[i][j] = dem[i][j];
			}
		}
	//��D8�㷨�����ݺ������������
	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(fill[i][j] == h.NODATA_value)

				flowdir[i][j] = h.NODATA_value;//��ֵ������������

			//�����Դ������Ϊ���ĵ�3��3���ڣ��ҳ������ĵ��������¶�ƽ��ĵ�
			else if(fill[i][j] != h.NODATA_value)
			{
				int x = 0; int y = 0;//��¼������¶ȶ�Ӧ�����ڵ��λ��

				double max = -9999;

				int x1 = 0; int y1 = 0;//��¼��С���¶ȶ�Ӧ�����ڵ��λ��

				double min = 9999;

				int s = 0;//������������¼�������Ƿ��г�������Χ����������ж����ĵ��Ƿ�λ�ڱ߽�

				for(int m = -1;m < 2;m++)
					for(int n = -1;n < 2;n++)
					{
						if(m == 0 && n == 0)//���ĵ㱾������
							continue;

						else if(i + m < 0 || i + m >= h.nrows || j + n < 0 || j + n >= h.ncols)//����դ�񳬳�����Χ
						{
							s++; //���м�¼
							continue; //����
						}
						else if(fill[i + m][j + n] == h.NODATA_value)//����դ����ֵ
						{
							s++; //���м�¼
							continue; //����
						}
						else
						{
							double grad = (fill[i][j] - fill[i + m][j + n]) / sqrt(pow((float)m,2) + pow((float)n,2));//�������ĵ������ڵ���¶�

							if(grad > max)//ȡ�������¶�
							{
								max = grad;
								x = m;
								y = n;//��¼�´����ڵ��λ��
							}
							if(grad < min)//ȡ��С�ĸ��¶�
							{
								min = grad;
								x1 = m;
								y1 = n;//��¼�´����ڵ��λ��
							}
						}
					}
				if(s > 0 && max == 0)//�����ĵ�λ�ڱ߽��ϣ��ҵ����������ڵ㣬��ȡ��С���¶ȵķ�����Ϊ������
				{
					x = -x1;
					y = -y1;
				}
				//��������¶ȶ�Ӧ��դ��λ�ã��ж���8�������е���һ��������ֵ
				if(x == 0 && y == 1)
					flowdir[i][j] = 1;

				if(x == 1 && y == 1)
					flowdir[i][j] = 2;

				if(x == 1 && y == 0)
					flowdir[i][j] = 4;

				if(x == 1 && y == -1)
					flowdir[i][j] = 8;

				if(x == 0 && y == -1)
					flowdir[i][j] = 16;

				if(x == -1 && y == -1)
					flowdir[i][j] = 32;

				if(x == -1 && y == 0)
					flowdir[i][j] = 64;

				if(x == -1 && y == 1)
					flowdir[i][j] = 128;
			}
		}
	return flowdir;
}

int** sort(double** dem,head h,int &num)//��DEM�����еĸ�դ���ո̴߳Ӹߵ��ͽ�������
{
	int **result = (int **)malloc(sizeof(int *) * h.nrows);

	for(int i = 0; i < h.nrows; i++)

		result[i] = (int *)malloc(h.ncols * sizeof(int));//������

	double max = -9999;//�����и߳����ֵ

	int x,y;

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(dem[i][j] != h.NODATA_value)
			{
				result[i][j] = -1;//��ֵդ�����ų�ʼ��

				num++;//��¼��ֵ���ݵ�����

				if(dem[i][j] > max)
				{
					max = dem[i][j];

					x = i;

					y = j;//��¼�¸߳����ֵ�������ڵ�λ��
				}
			}
			else if(dem[i][j] == h.NODATA_value)

				result[i][j] = h.NODATA_value;//��ֵ������Ϊԭֵ
		}
	int order = 0;//�������

	result[x][y] = order;//�߳����ֵ��Ӧ�����Ϊ0

	order++;//�������1

	while(order < num)//��������ֵդ���������
	{
		double difference = 9999;//��դ��ֵ֮��Ĳ�ֵ
		
		for(int i = 0;i < h.nrows;i++)
			for(int j = 0;j < h.ncols;j++)//��������դ��
			{
				if(result[i][j] >= 0)//����ǰդ���������������

					continue;

				if(max - dem[i][j] >= 0 && max - dem[i][j] < difference)//ѡ��С��������С�߳��Ҵ��������̵߳�դ����Ϊ��һ����դ��
				{
					difference = max - dem[i][j];//������С�߳��뵱ǰդ��ֵ�Ĳ�ֵ��С

					x = i;

					y = j;//��¼��һ����դ���λ��
				}
			}
		max = dem[x][y];//��¼�µ�ǰ�������е���С�߳�

		result[x][y] = order;//����������ż�¼����Ӧ��դ��

		order++;//�������1
	}
	FILE * fp1 = fopen("..\\temp1.txt","w");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	for(int i = 0;i < h.nrows;i++)
	{
		for(int j = 0;j < h.ncols;j++)
		{
			fprintf(fp1,"%d,",result[i][j]);
		}
		fprintf(fp1,"\n");
	}
	return result;
}

grid* get_upstream(int** sort,int** flowdir,int* &upstream_num,int num,head h,double** dem)//�ڸ߳�������������ݵĻ�����ȷ��ÿһդ�������
{
	grid* upstream = (grid*)malloc(num * sizeof(grid));//�������
	
	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(flowdir[i][j] != h.NODATA_value)
			{
				upstream[sort[i][j]].x = i;  upstream[sort[i][j]].y = j;

				upstream[sort[i][j]].order = sort[i][j];

				upstream[sort[i][j]].last = NULL;//���ݳ�ʼ��

				int s = 0;

				//�������������ж�ÿ��դ��������Щդ�������
				if(j + 1 < h.ncols && flowdir[i][j + 1] == 16)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i][j + 1];
					s++;
				}

				if(i + 1 < h.nrows && j + 1 < h.ncols && flowdir[i + 1][j + 1] == 32)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i + 1][j + 1];
					s++;
				}

				if(i + 1 < h.nrows && flowdir[i + 1][j] == 64)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i + 1][j];
					s++;
				}

				if(i + 1 < h.nrows && j - 1 >= 0 && flowdir[i + 1][j - 1] == 128)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i + 1][j - 1];
					s++;
				}

				if(j - 1 >= 0 && flowdir[i][j - 1] == 1)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i][j - 1];
					s++;
				}

				if(i - 1 >= 0 && j - 1 >= 0 && flowdir[i - 1][j - 1] == 2)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i - 1][j - 1];
					s++;
				}

				if(i - 1 >= 0 && flowdir[i - 1][j] == 4)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i - 1][j];
					s++;
				}

				if(i - 1 >= 0 && j + 1 < h.ncols && flowdir[i - 1][j + 1] == 8)
				{
					upstream[sort[i][j]].last = (int*)realloc(upstream[sort[i][j]].last,(s + 1) * sizeof(int));
					upstream[sort[i][j]].last[s] = sort[i - 1][j + 1];
					s++;
				}
				upstream_num[sort[i][j]] = s;
			}
		}
	FILE * fp1 = fopen("..\\temp2.txt","w");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	for(int i = 0;i < num;i++)
	{
		fprintf(fp1,"%d   %lf    ",upstream[i].order,dem[upstream[i].x][upstream[i].y]);
		fprintf(fp1,"%d,%d:",upstream[i].x,upstream[i].y);
		for(int j = 0;j < upstream_num[i];j++)
		{
			if(upstream[i].last == NULL)
				continue;
			
			fprintf(fp1,"%d,",upstream[i].last[j]);
		}
		fprintf(fp1,"\n");
	}
	
	return upstream;
}

double* flow_acc(grid* upstream,double** slope,int* upstream_num,int num,head h)//�����˶�����������
{
	int t = 0;//��ʼʱ��

	double* accumulation = (double*)malloc(num * sizeof(double));//��ǰʱ����ڸ��������

	double* last_time = (double*)malloc(num * sizeof(double));//��һʱ����ڸ��������
	
	for(int i = 0;i < num;i++)

		accumulation[i] = last_time[i] = 0;//��ʼֵΪ0

	double* result = (double*)malloc(TotalTime / Interval * sizeof(double));//��¼��ʱ�����������ڵ�����

	for(int i = 0;i < TotalTime / Interval;i++)

		result[i] = 0;

	int time = 0;

	while(t <= TotalTime)
	{
		double q1,q2;//������ʱ�̵Ľ���������λΪ����ÿ��

		if(t == 0)
		{
			q1 = 0;

			q2 = h.cellsize * 20.0 / 3600.0 / 1000.0;
		}
		else if(t <= RainTime)
		{
			q1 = h.cellsize * 20.0 / 3600.0 / 1000.0;

			q2 = h.cellsize * 20.0 / 3600.0 / 1000.0;
		}
		else if(t - Interval == RainTime)
		{
			q1 = h.cellsize * 20.0 / 3600.0 / 1000.0;

			q2 = 0;
		}
		else
		{
			q1 = 0;

			q2 = 0;
		}
		for(int i = 0;i < num;i++)
		{
			double last_stream = 0;

			if(upstream[i].last)
			{
				for(int n = 0;n < upstream_num[i];n++)

					last_stream += accumulation[upstream[i].last[n]];//�������������ܺ�
			}
		    //�������
			double a = pow((0.005 * pow(h.cellsize,2/(float)3)) / pow(slope[upstream[i].x][upstream[i].y],0.5),0.6);

			double b = 0.6;

			//�����ǰʱ�̸��������
			/*if(last_time[i] + last_stream == 0)

				accumulation[i] = (Interval / (float)h.cellsize * last_stream + Interval * (q1 + q2) / 2.0) / (Interval / (float)h.cellsize);*/

			if(t==0)
				accumulation[i] = ( ( 0*300 + (0.02/3600)*300 )/2 ) / ( 1/(double)300 );

			else

				accumulation[i] = (Interval / (float)h.cellsize * last_stream + a * b * last_time[i] * (1.0 / pow((last_time[i] + last_stream) / 2.0,1 - b)) + Interval * (q1 + q2) / 2.0) / (Interval / (float)h.cellsize + a * b * (1.0 / pow((last_time[i] + last_stream) / 2.0,1 - b)));
				//accumulation[i] = ( last_stream/300 + a * b * last_time[i] * (1/pow( (last_stream + last_time[i])/2 ,1-b )) + ( ( q1 + q2 )/2 ) ) / ( 1/(double)300 + a * b * (1/pow( (last_stream + last_time[i])/2,1-b) ) );
		}

		//if(time == 1)
		//{
		//	FILE * fp = fopen("..\\temp3.txt","w");

		//	if (fp==NULL)
		//	{
		//		puts("���ܴ��ļ���");

		//		return 0;
		//	}
		//	for(int i = 0;i < num;i++)
		//	{
		//		fprintf(fp,"%d  %lf\n",i,accumulation[i]);
		//	}

		//	FILE * fp2 = fopen("..\\time.txt","w");

		//	if (fp==NULL)
		//	{
		//		puts("���ܴ��ļ���");

		//		return 0;
		//	}
		//	double **temp = (double **)malloc(sizeof(double *) * h.nrows);
		//	for(int i = 0; i < h.nrows; i++)
		//		temp[i] = (double *)malloc(h.ncols * sizeof(double));
		//	for(int i = 0; i < h.nrows; i++)
		//		for(int j = 0; j < h.ncols; j++)
		//			temp[i][j] = h.NODATA_value;
		//	for(int i = 0;i < num;i++)
		//	{
		//		temp[upstream[i].x][upstream[i].y] = accumulation[i];
		//	}
		//	//���ļ�ͷ�ṹ��д�뵽�ļ���
		//	fprintf(fp2,"%s","ncols         ");
		//	fprintf(fp2,"%d\n",h.ncols);

		//	fprintf(fp2,"%s","nrows         ");
		//	fprintf(fp2,"%d\n",h.nrows);

		//	fprintf(fp2,"%s","xllcorner     ");
		//	fprintf(fp2,"%lf\n",h.xllcorner);

		//	fprintf(fp2,"%s","yllcorner     ");
		//	fprintf(fp2,"%lf\n",h.yllcorner);

		//	fprintf(fp2,"%s","cellsize      ");
		//	fprintf(fp2,"%d\n",h.cellsize);

		//	fprintf(fp2,"%s","NODATA_value  ");
		//	fprintf(fp2,"%d\n",h.NODATA_value);
		//	for(int i = 0;i < h.nrows;i++)//������д���ļ�
		//	{
		//		int j;

		//		for(j = 0;j < h.ncols - 1;j++)
		//		{
		//			if(temp[i][j] == h.NODATA_value)
		//				fprintf(fp2,"%.0lf",temp[i][j]);//д��������ֵ

		//			else if(temp[i][j] != h.NODATA_value)
		//				fprintf(fp2,"%lf",temp[i][j]);//д������ֵ

		//			fprintf(fp2,"%c",' ');//�������Կո�ָ�
		//		}
		//		if(temp[i][j] == h.NODATA_value)
		//			fprintf(fp2,"%.0lf\n",temp[i][j]);

		//		else if(temp[i][j] != h.NODATA_value)
		//			fprintf(fp2,"%lf\n",temp[i][j]);//һ������д����ɣ���Ҫ����
		//	}
		//}

		result[time] = accumulation[num - 1];//��¼��ǰʱ��������ڵ�����

		time++;//ʱ���������һʱ��

		for(int i = 0;i < num;i++)
		{
			last_time[i] = accumulation[i];//����ǰ�����������Ϊ��һʱ����ڸ��������

		}
		t += Interval;//ʱ���������һʱ��
	}
	FILE * fp1 = fopen("..\\result.txt","w");

	if (fp1==NULL)
	{
		puts("���ܴ��ļ���");

		return 0;
	}
	for(int i = 0;i < TotalTime / Interval;i++)
	{
		fprintf(fp1,"%lf\n",result[i]);
	}

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	head h;//�ļ�ͷ

	double** dem = read_DEM("..\\dem.txt",h);//���ļ���ȡDEM����

	double** slope = get_slope(dem,h);//�����¶�

	int** flowdir = get_flowdir(dem,h);//��������

	int num = 0;//��������ֵդ�������

	int** desc = sort(dem,h,num);//���ݸ߳�ֵ���н�������

	int* upstream_num = (int*)malloc(num * sizeof(int));

	for(int i = 0;i < num;i++)

		upstream_num[i] = 0;

	grid* upstream = get_upstream(desc,flowdir,upstream_num,num,h,dem);//���ݸ߳������������ж�ÿһ�������դ��

	double* result = flow_acc(upstream,slope,upstream_num,num,h);//�����˶�����������

	getchar();

	return 0;
}

