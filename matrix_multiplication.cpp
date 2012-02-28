#include <iostream>

void print_matrix(float *pf)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            printf("%8.2f ", pf[i * 4 + j]);
        printf("\n");
    }
    printf("\n");
}

void transpose(float *pf)
{
    _asm {
        mov ebx, pf
            movaps xmm0, [ebx]
            movaps xmm1, [ebx + 16]
            movaps xmm2, [ebx + 32]
            movaps xmm3, [ebx + 48]

            movaps xmm4, xmm0;
        movlhps xmm4, xmm1;
        movaps xmm5, xmm2;
        movlhps xmm5, xmm3;
        movaps xmm6, xmm4;
        shufps xmm6, xmm5, 0x88;
        shufps xmm4, xmm5, 0xDD;
        movaps [ebx], xmm6;
        movaps [ebx+16], xmm4;

        movhlps xmm0, xmm1;
        movhlps xmm2, xmm3;
		
        movaps xmm6, xmm0;
        shufps xmm6, xmm2, 0x22;
        shufps xmm0, xmm2, 0x77;
        movaps [ebx + 32], xmm6;
        movaps [ebx + 48], xmm0;
    }
}

void multiply_c(float *m1, float *m2, float *m3)
{
    float t;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            t = 0.0;
            for (int k = 0; k < 4; k++)
            {
                t += m1[i * 4 + k] * m2[k * 4 + j];
                m3[i * 4 + j] = t;
            }
        }
}

void multiply_asm(float *m1, float *m2)
{
    transpose(m2);
    _asm {
        mov ebx, m2;
        mov ecx, 4;
        movaps xmm0, [ebx];
        movaps xmm1, [ebx + 16];
        movaps xmm2, [ebx + 32];
        movaps xmm3, [ebx + 48];
        mov ebx, m1;
    row:
        movaps xmm4, [ebx];

        movaps xmm5, xmm4;
        mulps xmm4, xmm0;
        mulps xmm5, xmm1;

        movaps xmm6, xmm4;
        movlhps xmm4, xmm5;
        movhlps xmm5, xmm6;
        addps xmm4, xmm5;
        ; Б 4 КЕФХР ОНКСЯСЛЛЮ

              movaps xmm6, [ebx];
        movaps xmm5, xmm6;
        mulps xmm5, xmm2;
        mulps xmm6, xmm3;

        movaps xmm7, xmm6;
        movhlps xmm6, xmm5;
        movlhps xmm5, xmm7;
        addps xmm5, xmm6;
        ; Б 5 КЕФХР ОНКСЯСЛЛЮ ;

        movaps xmm6, xmm4;

        shufps xmm4, xmm5, 0x88;
        shufps xmm6, xmm5, 0xDD;
        addps xmm4, xmm6;
        movaps [ebx], xmm4;

        add ebx, 16;
        dec ecx;
        jz finish;
        jmp row;

    finish:
    }
}

int main(int argc, char* argv[])
{
    char k;
    char *pc = new char [1024];
    // Realign
    int ip = (int)pc;
    ip = ((ip + 16) >> 4) << 4;

    float *pf = (float *)ip;
    for (int i = 0; i < 16; i++)
    {
        pf[i] = (float)i;
    }

    print_matrix(pf);
    transpose(pf);
    print_matrix(pf);

    float *pf2 = pf + 16;
    float *pf3 = pf + 32;
    for (int i = 0; i < 16; i++)
    {
        pf[i] = (float)i;
        pf2[i] = (float)i;
    }
    multiply_c(pf, pf2, pf3);
    print_matrix(pf3);

    multiply_asm(pf, pf2);
    print_matrix(pf);
    delete[] pc;
    pc = 0;

    std::cin >> k;
    return 0;
}
