#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, int *argv[])
{

    srand(time(NULL));
    int MAX_RANDOM_VALUE = 10;
    int N, i, j;
    printf("Enter size of Matrix: ");
    scanf("%d", &N);

    int A[N][N], V[N], r[N];
    // initialization of Matrix & vector
    for (i = 0; i < N; ++i)
    {
        printf("Matrix row %d elements:", i);
        for (j = 0; j < N; ++j)
        {
            A[i][j] = rand() % (MAX_RANDOM_VALUE + 1);
            printf(" %d", A[i][j]);
        }
        V[i] = rand() % (MAX_RANDOM_VALUE + 1);
        printf("\nVector element %d: %d\n\n", i, V[i]);
    }

    omp_set_num_threads(N);
    // parallel part
    #pragma omp parallel private(i, j) shared(A, V, r)
    {
        int tempSum = 0;
        #pragma omp for
        for (i = 0; i < N; ++i)
        {
            for (j = 0; j < N; ++j)
            {
                tempSum += A[i][j] * V[j];
            }
            printf("Thread number %d calculated sum: %d\n", omp_get_thread_num(), tempSum);
            r[i] = tempSum;
        }
    }

    printf("\nFinal results stored in r: ");
    for (i = 0; i < N; i++)
    {
        printf("%d ", r[i]);
    }
}