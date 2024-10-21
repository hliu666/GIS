#include <iostream>
#include <cmath>
using namespace std;
void main()
{
	double x[12]={4,4,8,8,-4,-4,4,3,5,12,9,5},
		y[12]={6,-4,-4,-8,-8,6,6,4,11,8,5,6};
	double area=0;
	for(int i=0;i<11;i++)
	{
		area+=(x[i]+x[i+1])*(y[i+1]-y[i]);
	}
	area/=2;
    area=abs(area);
	cout<<"The polygon's area is "<<area<<"\n";
  
}

