#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <time.h>

// CONFIGURATION
// From the internet:
// If your PC has 16GB RAM, use 28 or 29. 
// If your PC has 8GB RAM, use 27 or 28. (i had 6gb on my VM, so I'm using 27)
// 2^30 REQUIRES 20GB RAM. Do not use 30 unless you have 32GB+.
#define MAX_POWER 28

extern void asmMCO2(long long n, float* x1, float* x2, float* y1, float* y2, float* z);

void cMCO2(long long n, float* x1, float* x2, float* y1, float* y2, float* z) {
    for (long long i = 0; i < n; i++) {
        float diffX = x2[i] - x1[i];
        float diffY = y2[i] - y1[i];
        z[i] = sqrtf((diffX * diffX) + (diffY * diffY));
    }
}

// for timing
double get_time_sec(LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER freq) {
    return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
}

// phase 1: correctness check then display
void run_correctness_phase() {
    long long n = 1 << 20; // 1 million elements
    printf("\n[PHASE 1] Checking Correctness & Displaying First 10 (N = 2^20 = %lld)...\n", n);

    float* x1 = (float*)malloc(n * sizeof(float));
    float* x2 = (float*)malloc(n * sizeof(float));
    float* y1 = (float*)malloc(n * sizeof(float));
    float* y2 = (float*)malloc(n * sizeof(float));
    float* z_c = (float*)malloc(n * sizeof(float));
    float* z_asm = (float*)malloc(n * sizeof(float));

    // initialize the data with random values
    for (long long i = 0; i < n; i++) {
        x1[i] = (float)(rand() % 100) / 2.0f; 
        x2[i] = (float)(rand() % 100) / 2.0f;
        y1[i] = (float)(rand() % 100) / 2.0f; 
        y2[i] = (float)(rand() % 100) / 2.0f;
    }

    // run the C Version
    cMCO2(n, x1, x2, y1, y2, z_c);
    
    // run the ASM Version
    asmMCO2(n, x1, x2, y1, y2, z_asm);

    // displaying the 1st 10 elements
    printf("\n%-10s %-15s %-15s %-10s\n", "Index", "C Result", "ASM Result", "Status");
    printf("-------------------------------------------------------\n");
    int errors = 0;
    for (long long i = 0; i < n; i++) {
        float diff = fabsf(z_c[i] - z_asm[i]);
    
        // Print first 10 strictly
        if (i < 10) {
            char* status;
            if (diff < 0.0001f) {
                status = "MATCH";
            } else {
                status = "DIFF";
            }

            printf("%-10lld %-15.6f %-15.6f %-10s\n", 
                   i, z_c[i], z_asm[i], status);
        }

        if (diff > 0.0001f) {
            errors++;
        }
    }
    printf("-------------------------------------------------------\n");

    if (errors == 0) printf("  [PASS] All %lld elements match.\n", n);
    else printf("  [FAIL] Total mismatches found: %d\n", errors);

    // free everything to clear RAM for phase 2
    free(x1); free(x2); free(y1); free(y2); free(z_c); free(z_asm);
    printf("  [INFO] Memory freed.\n");
}


// phase 2: performance test
void run_performance_phase(int power) {
    long long n = 1LL << power;
    printf("\n[PHASE 2] Benchmarking Performance (N = 2^%d = %lld)...\n", power, n);
    printf("  [INFO] Allocating %.2f GB of RAM...\n", (double)(n * 5 * 4) / (1024 * 1024 * 1024));

    // allocate the common inputs
    float* x1 = (float*)malloc(n * sizeof(float));
    float* x2 = (float*)malloc(n * sizeof(float));
    float* y1 = (float*)malloc(n * sizeof(float));
    float* y2 = (float*)malloc(n * sizeof(float));
    float* z = (float*)malloc(n * sizeof(float)); // reused for both

    if (!x1 || !x2 || !y1 || !y2 || !z) {
        printf("  [ERROR] Not enough RAM! Try lowering MAX_POWER.\n");
        return;
    }

    // initialize data (only once)
    for (long long i = 0; i < n; i++) {
        x1[i] = 1.0f; x2[i] = 4.0f; y1[i] = 2.0f; y2[i] = 6.0f;
    }

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    const int RUNS = 30;

    // TEST C
    double totalTimeC = 0.0;
    printf("  Running C Kernel %d times...\n", RUNS);
    for (int i = 0; i < RUNS; i++) {
        QueryPerformanceCounter(&start);
        cMCO2(n, x1, x2, y1, y2, z);
        QueryPerformanceCounter(&end);
        totalTimeC += get_time_sec(start, end, freq);
    }
    double avgC = totalTimeC / RUNS;
    printf("  >> Average C Time:   %.5f sec\n", avgC);

    // TEST ASM
    double totalTimeASM = 0.0;
    printf("  Running ASM Kernel %d times...\n", RUNS);
    for (int i = 0; i < RUNS; i++) {
        QueryPerformanceCounter(&start);
        asmMCO2(n, x1, x2, y1, y2, z);
        QueryPerformanceCounter(&end);
        totalTimeASM += get_time_sec(start, end, freq);
    }
    double avgASM = totalTimeASM / RUNS;
    printf("  >> Average ASM Time: %.5f sec\n", avgASM);

    free(x1); free(x2); free(y1); free(y2); free(z);
}

int main() {
	srand((unsigned int)time(NULL));
    run_correctness_phase();

    run_performance_phase(20);
    run_performance_phase(24);
    
    // in case of crash, lower MAX_POWER above
    run_performance_phase(MAX_POWER);

    return 0;
}
