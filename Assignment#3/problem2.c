#include <omp.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
    int N;
    printf("Enter array size: ");
    scanf("%d", &N);

    int A[N];

    printf("Array: ");
    for (int i = 0; i < N; ++i)
    {
        scanf("%d", &A[i]);
    }

    int sum = 0;

    #pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < N; i++)
    {
        sum += A[i];
    }

    double mean = (double)sum / N;

    double variance = 0.0;

    #pragma omp parallel for reduction(+ : variance)
    for (int i = 0; i < N; i++)
    {
        variance += (A[i] - mean) * (A[i] - mean);
    }

    variance /= N;

    double stddev = sqrt(variance);

    printf("Standard Deviation: %f\n", stddev);
    return 0;
}