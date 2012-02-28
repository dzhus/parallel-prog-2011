#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <windows.h>

const int seed = 1989;

/// Allocate 16-bit-aligned memory chunk
///
/// @param size Minimum size of aligned memory chunk
float* alloc_aligned(const int size)
{
    char *pc = new char [(size + 16) * sizeof(float)];
    int ip = (int)pc;
    ip = ((ip + 16) >> 4) << 4;
	return (float *)ip;
}

/// @param N Amount of points in a sample (must be even)
///
/// @param x Points of sample, 2*N-long float array 
///          of real and imaginary parts.
/// @param res Real parts of DFT result
void dft(float *x,
		 float *m,
		 float *res,
		 const int N)
{
	float *c = alloc_aligned(4);
	c[0] = 1.0;
	c[1] = -1.0;
	c[2] = 1.0;
	c[3] = -1.0;
	_asm {
		; Вектор +/- единичек
		mov ebx, c;
		movaps xmm3, [ebx];

		; Сумма двух соседних коэффициентов в точке
		xorps xmm4, xmm4;

		mov ebx, m;
		mov edi, res;
		mov ecx, N;

; (1)
step_point:
		mov edx, ecx;

		; Будем перемножать по две точки
		mov ecx, N;
		shr ecx, 1;

		mov eax, x;
		; ebx не сбрасываем, коэффициенты каждый раз новые

		; Сумма всех коэффициентов по точке
		xorps xmm5, xmm5;

		;; (2)
		step_convolute:
				movaps xmm0, [eax];
				movaps xmm1, [ebx];

				;; Mutliply two complex values
				movaps xmm2, xmm0;

				; Сделали по паре действительных частей
				shufps xmm2, xmm2, 0xA0;
				mulps xmm2, xmm1;

				; Сделали по паре мнимых частей
				shufps xmm0, xmm0, 0xF5;
				mulps xmm0, xmm3;
				mulps xmm0, xmm1;

				; Собираем мнимые и действительные
				shufps xmm0, xmm0, 0xB1;
				addps xmm0, xmm2;

				; Выстраиваем и складываем два соседних числа в свёртке
				movlhps xmm4, xmm0;
				addps xmm4, xmm0;

				; xmm4[2-3] содержат сумму
				addps xmm5, xmm4;

				; дальше возьмём следующие точки
				add eax, 16;
				add ebx, 16;
				dec ecx;
				jz finish_convolute;
				jmp step_convolute; (2)
                finish_convolute:

		; xmm5[2-3] содержат общую сумму для точки, её надо в res
		movhps [edi], xmm5;
		add edi, 8;


		mov ecx, edx;
		dec ecx;
		jz finish;
		jmp step_point; (1)

finish:
	}
}

/// Generate N random complex values which shall not
/// exceed max_value divided by dec_divior.
void fill_values(float *x, const int N, 
				 const int max_value = 10000, const float dec_divisor = 100.0)
{
	for (int i = 0; i < N; i++)
	{
		x[2 * i] = rand() % max_value / dec_divisor;
		x[2 * i + 1] = rand() % max_value / dec_divisor;
	}
}


/// Generate matrix of discrete Fourier transform for N points
///
/// @param m_real Empty matrix packed by rows, 2*N*N
void fill_exp_matrix(float *m, const int N)
{
	float x;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			/// Each row is 2*N long!
			m[i * 2 * N + 2 * j] =  static_cast<float>(cos(2 * M_PI / N * i * j));
			m[i * 2 * N + 2 * j + 1] = -static_cast<float>(sin(2 * M_PI / N * i * j));
		}
}

/// Print complex vector in Octave-friendly format
void print_values(float *x, const int N)
{
	printf("[");
	for (int i = 0; i < N; i++)
	{
		printf("%-lg + %-lgi", x[2 * i], x[2 * i + 1]);
		if (i < (N - 1))
			printf("; ");
	}
	printf("]\n");
}


int main(int argc, char* argv[])
{
	/// Sample size, must be even
	const int N = 8;

	float 
		*x = alloc_aligned(2 * N), 
		*res = alloc_aligned(2 * N), 
		*m = alloc_aligned(2 * N * N);

	srand(N);
	fill_values(x, N);
	fill_exp_matrix(m, N);
	
	print_values(x, N);
	dft(x, m, res, N);
	print_values(res, N);
}

