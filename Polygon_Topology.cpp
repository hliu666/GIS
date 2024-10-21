// topo.cpp : 定义控制台应用程序的入口点。
//
// polygon_topology.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include<math.h>

#define PI 3.1415926

typedef struct arc//弧段类型
{
	int id;//弧段序号

	double angle;//弧段方位角

	int sign;//弧段使用标记

	arc* next;//指向下一弧段的指针
}arc;

typedef struct//多边形有向边类型
{
	int arc_id;//弧段号

	int start;//起始结点序号

	int end;//终止结点序号
}edge;

typedef struct//结点类型
{
	double x,y;//结点的坐标

	arc* pn_a;//指向弧段链表的头指针
}node;

typedef struct
{
	double x,y;
}point;

point** Read(char* file_name,int* &points,int &line_num)
{
	
	FILE * fp1 = fopen(file_name,"r");

	if (fp1==NULL)
	{
		puts("不能打开文件！");

		return 0;
	}
	char temp1[100] = {0};//临时数组

	char temp2[100] = {0};//临时数组

	//遍历文件求出线的总数及每条线包含的点数
	while(1)
	{
		fscanf(fp1,"%s",temp2);//读出各条线的编号

		if(temp2[0] != 'E')//若没有遍历到最后一条线
		{
			line_num++;//线的总数增加1

			int i = 0;//当前线中包含点的总数

			while(1)
			{
				fscanf(fp1,"%s",temp1);//读取一行数据，代表一个点

				if(temp1[0] != 'E')//若没有遍历到最后一个点

					i++;//点的总数增加1

				else//若已遍历了所有点

					break;//跳出循环
			}
			points[line_num] = i;//将此线包含的点数放入线号对应的位置中
		}
		else//若已遍历了所有线

			break;//跳出循环
	}
	rewind(fp1);//将文件指针重置于开头

	//根据线的总数及每条线包含的点数动态开辟一个二维点数组
	point **lines = (point **)malloc(sizeof(point *) * line_num);

	for(int i = 0; i < line_num; i++)

		lines[i] = (point *)malloc(points[i + 1] * sizeof(point));

	//遍历文件，将各点读入到二维数组中
	for(int i = 0; i < line_num; i++)
	{
		fscanf(fp1,"%s",temp1);

		for(int j = 0;j < points[i + 1];j++)

			fscanf(fp1,"%lf,%lf",&lines[i][j].x,&lines[i][j].y);

		fscanf(fp1,"%s",temp1);
	}
	return lines;
}

//计算弧段的方位角
double get_angle(double x0,double y0,double x1,double y1)
{
	double dx = x1 - x0;  double dy = y1 - y0;

	if(dx == 0 && dy > 0)

		return PI/2;

	else if(dx == 0 && dy < 0)

		return 3*PI/2;

	else if(dx > 0 && dy >= 0)

		return atan(fabs(dy/dx));

	else if(dx < 0 && dy >= 0)

		return PI - atan(fabs(dy/dx));

	else if(dx < 0 && dy <= 0)

		return PI + atan(fabs(dy/dx));

	else if(dx > 0 && dy <= 0)

		return PI*2 - atan(fabs(dy/dx));
}

//构造结点—弧段表
node* get_node_arc(point** lines,int* points,int line_num,int &node_id)
{
	node* nodes = (node*)malloc(sizeof(node) * line_num * 2);//开辟结点数组

	//依次遍历各弧段的两端点
	for(int i = 0;i < line_num;i++)

		for(int j = 0,k = 0;k < 2;j = j + points[i + 1] - 1,k++)
		{
			int another;//与结点相邻接的弧段中最靠近结点的一点

			if(k == 0)

				another = j + 1;

			else if(k == 1)

				another = j - 1;

			double a = get_angle(lines[i][j].x,lines[i][j].y,lines[i][another].x,lines[i][another].y);//当前弧段的方位角

			int search;

			//在已有结点数组中搜索，node_id为已有的结点数量
			for(search = 0;search < node_id;search++)
			{
				//若当前弧段的起始结点已存在于结点数组中
				if(nodes[search].x == lines[i][j].x && nodes[search].y == lines[i][j].y)
				{
					arc* new_arc = (arc*)malloc(sizeof(arc));//新建一条弧段

					//设置弧段属性
					new_arc->id = i;

					new_arc->angle = a;

					new_arc->sign = 1;
					
					arc* temp = nodes[search].pn_a;//用于遍历与结点关联弧段的指针

					if(a < temp->angle)//若待插入的弧段的方位角小于已有弧段的最小方位角
					{
						while(temp->next->id != nodes[search].pn_a->id)

							temp = temp->next;//找到最后一条弧段

						//将待插入的弧段插入第一条弧段之前，并修改头指针的位置，使其指向新插入的弧段
						new_arc->next = nodes[search].pn_a;

						temp->next = new_arc;

						nodes[search].pn_a = new_arc;

						break;
					}	
					else//否则遍历弧段链表寻找应当插入的位置
					{
						for(temp = nodes[search].pn_a;;temp = temp->next)
						{
							//若待插入弧段的方位角大小处于两相邻弧段之间，则将其插入这两弧段之间；
							//若待插入弧段的方位角大于所有弧段，则将它插入最后一条弧段与第一条弧段之间
							if((temp->angle < a && temp->next->angle > a)||(temp->angle >= temp->next->angle))

								break;
						}
						new_arc->next = temp->next;

						temp->next = new_arc;

						break;
					}
				}
				//若当前结点不是弧段的起始结点，继续遍历
				else if(nodes[search].x != lines[i][j].x || nodes[search].y != lines[i][j].y)

					continue;
			}
			//若当前弧段的起始结点不存在于结点数组中
			if(search == node_id)
			{
				nodes[node_id].x = lines[i][j].x; nodes[node_id].y = lines[i][j].y;//新建一个结点

				nodes[node_id].pn_a = (arc*)malloc(sizeof(arc));//在结点下新建一个弧段

				//设置弧段属性
				nodes[node_id].pn_a->id = i;

				nodes[node_id].pn_a->angle = a;

				nodes[node_id].pn_a->sign = 1;

				nodes[node_id].pn_a->next = nodes[node_id].pn_a;//该弧段与自身形成循环链表

				node_id++;//结点数增加一个
			}
		}
	return nodes;
}

