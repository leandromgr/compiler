
#include <stdio.h>

//int a[2] ={2, 4};
//float b = 1;
//char b = 't';
/*int d[5];
int e[5] = {1,2,3,4,5};
*/
/*
int convert(int arg1, int arg2)
{
	int local1 = 10;
	char local2 = '5';
	char local3 = '6';
	printf("%c", local2);
	//local2 = local2 + arg1;
	return local1;
}
*/

int convert2(int a, int b, int c, int d, int e, int f, int g, int h, int i, int k)
{
	int local1 = 3;
	local1 = a + b + g;
	return local1;
}


int convert3(int ax, int bx)
{
	int a = 10;
	int b = 50;
}

void func(int a, int b)
{
	int local1 = 5;
	//char local2 = 't';
	//int local3 = 9;
	printf("%i .. ", local1);
	//printf("%c.", local2);
	//printf("%i .. ", local3);
}

int main (int a, int b)
{
	int local = 52;
	char local2 = 't';
	printf("%i ..", local);
	printf("%c.", local2);
	local2 = convert2(2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	//char c = 't';
	//float f = 3.14; 
	//printf("Valor");
	//convert(1, 2);
	func(local2, 2);
	//printf("%f", a[1]);
	//printf("%i", local);
	//printf("%f", b);
	//printf("%c", c);
	//printf("%f", f);
	//a = a + 3;
	//scanf("%i", &a);
	//printf("Ola, o valor eh: %i", a);
	return 0;
}

