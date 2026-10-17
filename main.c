#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

extern void kernel_asm_x64(float*, float*, float*, int*, int);
extern void kernel_avx2_xmm(float*, float*, float*, int*, int);
extern void kernel_avx2_ymm(float*, float*, float*, int*, int);

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

	return total_ms/30;
}

void generate_random_array(float* A, float* B, int size) {
	for (int i = 0; i < size; i++) {
		A[i] = sinf(i*0.001f);
		B[i] = cosf(i*0.002f);
	}
}

void kernel_c(float* A, float* B, float* C, int* idx,int size) {
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

int sanity_check(const float* C, const int* idx, const float* kernel, const int* kernel_idx, int size) {
	for (int i = 0; i < size; i++)
		if (C[i] != kernel[i] || idx[i] != kernel_idx[i]) {
			return 0;
		}
	return 1;
}

void print_first_last5(float* kernel, int* idx, int size, const char* label) {

	printf("\nFirst 5 elements:\n");
	for (int i = 0; i < 5; i++) {
		printf("[%d] %s = %.6f idx = %d\n", i, label, kernel[i], idx[i]);
	}

	printf("\nLast 5 elements:\n");
	for (int i = size - 5; i < size; i++) {
		printf("[%d] %s = %.6f idx = %d\n", i, label, kernel[i], idx[i]);
	}
}

int main() {
	int n[] = {1048576, 67108864, 268435456, 1003};
	char* size_label[] = { "2^20", "2^26", "2^30", "Boundary Elements Test Case" };
	int num_sizes = sizeof(n) / sizeof(n[0]);

	for (int i = 0; i < num_sizes; i++) {
		int size = n[i];
		float* A = (float*)malloc(size * sizeof(float));
		float* B = (float*)malloc(size * sizeof(float));
		float* C = (float*)malloc(size * sizeof(float));
		float* asm_x64_output = (float*)malloc(size * sizeof(float));
		float* avx2_xmm_output = (float*)malloc(size * sizeof(float));
		float* avx2_ymm_output = (float*)malloc(size * sizeof(float));
		int* idx = (int*)malloc(size * sizeof(int));
		int* asm_x64_idx = (int*)malloc(size * sizeof(int));
		int* avx2_xmm_idx = (int*)malloc(size * sizeof(int));
		int* avx2_ymm_idx = (int*)malloc(size * sizeof(int));

		generate_random_array(A, B, size);

		// Time kernel
		double c_time = time_kernel(kernel_c, A, B, C, idx, size);
		double asm_x64_time = time_kernel(kernel_asm_x64, A, B, asm_x64_output, asm_x64_idx, size);
		double avx2_xmm_time = time_kernel(kernel_avx2_xmm, A, B, avx2_xmm_output, avx2_xmm_idx, size);
		double avx2_ymm_time = time_kernel(kernel_avx2_ymm, A, B, avx2_ymm_output, avx2_ymm_idx, size);

		// Display execution time
		printf("Vector Size: %d (%s)\n\n", size, size_label[i]);
		printf("C Execution time: %.2fms\n", c_time);
		printf("ASM x64 Execution time: %.2fms\n", asm_x64_time);
		printf("AVX2 XMM Execution time: %.2fms\n", avx2_xmm_time);
		printf("AVX2 YMM Execution time: %.2fms\n\n", avx2_ymm_time);

		// Perform sanity checks
		if (sanity_check(C, idx, asm_x64_output, asm_x64_idx, size)) {
			printf("ASM x64 results are correct.\n");
		} else {
			printf("ASM x64 results are incorrect.\n");
		}

		if (sanity_check(C, idx, avx2_xmm_output, avx2_xmm_idx, size)) {
			printf("AVX2 XMM results are correct.\n");
		} else {
			printf("AVX2 XMM results are incorrect.\n");
		}

		if (sanity_check(C, idx, avx2_ymm_output, avx2_ymm_idx, size)) {
			printf("AVX2 YMM results are correct.\n");
		} else {
			printf("AVX2 YMM results are incorrect.\n");
		}

		// Print first and last five elements
		print_first_last5(C, idx, size, "C");
		print_first_last5(asm_x64_output, asm_x64_idx, size, "ASM x64");
		print_first_last5(avx2_xmm_output, avx2_xmm_idx, size, "AVX2 XMM");
		print_first_last5(avx2_ymm_output, avx2_ymm_idx, size, "AVX2 YMM");
		printf("\n===================================================\n\n");

		free(A);
		free(B);
		free(C);
		free(asm_x64_output);
		free(avx2_xmm_output);
		free(avx2_ymm_output);
		free(idx);
		free(asm_x64_idx);
		free(avx2_xmm_idx);
		free(avx2_ymm_idx);
	}

	return 0;
}