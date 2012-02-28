// omp-schedulers.cpp : Comparing OpenMP schedulers
// Static: первый излишки берёт на себя
// Dynamic: берёт, сколько сказали.
// Guided: округление вверх.

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

const int threads = 5;
const int iterations = 101;

int main(int argc, char* argv[])
{
	int i, nt;
	char name[1024];
	FILE *f[threads];

	omp_set_num_threads(threads);

	for (i = 0; i < threads; i++)
	{
		sprintf(name, "%d.txt", i);
		f[i] = fopen(name, "wt");
	}

#pragma omp parallel for schedule(guided)
	for (i = 0; i < iterations; i++)
	{
		nt = omp_get_thread_num();
		fprintf(f[nt], "%d\n", i);
		Sleep(0);
	}
	return 0;
}

