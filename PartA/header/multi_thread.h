#include <pthread.h>

#define MAX_THREADS 8
// Create other necessary functions here
int *A;
int *B;
int *C;
int n;

struct arg
{
    int startRow, endRow;
};

void *singleThreadCompute(void *a)
{
    struct arg *temp = (struct arg *)a;
    int start = temp->startRow;
    int end = temp->endRow;
    assert(n >= 4 and n == (n & ~(n - 1)));
    for (int rowA = start; rowA < end; rowA += 2)
    {
        for (int colB = 0; colB < n; colB += 2)
        {
            int sum = 0;
            /*Iterate for Row*/
            __m256i res = _mm256_setzero_si256();
            for (int iter = 0; iter < n; iter += 8)
            {
                __m256i a_row = _mm256_loadu_si256((__m256i *)(A + rowA * n + iter));
                __m256i a_row_plus_one = _mm256_loadu_si256((__m256i *)(A + (rowA + 1) * n + iter));
                __m256i b_col = _mm256_loadu_si256((__m256i *)(B + colB * n + iter));
                __m256i b_col_plus_one = _mm256_loadu_si256((__m256i *)(B + (colB + 1) * n + iter));

                res += _mm256_mullo_epi32(a_row, b_col);
                res += _mm256_mullo_epi32(a_row, b_col_plus_one);
                res += _mm256_mullo_epi32(a_row_plus_one, b_col);
                res += _mm256_mullo_epi32(a_row_plus_one, b_col_plus_one);
            }
            int *a = (int *)&res;
            for (int i = 0; i < 8; i++)
                sum += a[i];

            int rowC = rowA >> 1;
            int colC = colB >> 1;
            int indexC = rowC * (n >> 1) + colC;
            C[indexC] = sum;
        }
    }
    return NULL;
}

// Fill in this function
void multiThread(int N, int *matA, int *matB, int *output)
{
    pthread_t th[MAX_THREADS];
    struct arg v[MAX_THREADS];
    int chunks = N / MAX_THREADS;

    // Initialize Global Variables
    n = N;
    A = matA;
    B = matB;
    C = output;

    for (int i = 0; i < MAX_THREADS; i++)
    {
        v[i].startRow = i * chunks;
        v[i].endRow = (i + 1) * chunks;
        if (pthread_create(&th[i], NULL, &singleThreadCompute, (void *)(v + i)) != 0)
        {
            perror("Failed to create Thread");
        }
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join Threads");
        }
    }
}
