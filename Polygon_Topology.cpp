// topo.cpp : �������̨Ӧ�ó������ڵ㡣
//
// polygon_topology.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<stdlib.h>
#include<math.h>

#define PI 3.1415926

typedef struct arc//��������
{
	int id;//�������

	double angle;//���η�λ��

	int sign;//����ʹ�ñ��

	arc* next;//ָ����һ���ε�ָ��
}arc;

typedef struct//��������������
{
	int arc_id;//���κ�

	int start;//��ʼ������

	int end;//��ֹ������
}edge;

typedef struct//�������
{
	double x,y;//��������

	arc* pn_a;//ָ�򻡶������ͷָ��
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
		puts("���ܴ��ļ���");

		return 0;
	}
	char temp1[100] = {0};//��ʱ����

	char temp2[100] = {0};//��ʱ����

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

//���㻡�εķ�λ��
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

//�����㡪���α�
node* get_node_arc(point** lines,int* points,int line_num,int &node_id)
{
	node* nodes = (node*)malloc(sizeof(node) * line_num * 2);//���ٽ������

	//���α��������ε����˵�
	for(int i = 0;i < line_num;i++)

		for(int j = 0,k = 0;k < 2;j = j + points[i + 1] - 1,k++)
		{
			int another;//�������ڽӵĻ������������һ��

			if(k == 0)

				another = j + 1;

			else if(k == 1)

				another = j - 1;

			double a = get_angle(lines[i][j].x,lines[i][j].y,lines[i][another].x,lines[i][another].y);//��ǰ���εķ�λ��

			int search;

			//�����н��������������node_idΪ���еĽ������
			for(search = 0;search < node_id;search++)
			{
				//����ǰ���ε���ʼ����Ѵ����ڽ��������
				if(nodes[search].x == lines[i][j].x && nodes[search].y == lines[i][j].y)
				{
					arc* new_arc = (arc*)malloc(sizeof(arc));//�½�һ������

					//���û�������
					new_arc->id = i;

					new_arc->angle = a;

					new_arc->sign = 1;
					
					arc* temp = nodes[search].pn_a;//���ڱ�������������ε�ָ��

					if(a < temp->angle)//��������Ļ��εķ�λ��С�����л��ε���С��λ��
					{
						while(temp->next->id != nodes[search].pn_a->id)

							temp = temp->next;//�ҵ����һ������

						//��������Ļ��β����һ������֮ǰ�����޸�ͷָ���λ�ã�ʹ��ָ���²���Ļ���
						new_arc->next = nodes[search].pn_a;

						temp->next = new_arc;

						nodes[search].pn_a = new_arc;

						break;
					}	
					else//���������������Ѱ��Ӧ�������λ��
					{
						for(temp = nodes[search].pn_a;;temp = temp->next)
						{
							//�������뻡�εķ�λ�Ǵ�С���������ڻ���֮�䣬���������������֮�䣻
							//�������뻡�εķ�λ�Ǵ������л��Σ������������һ���������һ������֮��
							if((temp->angle < a && temp->next->angle > a)||(temp->angle >= temp->next->angle))

								break;
						}
						new_arc->next = temp->next;

						temp->next = new_arc;

						break;
					}
				}
				//����ǰ��㲻�ǻ��ε���ʼ��㣬��������
				else if(nodes[search].x != lines[i][j].x || nodes[search].y != lines[i][j].y)

					continue;
			}
			//����ǰ���ε���ʼ��㲻�����ڽ��������
			if(search == node_id)
			{
				nodes[node_id].x = lines[i][j].x; nodes[node_id].y = lines[i][j].y;//�½�һ�����

				nodes[node_id].pn_a = (arc*)malloc(sizeof(arc));//�ڽ�����½�һ������

				//���û�������
				nodes[node_id].pn_a->id = i;

				nodes[node_id].pn_a->angle = a;

				nodes[node_id].pn_a->sign = 1;

				nodes[node_id].pn_a->next = nodes[node_id].pn_a;//�û����������γ�ѭ������

				node_id++;//���������һ��
			}
		}
	return nodes;
}

