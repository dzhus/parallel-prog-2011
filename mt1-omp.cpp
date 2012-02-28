// mt1-omp.cpp : Multithreaded particle system with OpenMP

#include "stdafx.h"
#include <conio.h>
#include <windows.h>
#include <omp.h>

int quit = 0;

const int x_max = 60;
const int y_max = 21;
const int seed = 1989;

const int particle_count = 5;

COORD g_aCoord[particle_count], g_aAdd[particle_count];

CRITICAL_SECTION cs;

HANDLE hc;


void draw_board(HANDLE hc)
{
	COORD cur;
	int i;

	// lower horizontal
	cur.Y = y_max;
	for (i = 0; i < x_max; i++)
	{
		cur.X = i;
		SetConsoleCursorPosition(hc, cur);
		printf("=");
	}

	// right vertical
	cur.X = x_max;
	for (i = 0; i < y_max; i++)
	{
		cur.Y = i;
		SetConsoleCursorPosition(hc, cur);
		printf("|");
	}
}

void process_particle(int id)
{
#pragma omp critical 
	{
	SetConsoleCursorPosition(hc, g_aCoord[id]);
	printf(" ");
	}
	g_aCoord[id].X += g_aAdd[id].X;
	g_aCoord[id].Y += g_aAdd[id].Y;

	// boundary checking

	// vertical wall
	if ((g_aCoord[id].X < 0) || (g_aCoord[id].X >= x_max))
	{
		g_aCoord[id].X += g_aAdd[id].X * -2;
		g_aAdd[id].X *= -1;
	}

	// horizontal wall
	if ((g_aCoord[id].Y < 0) || (g_aCoord[id].Y >= y_max))
	{
		g_aCoord[id].Y += g_aAdd[id].Y * -2;
		g_aAdd[id].Y *= -1;
	}

#pragma omp critical
	{
	SetConsoleCursorPosition(hc, g_aCoord[id]);
	printf("%d", id);
	}
}

void particle_thread(int id)
{
	while(!quit)
	{
		process_particle(id);
		Sleep(25);
	}
}

/// Initialize particle positions and set random velocities
void do_particles(HANDLE hc)
{
	for (int i = 0; i < particle_count; i++)
	{
		g_aCoord[i].X = rand() % x_max;
		g_aCoord[i].Y = rand() % y_max;
		g_aAdd[i].X = (rand() & 1) * 2 - 1;
		g_aAdd[i].Y = (rand() & 1) * 2 - 1;
		SetConsoleCursorPosition(hc, g_aCoord[i]);
		printf("%d", i);
	}
}

int main(int argc, char* argv[])
{
	hc = GetStdHandle(STD_OUTPUT_HANDLE);

	srand(seed);

	draw_board(hc);
	do_particles(hc);

	//while (1)
	//{
	//	for (int i = 0; i < particle_count; i++)
	//		process_particle(hc, i);
	//	if (kbhit() && (getch() == 27))
	//		break;
	//	Sleep(50);
	//}
	int i;
#pragma omp parallel for num_threads(11)
	for (i = 0; i < particle_count + 1; i++)
	{
		if (i == particle_count)
		{
			while(1)
				if (kbhit() && (getch() == 27))
				{
					quit = 1;
					break;
				}
		}
		else
			particle_thread(i);
	}
	
	return 0;
}

