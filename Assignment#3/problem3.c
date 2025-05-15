#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int pid, size, chunkLen;
    int *arr;
    long long sum = 0, totalSum = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (pid == 0) // Master process
    {
        int N;
        printf("Enter array size: ");
        fflush(stdout);
        scanf("%d", &N);

        arr = (int *)malloc(N * sizeof(int));

        printf("Array: ");
        fflush(stdout);
        for (int i = 0; i < N; ++i)
        {
            scanf("%d", &arr[i]);
        }

        chunkLen = N / size;

        int offset = chunkLen;
        for (int i = 1; i < size - 1; i++)
        {
            MPI_Send(&chunkLen, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[offset], chunkLen, MPI_INT, i, 0, MPI_COMM_WORLD);
            offset += chunkLen;
        }
        int workLeft = N - offset;
        MPI_Send(&workLeft, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(&arr[offset], workLeft, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&chunkLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        arr = (int *)malloc(chunkLen * sizeof(int));
        MPI_Recv(arr, chunkLen, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    ////////// OpenMP parallel region //////////
    #pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < chunkLen; i++)
    {
        sum += arr[i];
    }
    ////////////////////////////////////////////

    printf("Process %d calculated sum: %lld\n", pid, sum);


    MPI_Reduce(&sum, &totalSum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (pid == 0)
    {
        printf("Total sum: %lld\n", totalSum);
        fflush(stdout);
    }
    free(arr);

    MPI_Finalize();

    return 0;
}