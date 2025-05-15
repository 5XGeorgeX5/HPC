#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, int *argv[])
{

    srand(time(NULL));
    int MAX_RANDOM_VALUE = 10;
    int N;
    printf("Enter size of Matrix: ");
    scanf("%d", &N);

    int A[N][N], V[N], r[N];
    // initialization of Matrix & vector
    printf("Matrix A:\n");
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            A[i][j] = rand() % (MAX_RANDOM_VALUE + 1);
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    printf("\nVector V:\n");
    for (int i = 0; i < N; ++i)
    {
        V[i] = rand() % (MAX_RANDOM_VALUE + 1);
        printf("%d ", V[i]);
    }
    printf("\n\n");

    // parallel part
    #pragma omp parallel for shared(A, V, r)
    for (int i = 0; i < N; ++i)
    {
        r[i] = 0;
        for (int j = 0; j < N; ++j)
        {
            r[i] += A[i][j] * V[j];
        }
        printf("Thread number %d calculated sum: %d\n", omp_get_thread_num(), r[i]);
    }

    printf("\nResulting vector r:\n");
    for (int i = 0; i < N; i++)
    {
        printf("%d ", r[i]);
    }
    printf("\n");
    return 0;
}