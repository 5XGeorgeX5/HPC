#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void encrypt(char *str, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = ((str[i] - 'a' + 3) % 26) + 'a';
        }
        else if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = ((str[i] - 'A' + 3) % 26) + 'A';
        }
        else
        {
            str[i] = str[i];
        }
    }
}

void decrypt(char *str, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = ((str[i] - 'a' - 3 + 26) % 26) + 'a';
        }
        else if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = ((str[i] - 'A' - 3 + 26) % 26) + 'A';
        }
        else
        {
            str[i] = str[i];
        }
    }
}

int main(int argc, char *argv[])
{
    int rank, size, mode, chunkLen, strLen;
    char str[1000];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Enter 1 for console input or 2 for file input: ");
        fflush(stdout);
        scanf("%d", &mode);

        if (mode == 1)
        {
            printf("Enter the string: ");
            fflush(stdout);
            scanf("%s", str);
        }
        else
        {
            char filename[100];
            printf("Enter the filename: ");
            fflush(stdout);
            scanf("%s", filename);
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("Error: Could not open the file '%s'.\n", filename);
                return 1;
            }
            fscanf(fp, "%s", str);
            fclose(fp);
        }

        printf("Enter 1 to encode or 2 to decode: ");
        fflush(stdout);
        scanf("%d", &mode);

        strLen = strlen(str);

        chunkLen = strLen / size;

        int offset = chunkLen;
        for (int i = 1; i < size - 1; i++)
        {
            MPI_Send(&chunkLen, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&str[offset], chunkLen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            offset += chunkLen;
        }
        int workLeft = strLen - offset;
        MPI_Send(&workLeft, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(&str[offset], workLeft, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&chunkLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&str, chunkLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (mode == 1)
    {
        encrypt(str, chunkLen);
    }
    else
    {
        decrypt(str, chunkLen);
    }
    if (rank == 0)
    {
        int offset = chunkLen;
        for (int i = 1; i < size - 1; i++)
        {
            MPI_Recv(&str[offset], chunkLen, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += chunkLen;
        }
        int workLeft = strLen - offset;
        MPI_Recv(&str[offset], workLeft, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Final Result: %s \n", str);
    }
    else
    {
        MPI_Send(&str, chunkLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