//构建弧段—结点表，明确文件中各弧段的起止结点
int** get_arc_node(node* nodes,int point_num,point** lines,int* points,int line_num)
{
	int** arc_node = (int**)malloc(line_num * sizeof(int*));

	for(int i = 0;i < line_num;i++)

		arc_node[i] = (int*)malloc(2 * sizeof(int));//开辟二维数组以存放弧段—结点表

	//对于每一条弧段的起点和终点，遍历结点数组找出它们的结点号
	for(int i = 0;i < line_num;i++)
	{
		for(int j = 0;j < point_num;j++)
		{
			if(nodes[j].x == lines[i][0].x && nodes[j].y == lines[i][0].y)//弧段的起点

				arc_node[i][0] = j;

			if(nodes[j].x == lines[i][points[i+1]-1].x && nodes[j].y == lines[i][points[i+1]-1].y)//弧段的终点

				arc_node[i][1] = j;
		}
	}
	return arc_node;
}

//从结点数组中选取起始结点
int start_point(node* nodes,int point_num)
{
	int i;

	for(i = 0;i < point_num;i++)//遍历各结点，顺序选取满足条件的结点
	{
		double first = nodes[i].pn_a->angle;//记录下第一条弧段的方位角，以免重复遍历

		int n = nodes[i].pn_a->sign;//第一条弧段的使用情况

		arc* temp = nodes[i].pn_a->next;//从第二条弧段开始遍历

		while(temp->angle != first)
		{
			n += temp->sign;//统计每条弧段的使用情况

			temp = temp->next;
		}
		if(n)//若存在未被使用过的弧段

			break;
	}
	return i;//返回结点号
}

//从当前结点中选取起始弧段
int get_start_arc(node* nodes,int node_id)
{
	arc* p = nodes[node_id].pn_a;

	while(p->sign == 0)//当此弧段已被使用过

		p = p->next;//转到下一弧段

	p->sign = 0;//将选中的弧段标记为已使用

	return p->id;//返回弧段号
}

//进行左转，选择下一条弧段
int get_next_arc(node* nodes,int node_id,int arc_id)
{
	arc* p = nodes[node_id].pn_a;

	while(p)
	{
		if(p->id == arc_id)//将指针定位到当前弧段

			break;

		p = p->next;
	}
	//选择未被使用过的下一弧段
	p = p->next;

	while(p->sign == 0)

		p = p->next;

	p->sign = 0;//将选中的弧段标记为已使用

	return p->id;//返回弧段号
}