//�������Ρ�������ȷ�ļ��и����ε���ֹ���
int** get_arc_node(node* nodes,int point_num,point** lines,int* points,int line_num)
{
	int** arc_node = (int**)malloc(line_num * sizeof(int*));

	for(int i = 0;i < line_num;i++)

		arc_node[i] = (int*)malloc(2 * sizeof(int));//���ٶ�ά�����Դ�Ż��Ρ�����

	//����ÿһ�����ε������յ㣬������������ҳ����ǵĽ���
	for(int i = 0;i < line_num;i++)
	{
		for(int j = 0;j < point_num;j++)
		{
			if(nodes[j].x == lines[i][0].x && nodes[j].y == lines[i][0].y)//���ε����

				arc_node[i][0] = j;

			if(nodes[j].x == lines[i][points[i+1]-1].x && nodes[j].y == lines[i][points[i+1]-1].y)//���ε��յ�

				arc_node[i][1] = j;
		}
	}
	return arc_node;
}

//�ӽ��������ѡȡ��ʼ���
int start_point(node* nodes,int point_num)
{
	int i;

	for(i = 0;i < point_num;i++)//��������㣬˳��ѡȡ���������Ľ��
	{
		double first = nodes[i].pn_a->angle;//��¼�µ�һ�����εķ�λ�ǣ������ظ�����

		int n = nodes[i].pn_a->sign;//��һ�����ε�ʹ�����

		arc* temp = nodes[i].pn_a->next;//�ӵڶ������ο�ʼ����

		while(temp->angle != first)
		{
			n += temp->sign;//ͳ��ÿ�����ε�ʹ�����

			temp = temp->next;
		}
		if(n)//������δ��ʹ�ù��Ļ���

			break;
	}
	return i;//���ؽ���
}

//�ӵ�ǰ�����ѡȡ��ʼ����
int get_start_arc(node* nodes,int node_id)
{
	arc* p = nodes[node_id].pn_a;

	while(p->sign == 0)//���˻����ѱ�ʹ�ù�

		p = p->next;//ת����һ����

	p->sign = 0;//��ѡ�еĻ��α��Ϊ��ʹ��

	return p->id;//���ػ��κ�
}

//������ת��ѡ����һ������
int get_next_arc(node* nodes,int node_id,int arc_id)
{
	arc* p = nodes[node_id].pn_a;

	while(p)
	{
		if(p->id == arc_id)//��ָ�붨λ����ǰ����

			break;

		p = p->next;
	}
	//ѡ��δ��ʹ�ù�����һ����
	p = p->next;

	while(p->sign == 0)

		p = p->next;

	p->sign = 0;//��ѡ�еĻ��α��Ϊ��ʹ��

	return p->id;//���ػ��κ�
}

//�������Ρ����α�
edge** get_polygon_arc(node* nodes,int* points,point** lines,int line_num,int point_num,int &polygon_num,int* &arc_num)
{
	edge** polygon_arc = (edge**)malloc(line_num * 2 * sizeof(edge*));

	for(int i = 0;i < line_num * 2;i++)

		polygon_arc[i] = (edge*)malloc(line_num * sizeof(edge));//���ٶ�ά�������Ŷ���Ρ����α�

	int** arc_node = get_arc_node(nodes,point_num,lines,points,line_num);//���Ρ�����ϵ��

	int start = start_point(nodes,point_num);//ѡ����ʼ���

	int i;//����θ���

	for(i = 0;start < point_num;i++)
	{
		int arc_id = get_start_arc(nodes,start);//ѡ����ʼ����

		polygon_arc[i][arc_num[i]].arc_id = arc_id;//���û��δ������Ρ����α�

		int node_id = arc_node[arc_id][0] + arc_node[arc_id][1] - start;//�����ε���һ�������Ϊ��һ���

		polygon_arc[i][arc_num[i]].start = start;

		polygon_arc[i][arc_num[i]].end = node_id;//��¼���ε������յ�

		arc_num[i]++;//��ǰ����εĻ���������1

		while(node_id != start)//���ص����ʱ������˵���γ���һ�������
		{
			arc_id = get_next_arc(nodes,node_id,polygon_arc[i][arc_num[i] - 1].arc_id);//��ת����һ����

			polygon_arc[i][arc_num[i]].arc_id = arc_id;//���û��δ������Ρ����α�

			polygon_arc[i][arc_num[i]].start = node_id;//��¼���ε����

			int next_node = arc_node[arc_id][0] + arc_node[arc_id][1] - node_id;//�����ε���һ�������Ϊ��һ���

			node_id = next_node;

			polygon_arc[i][arc_num[i]].end = next_node;//��¼���ε��յ�

			arc_num[i]++;//��ǰ����εĻ���������1
		}
		start = start_point(nodes,point_num);//����ȷ����ʼ���
	}
	polygon_num = i;

	return polygon_arc;
}

