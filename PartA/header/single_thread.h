// Optimize this function

void singleThread(int N, int *matA, int *matB, int *output)
{
  assert(N >= 4 and N == (N & ~(N - 1)));

  int t = 16; // Tile-Size
  for (int i = 0; i < N; i += t)
  {
    for (int k = 0; k < N; k += t)
    {
      for (int j = 0; j < N; j += t)
      {
        for (int rowA = i; rowA < i + t; rowA += 2)
        {
          for (int colB = j; colB < j + t; colB += 2)
          {
            int sum = 0;
            int rowC = rowA >> 1;
            int colC = colB >> 1;
            int indexC = rowC * (N >> 1) + colC;

            for (int iter = k; iter < k + t; iter++)
            {
              output[indexC] += matA[rowA * N + iter] * matB[iter * N + colB];
              output[indexC] += matA[rowA * N + iter] * matB[iter * N + (colB + 1)];
              output[indexC] += matA[(rowA + 1) * N + iter] * matB[iter * N + colB];
              output[indexC] += matA[(rowA + 1) * N + iter] * matB[iter * N + (colB + 1)];
            }
          }
        }
      }
    }
  }
}
