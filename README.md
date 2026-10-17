# PAPASA_SA_PARCO_MCO1

## 👥 Group Members 
*   **COPON, MATTHEW JOSHUA**
*   **LEE, ASHLEY FIONA SANTOS**
*   **ONG, EIRESS BASSEY HAO**
*   **XU, KAI WEN**

## Documentation
 **Execution Time Comparison**

**Release Build**
| Vector Size (n) | C Implementation | x86-64 Implementation | SIMD XMM | SIMD YMM |
|:---:|:---:|:---:|:---:|:---:|
| **2^20** | 1.73 ms | 0.79 ms | 0.39 ms | 0.38 ms |
| **2^26** | 113.15 ms | 64.05 ms | 47.06 ms | 46.43 ms |
| **2^30** | 472.15 ms | 333.72 ms | 235.54 ms | 220.27 ms |
| **Boundary (n=1003)** | <0.01 ms | < 0.01 ms | < 0.01 ms | < 0.01 ms |
| **Geometric Mean** | - | 1.88× | 3.10x | 3.21x |

AVX2 YMM is the fastest across all array sizes, with up to 4.5× speedup over pure C, followed closely by XMM at 4.4×. YMM processes 8 floats per instruction, while XMM processes 4, allowing YMM to handle more elements per iteration. The x86-64 assembly is also faster than pure C by 1.88× because it uses registers directly and avoids unnecessary instructions. However, the speedup does not reach the theoretical 4× or 8× limits because large arrays are memory-bound. Loading and storing data takes up a significant amount of time, limiting the benefits of wider registers.

**Debug Build**

| Vector Size (n) | C Implementation | x86-64 Implementation | SIMD XMM | SIMD YMM |
|:---:|:---:|:---:|:---:|:---:|
| **2^20** | 2.07 ms | 0.87 ms | 0.37 ms | 0.47 ms |
| **2^26** | 106.49 ms | 84.08 ms | 58.12 ms | 68.23 ms |
| **2^28** | 530.53 ms | 264.07 ms | 226.36 ms | 231.47 ms |
| **Boundary (n=1003)** | < 0.01 ms | < 0.01 ms | < 0.01 ms | < 0.01 ms |
| **Geometric Mean Speedup vs C** | — | 2.00× | 3.10× | 2.75× |

In Debug mode, pure C runs slower because compiler optimizations are disabled, resulting in higher speedups. XMM reaches 5.6× speedup on the smallest array, slightly faster than YMM, likely due to lower overhead for smaller workloads. Overall, SIMD remains faster than x86-64 assembly and pure C, though the gap between XMM and YMM is smaller than in Release mode.

<br>

- **Test Setup**
```c
void generate_random_array(float* A, float* B, int size) {
    for (int i = 0; i < size; i++) {
        A[i] = sinf(i * 0.002f);
        B[i] = cosf(i * 0.001f);
    }
}
```
Arrays A and B are generated using sinf() and cosf(), with A[i] = sinf(i * 0.002f) and B[i] = cosf(i * 0.001f). Each kernel compares the two arrays and stores the larger value in C[i], while idx[i] is set to 1 if A[i] is greater than or equal to B[i], and 0 otherwise. Timing covers only kernel execution, excluding array initialization and result printing. All tests run in x64 mode with AVX2 enabled, using array sizes of 2²⁰, 2²⁶, 2²⁸, and 1003 elements for the boundary case.

<br>

- **Correctness Check**

The C version was used as the answer key. The first 5 and last 5 elements were displayed, and all elements of each kernel's output were compared against the C output.

| Kernel | Result |
|:---:|:---:|
| x86-64 | Correct/Passed |
| SIMD XMM | Correct/Passed |
| SIMD YMM | Correct/Passed |

<br>

- **Boundary Handling**

<br>

- **Problems Encountered and Solutions**

<br>


- **Program Execution Output**

Refer to the images below for the benchmark results:

| C | x86-64 |
|:---:|:---:|
| ![C](screenshots/correctness_c.png) | ![x86-64](screenshots/correctness_x86_64.png) |

| SIMD XMM | SIMD YMM |
|:---:|:---:|
| ![XMM](screenshots/correctness_xmm.png) | ![YMM](screenshots/correctness_ymm.png) |

| Execution Time (All Cases) | Boundary Check (XMM and YMM) |
|:---:|:---:|
| ![All](screenshots/all_cases_timing.png) | ![Boundary](screenshots/boundary_check.png) |