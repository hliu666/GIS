// precip_interpolation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.1416

typedef struct//文件头结构体
{
	int ncols; //列数

	int nrows; //行数

	double xllcorner; //左下角点的横坐标

	double yllcorner; //左下角点的纵坐标

	int cellsize; //栅格大小（分辨率）

	int NODATA_value; //无数据的值
}head;

typedef struct//站点结构体
{
	double altitude;//站点高程

	int col;//站点在区域栅格数据中的列号

	int row;//站点在区域栅格数据中的行号

	double** precip;//该站点11年间的逐日降水量
}station;


//根据文件名读DEM数据，得到存有数据的二维数组和文件头结构体
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

//将插值结果数据写入文件
FILE * write_data(double** data,head h,char* file_name)
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

//读取站点数据
station* read_data(char* filename,head h)
{
	station* points = (station*)malloc(9 * sizeof(station));//开辟站点数组

	for(int i = 0;i < 9;i++)
	{
		//开辟二维数组存放11年间的逐日降水量，每行代表1年，每列代表1日
		points[i].precip = (double**)malloc(11 * sizeof(double*));

		for(int j = 0;j < 11;j++)
		{
			if(i == 2 || i == 6 || i == 10)//若为闰年，则有366日
				points[i].precip[j] = (double*)malloc(366 * sizeof(double));

			else//若为平年，则有365日
				points[i].precip[j] = (double*)malloc(365 * sizeof(double));
		}
	}

	FILE * fp = fopen(filename,"r");

	if (fp==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	for(int i = 0;i < 9;i++)
		fscanf(fp,"%lf",&points[i].altitude);//读出站点的高程
	
	for(int i = 0;i < 9;i++)
	{
		double x = 0;
		fscanf(fp,"%lf",&x);
		points[i].col = (x - h.xllcorner) / h.cellsize;//计算站点位置所处的列号
	}
	for(int i = 0;i < 9;i++)
	{
		double y = 0;
		fscanf(fp,"%lf",&y);
		points[i].row = (y - h.yllcorner) / h.cellsize;//计算站点位置所处的行号
	}
	
	for(int i = 0;i < 11;i++)
	{
		if(i == 2 || i == 6 || i == 10)//若为闰年
		{
			for(int j = 0;j < 366;j++)
				for(int k = 0;k < 9;k++)
					fscanf(fp,"%lf",&points[k].precip[i][j]);//读出各日的降水量
		}
		else//若为平年
		{
			for(int j = 0;j < 365;j++)
				for(int k = 0;k < 9;k++)
					fscanf(fp,"%lf",&points[k].precip[i][j]);//读出各日的降水量
		}
	}
	return points;
}

double** IDW(double** given_point,int point_num,double** dem,head h)//反距离权重插值
{
	double** all_point = (double**)malloc(h.nrows * sizeof(double*));
	for(int i = 0;i < h.nrows;i++)
		all_point[i] = (double*)malloc(h.ncols * sizeof(double));//插值后所有点的属性值

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
			all_point[i][j] = 0;//初始化

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(dem[i][j] == h.NODATA_value)
			{
				all_point[i][j] = h.NODATA_value;
				continue;
			}
			//遍历所有已知点判断待插值的点是否为已知点
			int k;

			for(k = 0;k < point_num;k++)
			{
				if(i == given_point[k][1] && j == given_point[k][2])
				{
					all_point[i][j] = given_point[k][3];//若是，则直接将已知点的属性值放入结果中
					break;
				}
			}
			//若是，则跳过此点
			if(k != point_num)
				continue;

			//若不是，则进行插值
			else if(k == point_num)
			{
				double* ID = (double*)malloc(point_num * sizeof(double));//开辟数组存放反距离平方

				double sum = 0;//所有样点反距离平方之和

				for(int n = 0;n < point_num;n++)
				{
					ID[n] = pow(1 / sqrt(pow(given_point[n][1] - i,2) + pow(given_point[n][2] - j,2)),2);//计算反距离平方

					sum += ID[n];
				}
				for(int n = 0;n < point_num;n++)
				{
					double w = ID[n] / sum;//计算每个点的权重

					all_point[i][j] = all_point[i][j] + given_point[n][3] * w;//进行加权，得出降水量
				}
			}
		}
	return all_point;
}

int get_month(int year,int day)//根据年数和天数计算所属月份
{
	int month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};//各月天数

	if(year == 2 || year == 6 || year == 10)//若为闰年，2月为29天
		month[1] = 29;

	for(int i = 0;i < 12;i++)
	{
		day -= month[i];//依次减去各月的天数

		if(day <= 0)
			return i;//返回月数
	}
}

double** precip_interpolation(double** dem,head h,station* points,double parameter[12],int year,int day,char* filename)//进行降水插值
{
	double** result = (double**)malloc(h.nrows * sizeof(double*));
	for(int i = 0;i < h.nrows;i++)
		result[i] = (double*)malloc(h.ncols * sizeof(double));
	
	int point_num = 0;//记录有降水值的站点的数量

	double** given_point = NULL;

	for(int n = 0;n < 9;n++)
		if(points[n].precip[year][day] != -9999)//若站点有降水量记录
		{
			given_point = (double**)realloc(given_point,(point_num+1) * sizeof(double*));

			given_point[point_num] = (double*)malloc(4 * sizeof(double));//新开辟一行

			//将站点信息复制到数组中
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
			alti_min = given_point[n][0];//找出各站点中的最小高程作为基准面高程
		}
	for(int n = 0;n < point_num;n++)//将所有点的降水量调整至同一基准高程
	{
		given_point[n][3] = given_point[n][3] / (1 + parameter[get_month(year,day)] * (given_point[n][0] - alti_min)) * (1 - parameter[get_month(year,day)] * (given_point[n][0] - alti_min));
	}
	/*for(int n = 0;n < point_num;n++)
		printf("%lf,%lf,%lf,%lf\n",given_point[n][0],given_point[n][1],given_point[n][2],given_point[n][3]);*/
			
	result = IDW(given_point,point_num,dem,h);//进行平面上的反距离权插值

	for(int i = 0;i < h.nrows;i++)
		for(int j = 0;j < h.ncols;j++)
		{
			if(result[i][j] == h.NODATA_value)//若栅格无值，则不处理
				continue;

			//将插值结果中的降雨量再从基准面高程调整回实际高程
			result[i][j] = result[i][j] * (1 + parameter[get_month(year,day)] * (dem[i][j] - alti_min)) / (1 - parameter[get_month(year,day)] * (dem[i][j] - alti_min));
		}

	write_data(result,h,filename);//将插值结果写入文件

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	double parameter[12] = {-0.000048,-0.000222,-0.000130,0.000246,0.000221,0.000223,0.000243,0.000286,0.000244,0.000164,-0.000444,-0.000375};//各月降水调整系数

	head h;

	double** dem = read_DEM("..\\area.txt",h);//将DEM数据读入二维数组

	station* points = read_data("..\\sta.txt",h);//将站点数据读入数组

	int year,day;

	printf("请输入年数和天数\n");

	scanf("%d,%d",&year,&day);

	double** area_daily = precip_interpolation(dem,h,points,parameter,year,day,"..\\result.txt");//根据当日的降水数据进行插值
	
	getchar();

	return 0;
}

