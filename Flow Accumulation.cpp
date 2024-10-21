// flow_accumulation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926

#define RainTime 3600 //降雨时间为1小时

#define TotalTime 10800 //总过程持续时间为3小时

#define Interval 1 //时间间隔为5秒

typedef struct//文件头结构体
{
	int ncols; //列数

	int nrows; //行数

	double xllcorner; //左下角点的横坐标

	double yllcorner; //左下角点的纵坐标

	int cellsize; //栅格大小（分辨率）

	int NODATA_value; //无数据的值
}head;

typedef struct//栅格点结构体
{
	int order;//按高程排序后的序号

	int x,y;//栅格点的坐标

	int* last;//当前栅格点的上游栅格序号
}grid;


//根据文件名读文件，得到存有数据的二维数组和文件头结构体
double** read_DEM(char* file_name,head &h)
{
	FILE * fp1 = fopen(file_name,"r");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	char temp[100];

	//将文件头读入到结构体中
	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.ncols);
	
	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.nrows);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%lf",&h.xllcorner);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%lf",&h.yllcorner);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.cellsize);

	fscanf(fp1,"%s",temp);fscanf(fp1,"%d",&h.NODATA_value);

	double **dem = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		dem[i] = (double *)malloc(h.ncols * sizeof(double));//动态开辟二维数组

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
			fscanf(fp1,"%lf",&dem[i][j]);//读出数据到二维数组中

	return dem;
}

//将二维数组p_list中的数和文件头h中的信息写入文件
FILE * write_DEM(double** data,head h,char* file_name)
{
	FILE * fp1 = fopen(file_name,"w");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	//将文件头结构体写入到文件中
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

	for(int i = 0;i < h.nrows;i++)//将数据写入文件
	{
		int j;

		for(j = 0;j < h.ncols - 1;j++)
		{
			if(data[i][j] == h.NODATA_value)
				fprintf(fp1,"%.0lf",data[i][j]);//写入无数据值

			else if(data[i][j] != h.NODATA_value)
				fprintf(fp1,"%lf",data[i][j]);//写入数据值

			fprintf(fp1,"%c",' ');//两数间以空格分隔
		}
		if(data[i][j] == h.NODATA_value)
			fprintf(fp1,"%.0lf\n",data[i][j]);

		else if(data[i][j] != h.NODATA_value)
			fprintf(fp1,"%lf\n",data[i][j]);//一行数据写入完成，需要换行
	}
	return fp1;
}

double** get_slope(double** dem,head h)//根据地区的DEM数据计算坡度和坡向
{
	double **slope = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		slope[i] = (double *)malloc(h.ncols * sizeof(double));//开辟二维数组存放坡度

	double b,c;
	double z1,z2,z3,z4,z6,z7,z8,z9;

	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(dem[i][j] == h.NODATA_value)

				slope[i][j] = h.NODATA_value;//无值区不计算坡度或坡向

			//在以待计算点为中心的3×3窗口中，读取t1-t9的值，若t1-t9没有值或在处理区域外，则用中心栅格值代替进行计算
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
				
				b = (z3 + 2 * z6 + z9 - z1 - 2 * z4 -z7) / (float)(8 * h.cellsize);//x方向上的坡度

				c = (z1 + 2 * z2 + z3 - z7 - 2 * z8 - z9) / (float)(8 * h.cellsize);//y方向上的坡度

				slope[i][j] = atan(sqrt(pow(b,2) + pow(c,2))) / PI * 180;//计算坡度（单位为度）
			}
		}

	return slope;
}

