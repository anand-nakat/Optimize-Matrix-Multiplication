// Optimize this function
#include <immintrin.h>

void transpose(int N, int *matB)
{
  for (int i = 0; i < N; i++)
  {
    for (int j = i; j < N; j++)
    {
      swap(matB[i * N + j], matB[j * N + i]);
    }
  }
}

void singleThread(int N, int *matA, int *matB, int *output)
{
  assert(N >= 8 and N == (N & ~(N - 1)));
  transpose(N, matB);

  for (int rowA = 0; rowA < N; rowA += 2)
  {
    for (int colB = 0; colB < N; colB += 2)
    {
      int sum = 0;
      /*Iterate for Row*/
      __m256i res = _mm256_setzero_si256();
      for (int iter = 0; iter < N; iter += 8)
      {
        __m256i a_row = _mm256_loadu_si256((__m256i *)(matA + rowA * N + iter));
        __m256i a_row_plus_one = _mm256_loadu_si256((__m256i *)(matA + (rowA + 1) * N + iter));
        __m256i b_col = _mm256_loadu_si256((__m256i *)(matB + (colB)*N + iter));
        __m256i b_col_plus_one = _mm256_loadu_si256((__m256i *)(matB + (colB + 1) * N + iter));

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
      int indexC = rowC * (N >> 1) + colC;
      output[indexC] = sum;
    }
  }
}
