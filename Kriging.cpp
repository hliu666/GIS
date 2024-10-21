#include <stdio.h>
#include <math.h>

typedef struct point
{
	int x;
	int y;
	float z;
}point;

typedef struct gamah
{
	float h;//距离
	float value;//半方差值
	int n;//点对数
}gamah;

float inv[10][10] = { {-0.0766601562,0.0227508545,0.0128326416,0.0353393555,-0.00767898560,-0.00922393799,0.00632095337,0.00909423828, -0.00725936890,0.185180664},
{0.0227508545,-0.152099609,0.00123691559,0.0718383789,0.0450439453,0.0174560547,-0.0113296509,-0.000364065170,0.00548171997,0.0570678711},
{0.0128326416,0.00123691559,-0.0688476562,-0.00511550903,0.0407104492,-0.00721359253,0.0104980469,0.00774383545,0.00815582275,0.198974609},
{0.0353393555,0.0718383789,-0.00511550903,-0.184448242,-0.00605773926,0.0724487305,0.0228271484,-0.00460433960,-0.00228500366,0.0164337158},
{-0.00767898560,0.0450439453,0.0407104492,-0.00605773926,-0.121704102,0.0481567383,-0.00649261475,0.00500488281,0.00299453735,0.0850830078},
{-0.00922393799, 0.0174560547, -0.00721359253,0.0724487305, 0.0481567383, -0.162597656,0.0353393555, 0.0105438232, -0.00484848022,0.0269775391},
{0.00632095337, -0.0113296509, 0.0104980469,0.0228271484, -0.00649261475, 0.0353393555,-0.0830688477, 0.00651168823, 0.0193786621,0.146118164},
{0.00909423828, -0.000364065170, 0.00774383545,-0.00460433960, 0.00500488281, 0.0105438232,0.00651168823, -0.0878295898, 0.0538635254,0.147216797},
{0.00725936890, 0.00548171997, 0.00815582275,-0.00228500366, 0.00299453735, -0.00484848022,0.0193786621, 0.0538635254, -0.0900268555,0.136840820},
{0.185180664,0.0570678711,0.198974609,0.0164337158,0.0850830078,0.0269775391,0.146118164,0.147216797,0.136840820,-10.11093750} };//逆矩阵

float matrix[10][10];
float w[10];
const char* filename = "E:\kriging.txt";
const char* filename2 = "E:\krigingres.txt";
point p[9];
int count = 0;

void readfile(const char* FileName);//读文件
void writefile(const char* FileName, float a[10][10]);//写文件
float distance(point a, point b);//求距离
gamah* GamaH(point a[]);//半方差
float fgama(point a, point b);//求γ（h）的值
void Matrix(point p[]);//求矩阵
float kriging(point p);
int main()
{
	readfile(filename);
	/*for (int i = 0; i < 9; i++)
	{
		printf("%d,%d,%f\n", p[i].x, p[i].y,p[i].z);
	}*/


	/*gamah *g;
	g = GamaH(p);*///求半方差
	//for (int i = 0; i < count; i++)
	//{
	//	printf("%f,%f\n", g[i].h, g[i].value);
	//}


	//float *m[10];
	//Matrix(p);//生成矩阵
	//for (int i = 0; i < 9; i++)
	//{
	//	for (int j = 0; j < 9; j++)
	//	{
	//		printf("%f,", matrix[i][j]);
	//	}
	//	printf("\n");
	//}
	
	float value[10][10];
	
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			point p;
			p.x = i+1; p.y = j+1;
			value[i][j]=kriging(p);
			printf("%f ", value[i][j]);
		}
		printf("\n");
	}
	writefile(filename2, value);

	

}

void readfile(const char* FileName)//读文件
{
	FILE *fp = fopen(FileName, "r");
	int x, y, i;
	float z;
	i = 0;
	fscanf(fp, "x,y,z\n");
	while (!feof(fp))
	{
		fscanf(fp, "%d,%d,%f\n",&x,&y,&z);
		p[i].x = x;
		p[i].y = y;
		p[i].z = z;
		i++;
	}
	fclose(fp);
}

void writefile(const char* FileName, float a[10][10])//写文件
{
	FILE*fp = fopen(FileName, "w");
	fprintf(fp, "ncols         %d\n", 10);
	fprintf(fp, "nrows         %d\n", 10);
	//fprintf(fp, "xllcorner     %d\n", xllcorner);
	//fprintf(fp, "yllcorner     %d\n", yllcorner);
	fprintf(fp, "cellsize      %d\n", 1);
	fprintf(fp, "NODATA_value  %d\n", -9999);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			fprintf(fp, "%f ", a[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

float distance(point a, point b)//求两点间距离
{
	float dis;
	dis = sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
	return dis;
}

gamah* GamaH(point a[])
{
	int judge;
	float h,v;
	judge = 0;
	gamah g[100] = { 0,0 };//半方差数组

	for (int i = 0; i < 9; i++)//依次求两点间距离，
	{
		for (int j = i + 1; j < 9; j++)
		{
			h = distance(a[i], a[j]);
			v = pow((a[i].z - a[j].z), 2);
			judge = 0;
			for (int m = 0; m < count; m++)//判断数组中是否有相同的距离值
			{
				if (g[m].h == h)//如果有，则增加值和点对数目
				{
					g[m].value =g[m].value + v;
					g[m].n = g[m].n + 1;
					judge = 1;
				}
			}
			if (judge == 0)//如果没有就插入该距离值
			{
				g[count].h = h;
				g[count].value = v;
				g[count].n = 1;
				count++;
			}		
		}
	}
	for (int i = 0; i < count; i++)//计算每个距离的半方差值
	{
		g[i].value = g[i].value / (2 * g[i].n);
	}
	return g;
}

float fgama(point a, point b)//求γ（h）的值
{
	float c, c0,ar,h;
	c0 = 0.627144984;
	c = 13.93716993;
	ar = 7;
	h = distance(a, b);
	if (h > ar)
	{
		return (c + c0);
	}
	else if(h==0)
	{
		return 0;
	}
	else
	{
		return (c0 + c*(3 * h / 2 / ar - 0.5*pow(h / ar , 3)));
	}

}

void Matrix(point p[])//求矩阵
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			matrix[i][j] = fgama(p[i], p[j]);
		}
	}
	
}

float kriging(point a)//克里金插值
{
	float y[10];
	float z=0;
	for (int i = 0; i < 9; i++)
	{
		y[i] = fgama(p[i], a);
	}
	y[9] = 1;
	for (int i = 0; i < 10; i++)
	{
		w[i] = inv[i][0] * y[0] + inv[i][1] * y[1] + inv[i][2] * y[2] + inv[i][3] * y[3] + inv[i][4] * y[4] + inv[i][5] * y[5] + inv[i][6] * y[6] + inv[i][7] * y[7] + inv[i][8] * y[8] + inv[i][9] * y[9];
	}

	for (int i = 0; i < 9; i++)
	{
		z = z + w[i] * p[i].z;
	}
	return z;
}

