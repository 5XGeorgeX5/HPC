#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid, size;

    // Creation of parallel processes
    MPI_Init(&argc, &argv);

    // find out process ID,
    // and how many processes were started
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        if (pid == 0)
            printf("Please run with at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }
    if (pid == 0) // Master process
    {
        int n;
        int slaves = size - 1;
        int *arr;
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n", slaves);
        printf("Please enter size of array: ");
        fflush(stdout);
        scanf("%d", &n);
        arr = (int *)malloc(n * sizeof(int));
        printf("Please enter array elements\n");
        fflush(stdout);
        for (int i = 0; i < n; i++)
        {
            scanf("%d", &arr[i]);
        }
        int work = n / slaves;
        for (int i = 0; i < slaves - 1; i++)
        {
            int start = i * work;
            MPI_Send(&work, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[start], work, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }
        int start = (slaves - 1) * work;
        int workLeft = n - start;
        MPI_Send(&workLeft, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(&arr[start], workLeft, MPI_INT, size - 1, 0, MPI_COMM_WORLD);

        int max = -2147483648;
        int index = -1;
        for (int i = 0; i < slaves; i++)
        {
            int current_index;
            MPI_Recv(&current_index, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            current_index += i * work;
            if (arr[current_index] > max)
            {
                max = arr[current_index];
                index = current_index;
            }
        }
        printf("Master process announce the final max which is %d and its index is %d.\n", max, index);
    }
    else // Slave processes
    {
        int n;
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *arr = (int *)malloc(n * sizeof(int));
        MPI_Recv(arr, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int max = -2147483648;
        int index = -1;
        for (int i = 0; i < n; i++)
        {
            if (arr[i] > max)
            {
                max = arr[i];
                index = i;
            }
        }
        printf("Hello from slave#%d Max number in my partition is %d and index is %d\n", pid, max, index);
        MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // cleans up all MPI state before exit of process
    MPI_Finalize();

    return 0;
}