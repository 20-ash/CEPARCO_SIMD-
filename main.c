#include <stdio.h>
#include <windows.h>
#include <math.h>

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
		A[i] = sin(i*0.001);
		B[i] = cos(i*0.002);
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

int matches(float* refC, int* refIdx, float* testC, int* testIdx, int size) {
	for (int i = 0; i < size; i++) {
		if (refC[i] != testC[i] || refIdx[i] != testIdx[i])
			return 0;
	}
	return 1;
}

void print_last5(float* C, int* idx, int size, const char* label) {
	int start = (size >= 5) ? (size - 5) : 0;
	printf("\n  %s — last 5 elements:\n", label);
	for (int i = start; i < size; i++) {
		printf("    [%d]  C = %10.6f   idx = %d\n", i, C[i], idx[i]);
	}
}


int main() {
	int sizes[] = {12, 67, 10};
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

		generate_random_array(A, B, size);

		// Time kernel
		double c_time = time_kernel(kernel_c, A, B, C_output, C_idx, size);
		double asm_x64_time = time_kernel(kernel_asm_x64, A, B, asm_x64_output, asm_x64_idx, size);
		double avx2_xmm_time = time_kernel(kernel_avx2_xmm, A, B, avx2_xmm_output, avx2_xmm_idx, size);
		double avx2_ymm_time = time_kernel(kernel_avx2_ymm, A, B, avx2_ymm_output, avx2_ymm_idx, size);

		kernel_c(A, B, C_output, C_idx, size);

		// Run ASM kernels & check
		kernel_asm_x64(A, B, asm_x64_output, asm_x64_idx, size);
		printf("  kernel_asm_x64    %s\n",
			matches(C_output, C_idx, asm_x64_output, asm_x64_idx, size) ? "MATCHES C" : "DOES NOT MATCH C");

		kernel_avx2_xmm(A, B, avx2_xmm_output, avx2_xmm_idx, size);
		printf("  kernel_avx2_xmm   %s\n",
			matches(C_output, C_idx, avx2_xmm_output, avx2_xmm_idx, size) ? "MATCHES C" : "DOES NOT MATCH C");
		print_last5(avx2_xmm_output, avx2_xmm_idx, size, "avx2_xmm");
		print_last5(avx2_xmm_output, avx2_xmm_idx, size, "AVX2 XMM Output");

		kernel_avx2_ymm(A, B, avx2_ymm_output, avx2_ymm_idx, size);
		printf("  kernel_avx2_ymm   %s\n",
			matches(C_output, C_idx, avx2_ymm_output, avx2_ymm_idx, size) ? "MATCHES C" : "DOES NOT MATCH C");
		print_last5(avx2_ymm_output, avx2_ymm_idx, size, "AVX2 YMM Output");

		// Optional timing display
		printf("\n  Timing (ms):\n");
		printf("  C:         %.4f\n", c_time);
		printf("  asm_x64:   %.4f\n", asm_x64_time);
		printf("  avx2_xmm:  %.4f\n", avx2_xmm_time);
		printf("  avx2_ymm:  %.4f\n", avx2_ymm_time);
		printf("size = %d\n", size);
		printf("remainder = %d\n", size % 8);

	}



	return 0;
}