int** get_flowdir(double** dem,head h)//用D8算法根据DEM数据计算流向
{
	double **fill = (double **)malloc(sizeof(double *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		fill[i] = (double *)malloc(h.ncols * sizeof(double));//开辟二维数组存放填洼后的数据
	
	int **flowdir = (int **)malloc(sizeof(int *) * h.nrows);
	for(int i = 0; i < h.nrows; i++)
		flowdir[i] = (int *)malloc(h.ncols * sizeof(int));//开辟二维数组存放流向数据

	//先对原始DEM数据进行填洼
	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(dem[i][j] == h.NODATA_value)

				fill[i][j] = h.NODATA_value;//无值区不进行填洼

			else if(dem[i][j] != h.NODATA_value)
			{
				double min = 9999;

				//遍历以待计算点为中心的3×3窗口
				for(int m = -1;m < 2;m++)
					for(int n = -1;n < 2;n++)
					{
						if(i + m < 0 || i + m >= h.nrows || j + n < 0 || j + n >= h.ncols)//邻域在处理范围之外，舍去
							continue;

						else if(m == 0 && n == 0)//中心点本身，跳过
							continue;

						else if(dem[i + m][j + n] != h.NODATA_value && dem[i + m][j + n] < min)//在相邻栅格中找出最小值

							min = dem[i + m][j + n];
					}
				if(min > dem[i][j])//若相邻栅格中的最小值大于中心栅格值，则将最小值赋给中心栅格

					fill[i][j] = min;

				else if(min <= dem[i][j])//若中心栅格值大于相邻栅格中的最小值，则保持原值不变

					fill[i][j] = dem[i][j];
			}
		}
	//用D8算法对填洼后的数据求流向
	for(int i = 0; i < h.nrows; i++)
		for(int j = 0; j < h.ncols; j++)
		{
			if(fill[i][j] == h.NODATA_value)

				flowdir[i][j] = h.NODATA_value;//无值区不计算流向

			//遍历以待计算点为中心的3×3窗口，找出与中心点组成最大坡度平面的点
			else if(fill[i][j] != h.NODATA_value)
			{
				int x = 0; int y = 0;//记录最大正坡度对应的相邻点的位置

				double max = -9999;

				int x1 = 0; int y1 = 0;//记录最小负坡度对应的相邻点的位置

				double min = 9999;

				int s = 0;//计数变量，记录邻域中是否有超出处理范围的情况，以判断中心点是否位于边界

				for(int m = -1;m < 2;m++)
					for(int n = -1;n < 2;n++)
					{
						if(m == 0 && n == 0)//中心点本身，跳过
							continue;

						else if(i + m < 0 || i + m >= h.nrows || j + n < 0 || j + n >= h.ncols)//相邻栅格超出处理范围
						{
							s++; //进行记录
							continue; //跳过
						}
						else if(fill[i + m][j + n] == h.NODATA_value)//相邻栅格无值
						{
							s++; //进行记录
							continue; //跳过
						}
						else
						{
							double grad = (fill[i][j] - fill[i + m][j + n]) / sqrt(pow((float)m,2) + pow((float)n,2));//计算中心点与相邻点的坡度

							if(grad > max)//取最大的正坡度
							{
								max = grad;
								x = m;
								y = n;//记录下此相邻点的位置
							}
							if(grad < min)//取最小的负坡度
							{
								min = grad;
								x1 = m;
								y1 = n;//记录下此相邻点的位置
							}
						}
					}
				if(s > 0 && max == 0)//若中心点位于边界上，且低于所有相邻点，则取最小负坡度的方向作为其流向
				{
					x = -x1;
					y = -y1;
				}
				//根据最大坡度对应的栅格位置，判断是8个方向中的哪一个，并赋值
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

int** sort(double** dem,head h,int &num)//对DEM数据中的各栅格按照高程从高到低进行排序
{
	int **result = (int **)malloc(sizeof(int *) * h.nrows);

	for(int i = 0; i < h.nrows; i++)

		result[i] = (int *)malloc(h.ncols * sizeof(int));//排序结果

	double max = -9999;//数据中高程最大值

	int x,y;

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(dem[i][j] != h.NODATA_value)
			{
				result[i][j] = -1;//有值栅格的序号初始化

				num++;//记录有值数据的数量

				if(dem[i][j] > max)
				{
					max = dem[i][j];

					x = i;

					y = j;//记录下高程最大值及其所在的位置
				}
			}
			else if(dem[i][j] == h.NODATA_value)

				result[i][j] = h.NODATA_value;//无值数据仍为原值
		}
	int order = 0;//排序序号

	result[x][y] = order;//高程最大值对应的序号为0

	order++;//序号增加1

	while(order < num)//对所有有值栅格进行排序
	{
		double difference = 9999;//两栅格值之间的差值
		
		for(int i = 0;i < h.nrows;i++)
			for(int j = 0;j < h.ncols;j++)//遍历所有栅格
			{
				if(result[i][j] >= 0)//若当前栅格已有序号则跳过

					continue;

				if(max - dem[i][j] >= 0 && max - dem[i][j] < difference)//选出小于排序最小高程且大于其他高程的栅格作为下一排序栅格
				{
					difference = max - dem[i][j];//排序最小高程与当前栅格值的差值最小

					x = i;

					y = j;//记录下一排序栅格的位置
				}
			}
		max = dem[x][y];//记录下当前排序区中的最小高程

		result[x][y] = order;//将排序后的序号记录在相应的栅格处

		order++;//序号增加1
	}
	FILE * fp1 = fopen("..\\temp1.txt","w");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

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

grid* get_upstream(int** sort,int** flowdir,int* &upstream_num,int num,head h,double** dem)//在高程排序和流向数据的基础上确定每一栅格的上游
{
	grid* upstream = (grid*)malloc(num * sizeof(grid));//结果数据
	
	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(flowdir[i][j] != h.NODATA_value)
			{
				upstream[sort[i][j]].x = i;  upstream[sort[i][j]].y = j;

				upstream[sort[i][j]].order = sort[i][j];

				upstream[sort[i][j]].last = NULL;//数据初始化

				int s = 0;

				//根据流向数据判断每个栅格是由哪些栅格汇流的
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
		puts("不能打开文件！");

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

double* flow_acc(grid* upstream,double** slope,int* upstream_num,int num,head h)//进行运动波汇流计算
{
	int t = 0;//初始时间

	double* accumulation = (double*)malloc(num * sizeof(double));//当前时间段内各点的流量

	double* last_time = (double*)malloc(num * sizeof(double));//上一时间段内各点的流量
	
	for(int i = 0;i < num;i++)

		accumulation[i] = last_time[i] = 0;//初始值为0

	double* result = (double*)malloc(TotalTime / Interval * sizeof(double));//记录各时间段内流域出口的流量

	for(int i = 0;i < TotalTime / Interval;i++)

		result[i] = 0;

	int time = 0;

	while(t <= TotalTime)
	{
		double q1,q2;//相邻两时刻的降雨量，单位为毫米每秒

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

					last_stream += accumulation[upstream[i].last[n]];//计算上游流量总和
			}
		    //计算参数
			double a = pow((0.005 * pow(h.cellsize,2/(float)3)) / pow(slope[upstream[i].x][upstream[i].y],0.5),0.6);

			double b = 0.6;

			//求出当前时刻各点的流量
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
		//		puts("不能打开文件！");

		//		return 0;
		//	}
		//	for(int i = 0;i < num;i++)
		//	{
		//		fprintf(fp,"%d  %lf\n",i,accumulation[i]);
		//	}

		//	FILE * fp2 = fopen("..\\time.txt","w");

		//	if (fp==NULL)
		//	{
		//		puts("不能打开文件！");

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
		//	//将文件头结构体写入到文件中
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
		//	for(int i = 0;i < h.nrows;i++)//将数据写入文件
		//	{
		//		int j;

		//		for(j = 0;j < h.ncols - 1;j++)
		//		{
		//			if(temp[i][j] == h.NODATA_value)
		//				fprintf(fp2,"%.0lf",temp[i][j]);//写入无数据值

		//			else if(temp[i][j] != h.NODATA_value)
		//				fprintf(fp2,"%lf",temp[i][j]);//写入数据值

		//			fprintf(fp2,"%c",' ');//两数间以空格分隔
		//		}
		//		if(temp[i][j] == h.NODATA_value)
		//			fprintf(fp2,"%.0lf\n",temp[i][j]);

		//		else if(temp[i][j] != h.NODATA_value)
		//			fprintf(fp2,"%lf\n",temp[i][j]);//一行数据写入完成，需要换行
		//	}
		//}

		result[time] = accumulation[num - 1];//记录当前时刻流域出口的流量

		time++;//时间调整到下一时刻

		for(int i = 0;i < num;i++)
		{
			last_time[i] = accumulation[i];//将当前各点的流量赋为上一时间段内各点的流量

		}
		t += Interval;//时间调整到下一时刻
	}
	FILE * fp1 = fopen("..\\result.txt","w");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

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
	head h;//文件头

	double** dem = read_DEM("..\\dem.txt",h);//读文件获取DEM数据

	double** slope = get_slope(dem,h);//计算坡度

	int** flowdir = get_flowdir(dem,h);//计算流向

	int num = 0;//数据中有值栅格的数量

	int** desc = sort(dem,h,num);//根据高程值进行降序排列

	int* upstream_num = (int*)malloc(num * sizeof(int));

	for(int i = 0;i < num;i++)

		upstream_num[i] = 0;

	grid* upstream = get_upstream(desc,flowdir,upstream_num,num,h,dem);//根据高程排序后的数据判断每一点的上游栅格

	double* result = flow_acc(upstream,slope,upstream_num,num,h);//进行运动波汇流计算

	getchar();

	return 0;
}