//构造多边形—弧段表
edge** get_polygon_arc(node* nodes,int* points,point** lines,int line_num,int point_num,int &polygon_num,int* &arc_num)
{
	edge** polygon_arc = (edge**)malloc(line_num * 2 * sizeof(edge*));

	for(int i = 0;i < line_num * 2;i++)

		polygon_arc[i] = (edge*)malloc(line_num * sizeof(edge));//开辟二维边数组存放多边形—弧段表

	int** arc_node = get_arc_node(nodes,point_num,lines,points,line_num);//弧段—结点关系表

	int start = start_point(nodes,point_num);//选择起始结点

	int i;//多边形个数

	for(i = 0;start < point_num;i++)
	{
		int arc_id = get_start_arc(nodes,start);//选择起始弧段

		polygon_arc[i][arc_num[i]].arc_id = arc_id;//将该弧段存入多边形—弧段表

		int node_id = arc_node[arc_id][0] + arc_node[arc_id][1] - start;//将弧段的另一个结点作为下一结点

		polygon_arc[i][arc_num[i]].start = start;

		polygon_arc[i][arc_num[i]].end = node_id;//记录弧段的起点和终点

		arc_num[i]++;//当前多边形的弧段数增加1

		while(node_id != start)//当回到起点时结束，说明形成了一个多边形
		{
			arc_id = get_next_arc(nodes,node_id,polygon_arc[i][arc_num[i] - 1].arc_id);//左转至下一弧段

			polygon_arc[i][arc_num[i]].arc_id = arc_id;//将该弧段存入多边形—弧段表

			polygon_arc[i][arc_num[i]].start = node_id;//记录弧段的起点

			int next_node = arc_node[arc_id][0] + arc_node[arc_id][1] - node_id;//将弧段的另一个结点作为下一结点

			node_id = next_node;

			polygon_arc[i][arc_num[i]].end = next_node;//记录弧段的终点

			arc_num[i]++;//当前多边形的弧段数增加1
		}
		start = start_point(nodes,point_num);//重新确定起始结点
	}
	polygon_num = i;

	return polygon_arc;
}

double get_area(point* polygon,int point_num)//计算多边形面积
{
	double area = 0;

	for(int i = 1;i <= point_num;i++)//用公式对每个顶点进行计算，并求和
	{
		//将顶点的序号除总点数取余，确保了第一个点是最后一个点的后继顶点
		area += polygon[i % point_num].x * (polygon[(i + 1) % point_num].y - polygon[(i - 1) % point_num].y);
	}
	return -area / 2.0;//返回整个多边形的面积
}

//建立多边形与结点的关系
point** get_polygons_point(edge** polygon_arc,int** arc_node,int polygon_num,int* arc_num,point** lines,int* points,double* &area,int* &num)
{
	point** polygons = (point**)malloc(polygon_num * sizeof(point*));
	
	int sign = 0;//标记变量，保证首尾相连弧段顺、逆序都存储

	for(int i = 0;i < polygon_num;i++)
	{
		int n = 0;
		for(int j = 0;j < arc_num[i];j++)
		{
			n += points[polygon_arc[i][j].arc_id + 1];//计算多边形共有多少点
		}
		polygons[i] = (point*)malloc(n * sizeof(point));
		int s = 0;
		for(int j = 0;j < arc_num[i];j++)
		{
			if(polygon_arc[i][j].start == polygon_arc[i][j].end)//弧段首尾相连
			{
				if(sign == 0)//若没有存储过，顺序存储
				{
					for(int k = 0;k < points[polygon_arc[i][j].arc_id + 1];k++)
					{
						polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
						polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
						s++;
					}
				}
				else if(sign == 1)//若已经存储过，逆序存储
				{
					for(int k = points[polygon_arc[i][j].arc_id + 1] - 1;k >= 0;k--)
					{
						polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
						polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
						s++;
					}
				}
				sign = !sign;
				break;
			}
			else if(polygon_arc[i][j].start == arc_node[polygon_arc[i][j].arc_id][0])//弧段方向与文件中相同
			{
				for(int k = 0;k < points[polygon_arc[i][j].arc_id + 1];k++)//顺序将弧段中的点取出
				{
					polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
					polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
					s++;
				}
			}
			else if(polygon_arc[i][j].start == arc_node[polygon_arc[i][j].arc_id][1])//弧段方向与文件中相反
			{
				for(int k = points[polygon_arc[i][j].arc_id + 1] - 1;k >= 0;k--)//倒序将弧段中的点取出
				{
					polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
					polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
					s++;
				}
			}
		}
		area[i] = get_area(polygons[i],n);
		num[i] = n;
	}
	return polygons;
}

