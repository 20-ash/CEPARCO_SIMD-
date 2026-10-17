#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>

extern void kernel_asm_x64(float*, float*, float*, int*, int);
//extern void kernel_avx2_xmm(float*, float*, float*, int*, int);
//extern void kernel_avx2_ymm(float*, float*, float*, int*, int);

// Function to time the execution of a kernel function
double time_kernel(void (*kernel)(float*, float*, float*, int*, int), float* A, float* B, float* output, int* idx, int size) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double total_ms = 0.0;

    QueryPerformanceFrequency(&frequency);

    for (int i = 0; i < 30; i++) {
        QueryPerformanceCounter(&start);

        kernel(A, B, output, idx, size);

        QueryPerformanceCounter(&end);

        double elapsed_ms = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000.0;
        total_ms += elapsed_ms;
    }

    return total_ms / 30;
}

void generate_random_array(float* A, float* B, int size) {
    for (int i = 0; i < size; i++) {
        A[i] = sin(i * 0.001);
        B[i] = cos(i * 0.002);
    }
}

void kernel_c(float* A, float* B, float* C, int* idx, int size) {
    for (int i = 0; i < size; i++) {
        if (A[i] >= B[i]) {
            C[i] = A[i];
            idx[i] = 0;
        }
        else {
            C[i] = B[i];
            idx[i] = 1;
        }
    }
}

// Count how many elements differ from the C reference output
int count_mismatches(float* refC, int* refIdx, float* C, int* idx, int size) {
    int errors = 0;
    for (int i = 0; i < size; i++) {
        if (refC[i] != C[i] || refIdx[i] != idx[i]) {
            errors++;
        }
    }
    return errors;
}

int main() {
    int sizes[] = { 1048576, 67108864, 20 };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int size = sizes[i];
        float* A = (float*)malloc(size * sizeof(float));
        float* B = (float*)malloc(size * sizeof(float));
        float* C_output = (float*)malloc(size * sizeof(float));
        float* asm_x64_output = (float*)malloc(size * sizeof(float));
        float* avx2_xmm_output = (float*)malloc(size * sizeof(float));
        float* avx2_ymm_output = (float*)malloc(size * sizeof(float));
        int* C_idx = (int*)malloc(size * sizeof(int));
        int* asm_x64_idx = (int*)malloc(size * sizeof(int));
        int* avx2_xmm_idx = (int*)malloc(size * sizeof(int));
        int* avx2_ymm_idx = (int*)malloc(size * sizeof(int));

        if (!A || !B || !C_output || !asm_x64_output || !avx2_xmm_output || !avx2_ymm_output ||
            !C_idx || !asm_x64_idx || !avx2_xmm_idx || !avx2_ymm_idx) {
            printf("Size %d: not enough memory, skipping\n\n", size);
            free(A); free(B); free(C_output); free(asm_x64_output); free(avx2_xmm_output);
            free(avx2_ymm_output); free(C_idx); free(asm_x64_idx); free(avx2_xmm_idx); free(avx2_ymm_idx);
            continue;
        }

        generate_random_array(A, B, size);

        // Time kernel
        double c_time = time_kernel(kernel_c, A, B, C_output, C_idx, size);
        double asm_x64_time = time_kernel(kernel_asm_x64, A, B, asm_x64_output, asm_x64_idx, size);
        //double avx2_xmm_time = time_kernel(kernel_avx2_xmm, A, B, avx2_xmm_output, avx2_xmm_idx, size);
        //double avx2_ymm_time = time_kernel(kernel_avx2_ymm, A, B, avx2_ymm_output, avx2_ymm_idx, size);

        // Check every kernel against the C version
        int asm_x64_err = count_mismatches(C_output, C_idx, asm_x64_output, asm_x64_idx, size);
        //int avx2_xmm_err = count_mismatches(C_output, C_idx, avx2_xmm_output, avx2_xmm_idx, size);
        //int avx2_ymm_err = count_mismatches(C_output, C_idx, avx2_ymm_output, avx2_ymm_idx, size);

        printf("Size: %d\n", size);
        printf("  C        : %10.4f ms\n", c_time);
        printf("  x64 asm  : %10.4f ms   mismatches: %d\n", asm_x64_time, asm_x64_err);
        //printf("  AVX2 XMM : %10.4f ms   mismatches: %d\n", avx2_xmm_time, avx2_xmm_err);
        //printf("  AVX2 YMM : %10.4f ms   mismatches: %d\n", avx2_ymm_time, avx2_ymm_err);
        printf("  First 5 results (C / x86):\n");
        for (int j = 0; j < 5; j++) {
            printf("    [%d] C = %f idx = %d  |  C = %f idx = %d\n",
                j, C_output[j], C_idx[j], asm_x64_output[j], asm_x64_idx[j]);
        }
        printf("\n");   
        printf("  Last 5 results (C / x86):\n");
        for (int j = size - 5; j < size; j++) {
            printf("    [%d] C = %f idx = %d  |  C = %f idx = %d\n",
                j, C_output[j], C_idx[j], asm_x64_output[j], asm_x64_idx[j]);
        }
        printf("\n");

        free(A);
        free(B);
        free(C_output);
        free(asm_x64_output);
        //free(avx2_xmm_output);
        //free(avx2_ymm_output);
        free(C_idx);
        free(asm_x64_idx);
        //free(avx2_xmm_idx);
        //free(avx2_ymm_idx);
    }

    return 0;
}