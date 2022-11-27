#include <pthread.h>

#define MAX_THREADS 16
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
    /*Get the Row limits of matA on which Thread should perform*/
    int start = temp->startRow;
    int end = temp->endRow;

    assert(n >= 8 and n == (n & ~(n - 1)));
    for (int rowA = start; rowA < end; rowA += 2)
    {
        for (int colB = 0; colB < n; colB += 2)
        {
            int sum = 0;
            /*Initialize Result Vector with 0*/
            __m256i res = _mm256_setzero_si256();
            /*Parallel computation on 8 elements, so increment by 8*/
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
            /*Add all the elements of the Vector to get the result*/
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

/*Note that transpose of matB is not required here as it's already computed in single_thread.h */
void multiThread(int N, int *matA, int *matB, int *output)
{
    int threads = min(MAX_THREADS, N);
    /*More Threads cause overhead for smaller N*/
    if (N <= 16)
        threads = 1;
    pthread_t th[threads];
    struct arg v[threads];
    int chunks = N / threads;

    // Initialize Global Variables
    n = N;
    A = matA;
    B = matB;
    C = output;

    for (int i = 0; i < threads; i++)
    {
        v[i].startRow = i * chunks;
        v[i].endRow = (i + 1) * chunks;
        if (pthread_create(&th[i], NULL, &singleThreadCompute, (void *)(v + i)) != 0)
        {
            perror("Failed to create Thread");
        }
    }

    for (int i = 0; i < threads; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join Threads");
        }
    }
}
