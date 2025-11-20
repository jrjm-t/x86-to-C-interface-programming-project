#include <stdio.h>

extern void asmMCO2(int n, float* x1, float* x2, float* y1, float* y2);

int main()
{
	int n = 10;
	// some hardcoded example inputs for now
	float x1[10] = {1,2,3,4,5,6,7,8,9,10};
	float x2[10] = {10,2,3,4,5,6,7,8,9,10};
	float y1[10] = {1,2,3,4,5,6,7,8,9,10};
	float y2[10] = {1,2,3,4,5,6,7,8,9,10};
	printf("Hi there!");
	asmMCO2(n, x1, x2, y1, y2);
	printf("Yes hello");
	return 0;
};
