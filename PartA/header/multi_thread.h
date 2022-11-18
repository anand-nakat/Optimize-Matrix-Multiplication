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

    int rowsPerThread = n / MAX_THREADS;
    int t = min(16, rowsPerThread); // Tile-Size

    for (int i = start; i < end; i += t)
    {
        for (int k = 0; k < n; k += t)
        {
            for (int j = 0; j < n; j += t)
            {

                for (int rowA = i; rowA < i + t; rowA += 2)
                {
                    for (int colB = j; colB < j + t; colB += 2)
                    {
                        int rowC = rowA >> 1;
                        int colC = colB >> 1;
                        int indexC = rowC * (n >> 1) + colC;
                        for (int iter = k; iter < k + t; iter++)
                        {
                            C[indexC] += A[rowA * n + iter] * B[iter * n + colB];
                            C[indexC] += A[rowA * n + iter] * B[iter * n + (colB + 1)];
                            C[indexC] += A[(rowA + 1) * n + iter] * B[iter * n + colB];
                            C[indexC] += A[(rowA + 1) * n + iter] * B[iter * n + (colB + 1)];
                        }

                        // compute output indices
                    }
                }
            }
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
