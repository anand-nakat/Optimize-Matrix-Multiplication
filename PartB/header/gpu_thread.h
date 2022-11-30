// Create other necessary functions here
__global__ void GPU_Mul(int MatSize, int *matA, int *matB, int *matC)
{

    // Caclulating Global rows and coloumn for each thread
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    // Calculating row and column for reduced matrice multiplication
    int R1 = 2 * row;
    int R2 = 2 * row + 1;
    int C1 = 2 * col;
    int C2 = 2 * col + 1;

    int tempMag = MatSize / 2;
    int tempValue = 0;

    for (int k = 0; k < MatSize; k++)
    {

        // Sum1 ----> Calculating dot product of (R1,C1)
        int posA1 = matA[R1 * MatSize + k];
        int posB1 = matB[k * MatSize + C1];
        tempValue += posA1 * posB1;

        // Sum2 ----> Calculating dot product of (R1,C2)
        int posA2 = matA[R1 * MatSize + k];
        int posB2 = matB[k * MatSize + C2];
        tempValue += posA2 * posB2;

        // S3 ----> Calculating dot product of (R2,C1)
        int posA3 = matA[R2 * MatSize + k];
        int posB3 = matB[k * MatSize + C1];
        tempValue += posA3 * posB3;

        // Sum4 ----> Calculating dot product of (R2,C2)
        int posA4 = matA[R2 * MatSize + k];
        int posB4 = matB[k * MatSize + C2];
        tempValue += posA4 * posB4;
    }
    matC[row * tempMag + col] = tempValue;
}

// Fill in this function
void gpuThread(int N, int *matA, int *matB, int *output)
{
    // cout<<"Thread Executing"<<"\n";

    // Calculate size of matrix in bytes
    size_t size = N * N * sizeof(int);

    // Pointers for device matrices
    int *A, *B, *C;

    // Allocate device memory for matrice A
    cudaMalloc((void **)&A, size);
    cudaMemcpy(A, matA, size, cudaMemcpyHostToDevice);

    // Allocate device memory for matrice B
    cudaMalloc((void **)&B, size);
    cudaMemcpy(B, matB, size, cudaMemcpyHostToDevice);

    // Allocate device memory for d_C
    cudaMalloc((void **)&C, size / 4);

    // Launch kernel on device
    int blocks = 16;
    int threads = (N / (2 * blocks));
    dim3 gridDim(threads, threads);
    dim3 blockDim(blocks, blocks);

    // Invoking Kernel
    GPU_Mul<<<gridDim, blockDim>>>(N, A, B, C);

    // Copy result from device to host
    cudaMemcpy(output, C, size / 4, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(A);
    cudaFree(B);
    cudaFree(C);

    // Free host memory
    cudaFree(matA);
    cudaFree(matB);
}