double get_area(point* polygon,int point_num)//�����������
{
	double area = 0;

	for(int i = 1;i <= point_num;i++)//�ù�ʽ��ÿ��������м��㣬�����
	{
		//���������ų��ܵ���ȡ�࣬ȷ���˵�һ���������һ����ĺ�̶���
		area += polygon[i % point_num].x * (polygon[(i + 1) % point_num].y - polygon[(i - 1) % point_num].y);
	}
	return -area / 2.0;//������������ε����
}

//�������������Ĺ�ϵ
point** get_polygons_point(edge** polygon_arc,int** arc_node,int polygon_num,int* arc_num,point** lines,int* points,double* &area,int* &num)
{
	point** polygons = (point**)malloc(polygon_num * sizeof(point*));
	
	int sign = 0;//��Ǳ�������֤��β��������˳�����򶼴洢

	for(int i = 0;i < polygon_num;i++)
	{
		int n = 0;
		for(int j = 0;j < arc_num[i];j++)
		{
			n += points[polygon_arc[i][j].arc_id + 1];//�������ι��ж��ٵ�
		}
		polygons[i] = (point*)malloc(n * sizeof(point));
		int s = 0;
		for(int j = 0;j < arc_num[i];j++)
		{
			if(polygon_arc[i][j].start == polygon_arc[i][j].end)//������β����
			{
				if(sign == 0)//��û�д洢����˳��洢
				{
					for(int k = 0;k < points[polygon_arc[i][j].arc_id + 1];k++)
					{
						polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
						polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
						s++;
					}
				}
				else if(sign == 1)//���Ѿ��洢��������洢
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
			else if(polygon_arc[i][j].start == arc_node[polygon_arc[i][j].arc_id][0])//���η������ļ�����ͬ
			{
				for(int k = 0;k < points[polygon_arc[i][j].arc_id + 1];k++)//˳�򽫻����еĵ�ȡ��
				{
					polygons[i][s].x = lines[polygon_arc[i][j].arc_id][k].x;
					polygons[i][s].y = lines[polygon_arc[i][j].arc_id][k].y;
					s++;
				}
			}
			else if(polygon_arc[i][j].start == arc_node[polygon_arc[i][j].arc_id][1])//���η������ļ����෴
			{
				for(int k = points[polygon_arc[i][j].arc_id + 1] - 1;k >= 0;k--)//���򽫻����еĵ�ȡ��
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

		arc_polygon[i] = (int*)malloc(4 * sizeof(int));//���ٶ�ά�����Դ�Ż��Ρ�����

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

		range[i] = (double*)malloc(4 * sizeof(double));//����˫���ȶ�ά�����Ÿ��������Ӿ��η�Χ

	for(int i = 0;i < polygon_num;i++)
	{
		range[i][0] = polygons_point[i][0].x,range[i][1] = polygons_point[i][0].x;//����ξ��ȷ�Χ

		range[i][2] = polygons_point[i][0].y,range[i][3] = polygons_point[i][0].y;//�����γ�ȷ�Χ

		for(int j = 0;j < num[i];j++)
		{
			if(polygons_point[i][j].x < range[i][0])
				range[i][0] = polygons_point[i][j].x;//x��Сֵ

			if(polygons_point[i][j].x > range[i][1])
				range[i][1] = polygons_point[i][j].x;//x���ֵ

			if(polygons_point[i][j].y < range[i][2])
				range[i][2] = polygons_point[i][j].y;//y��Сֵ

			if(polygons_point[i][j].y > range[i][3])
				range[i][3] = polygons_point[i][j].y;//y���ֵ
		}
	}
	for(int i = 0;i < polygon_num;i++)
	{
		if(areas[i] < 0)//�����С��0���˶����Ϊ��
		{
			int j;
			for(j = 0;j < polygon_num;j++)//�������ж���Σ�Ѱ�Ұ����˵��Ķ����
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
			for(int k = 0;k < arc_num[i];k++)//�ж����Ҷ����
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

	point** lines = Read(file_name,points,line_num);//���ļ���ȡ��ά������

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
	printf("����  �Ҷ����  ������  ����  �Ҷ����  ������\n");
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

