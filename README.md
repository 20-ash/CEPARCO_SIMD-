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
| **2^20** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **2^26** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **2^30** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **Boundary (n=1003)** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **Geometric Mean** | 0.00x | 0.00x | 0.00x |

**Debug Build**

| Vector Size (n) | C Implementation | x86-64 Implementation | SIMD XMM | SIMD YMM |
|:---:|:---:|:---:|:---:|:---:|
| **2^20** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **2^26** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **2^30** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **Boundary (n=1003)** | 0.000000 | 0.000000 | 0.000000 | 0.000000 |
| **Geometric Mean** | 0.00x | 0.00x | 0.00x |

<br>

- **Test Setup**

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


## Execution Video
[Google Drive Link](link)
