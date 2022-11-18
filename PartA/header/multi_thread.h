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
            for (int iter = 0; iter < n; iter++)
            {
                sum += A[rowA * n + iter] * B[iter * n + colB];
                sum += A[rowA * n + iter] * B[iter * n + (colB + 1)];
                sum += A[(rowA + 1) * n + iter] * B[iter * n + colB];
                sum += A[(rowA + 1) * n + iter] * B[iter * n + (colB + 1)];
            }

            // compute output indices
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