int** get_arc_polygon(edge** polygon_arc,double* areas,int polygon_num,int* arc_num,int** a_n,point** lines,int line_num,int* points)
{
	int* num = (int*)malloc(polygon_num * sizeof(int));

	point** polygons_point = get_polygons_point(polygon_arc,a_n,polygon_num,arc_num,lines,points,areas,num);

	int** arc_polygon = (int**)malloc(line_num * sizeof(int*));

	for(int i = 0;i < line_num;i++)

		arc_polygon[i] = (int*)malloc(4 * sizeof(int));//开辟二维数组以存放弧段—结点表

	for(int i = 0;i < polygon_num;i++)
	{
		if(areas[i] > 0)
		{
			for(int j = 0;j < arc_num[i];j++)
			{
				if(polygon_arc[i][j].start == a_n[polygon_arc[i][j].arc_id][0])
				{
					arc_polygon[polygon_arc[i][j].arc_id][0] = i;
					arc_polygon[polygon_arc[i][j].arc_id][3] = i;

				}
				else if(polygon_arc[i][j].start == a_n[polygon_arc[i][j].arc_id][1])
				{
					arc_polygon[polygon_arc[i][j].arc_id][2] = i;
					arc_polygon[polygon_arc[i][j].arc_id][1] = i;
				}
			}
		}
	}
	
	double **range = (double**)malloc(sizeof(double*) * polygon_num);

	for(int i = 0; i < polygon_num; i++)

		range[i] = (double*)malloc(4 * sizeof(double));//建立双精度二维数组存放各多边形外接矩形范围

	for(int i = 0;i < polygon_num;i++)
	{
		range[i][0] = polygons_point[i][0].x,range[i][1] = polygons_point[i][0].x;//多边形经度范围

		range[i][2] = polygons_point[i][0].y,range[i][3] = polygons_point[i][0].y;//多边形纬度范围

		for(int j = 0;j < num[i];j++)
		{
			if(polygons_point[i][j].x < range[i][0])
				range[i][0] = polygons_point[i][j].x;//x最小值

			if(polygons_point[i][j].x > range[i][1])
				range[i][1] = polygons_point[i][j].x;//x最大值

			if(polygons_point[i][j].y < range[i][2])
				range[i][2] = polygons_point[i][j].y;//y最小值

			if(polygons_point[i][j].y > range[i][3])
				range[i][3] = polygons_point[i][j].y;//y最大值
		}
	}
	for(int i = 0;i < polygon_num;i++)
	{
		if(areas[i] < 0)//若面积小于0，此多边形为岛
		{
			int j;
			for(j = 0;j < polygon_num;j++)//遍历所有多边形，寻找包含此岛的多边形
			{
				if(areas[j] > 0)
					if(range[i][0] > range[j][0] && range[i][1] < range[j][1])
						if(range[i][2] > range[j][2] && range[i][3] < range[j][3])
						{
							break;
						}
			}
			if(j == polygon_num)
				j = i;
			for(int k = 0;k < arc_num[i];k++)//判断左右多边形
			{
				if(polygon_arc[i][k].start == polygon_arc[i][k].end)
				{
					arc_polygon[polygon_arc[i][k].arc_id][2] = j;
					arc_polygon[polygon_arc[i][k].arc_id][1] = j;
					break;
				}
				if(polygon_arc[i][k].start == a_n[polygon_arc[i][k].arc_id][0])
				{
					arc_polygon[polygon_arc[i][k].arc_id][0] = j;
					arc_polygon[polygon_arc[i][k].arc_id][3] = j;

				}
				else if(polygon_arc[i][k].start == a_n[polygon_arc[i][k].arc_id][1])
				{
					arc_polygon[polygon_arc[i][k].arc_id][2] = j;
					arc_polygon[polygon_arc[i][k].arc_id][1] = j;
				}
			}
		}
	}
	return arc_polygon;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int line_num = 0;

	int* points = (int*)malloc(sizeof(int) * 50);

	char* file_name = "..//PlyBuild_1_arc.gen";

	point** lines = Read(file_name,points,line_num);//读文件获取二维点数组

	int node_id = 0;

	node* nodes = get_node_arc(lines,points,line_num,node_id);
	
	int** a_n = get_arc_node(nodes,node_id,lines,points,line_num);
	
	int polygon_num = 0;
	int* arc_num = (int*)malloc(line_num * 2 * sizeof(int));
	for(int i = 0;i < line_num*2;i++)
		arc_num[i] = 0;
	edge** polygon_arc = get_polygon_arc(nodes,points,lines,line_num,node_id,polygon_num,arc_num);
	
	for(int i = 0;i < polygon_num;i++)
	{
		printf("A%d:",i);
		for(int j = 0;j < arc_num[i];j++)
			printf("N%dN%d->",polygon_arc[i][j].start,polygon_arc[i][j].end);
		printf("\n");
	}

	double* areas = (double*)malloc(polygon_num * sizeof(double));
	for(int i = 0;i < polygon_num;i++)
		areas[i] = 0;
	int** arc_polygon = get_arc_polygon(polygon_arc,areas,polygon_num,arc_num,a_n,lines,line_num,points);
	printf("弧段  右多边形  左多边形  弧段  右多边形  左多边形\n");
	for(int i = 0;i < line_num;i++)
	{
		int j;
		printf("N%dN%d    ",a_n[i][0],a_n[i][1]);
		for(j = 0;j < 2;j++)
			printf("A%d       ",arc_polygon[i][j]);
		printf("N%dN%d    ",a_n[i][1],a_n[i][0]);
		for(;j < 4;j++)
			printf("A%d       ",arc_polygon[i][j]);
		printf("\n");
	}
	getchar();
	return 0;
}

