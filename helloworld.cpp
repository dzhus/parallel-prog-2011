// helloworld.cpp : Benchmarking SSE and OpenMP (array sum problem)
//
#include <omp.h>
#include <stdio.h>
#include <windows.h>
#include <cstring>


void print_by_char(char *c)
{
	do
	{
		printf("%c", *c);
	}
	while (*c++);
}

void fill_array(float *x, int count)
{
	for (int i = 0; i < count; i++)
		x[i] = i * 1.0;
}

void print_array(float *x, int count)
{
	for (int i = 0; i < count; i++)
		printf("%f ", x[i]);
}

float sum_array_omp(float *x, int count)
{
	float sum = 0;
#pragma omp parallel for num_threads(4), reduction(+:sum)
	for (int i = 0; i < count; i++)
		sum += x[i];
	return sum;
}



float sum_array_sse(float *x, int count)
{
	_asm {
		mov ebx, x;
		mov edx, x;
		xorps xmm1, xmm1;
		; количество элементов в массиве
		mov ecx, count;
		shr ecx, 2;

step:
		; загоняем числа в регистры SSE по 4 штуки
		movaps xmm0, [ebx];
		addps xmm1, xmm0;

		add ebx, 16
		loop step;
		movaps [edx], xmm1
	}
	return x[0] + x[1] + x[2] + x[3];
}

float sum_array_sse_omp(float *x, int count)
{
	float sum = 0;
	int i;
#pragma omp parallel for num_threads(4), reduction(+:sum)
	for (i = 0; i <= 1; i++)
	{
		sum += sum_array_sse(x + i * (count / 2), count / 2);
	}
	return sum;
}

int main(int argc, char* argv[])
{
	double tick = omp_get_wtick();
	double start = omp_get_wtime();

/// при ста не пашет
	const int x_count = 100000000;
	srand(1989);

    // Выделение памяти
    char *pc = new char [(x_count + 16) * sizeof(float)];
    int ip = (int)pc;
    ip = ((ip + 16) >> 4) << 4;
    float *ar = (float *)ip;

	fill_array(ar, x_count);
//	print_array(ar, x_count);
	printf("\n");
	float s = sum_array_sse_omp(ar, x_count);
	printf("Elements: %d, Sum: %lg\n", x_count, s);

	//	Sleep(1000);
	double end = omp_get_wtime();
	printf("Tick: %lg; duration: %lg\n", tick, (end - start));
	return 0;
}

