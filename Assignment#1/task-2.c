#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void encrypt(char *input, char *output, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            output[i] = ((input[i] - 'a' + 3) % 26) + 'a';
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            output[i] = ((input[i] - 'A' + 3) % 26) + 'A';
        } else {
            output[i] = input[i];
        }
    }
}

void decrypt(char *input, char *output, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            output[i] = ((input[i] - 'a' - 3 + 26) % 26) + 'a';
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            output[i] = ((input[i] - 'A' - 3 + 26) % 26) + 'A';
        } else {
            output[i] = input[i];
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size, strLen, mode;
    char input[1000], result[1000];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter 1 for console input or 2 for file input: ");
        scanf("%d", &mode);

        if (mode == 1) {
            printf("Enter the string: ");
            scanf("%s", input);
        } else {
            FILE *fp = fopen("input.txt", "r");
            if (fp == NULL) {
                printf("Error: Could not open the file 'input.txt'.\n");
                return 1;
            }
            fscanf(fp, "%s", input);
            fclose(fp);
        }

        printf("Enter 1 to encode or 2 to decode: ");
        scanf("%d", &mode);

        strLen = 0;
        while (input[strLen] != '\0') strLen++;

        int chunk = strLen / size;
        int rem = strLen % size;

        for (int i = 1; i < size; i++) {
            int sendCount = chunk + (i < rem ? 1 : 0);
            int offset = i * chunk + (i < rem ? i : rem);
            MPI_Send(&sendCount, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&input[offset], sendCount, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            MPI_Send(&mode, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int localLen = chunk + (0 < rem ? 1 : 0);
        if (mode == 1) {
            encrypt(input, result, localLen);
        } else {
            decrypt(input, result, localLen);
        }

        int offset = localLen;
        for (int i = 1; i < size; i++) {
            int recvCount;
            MPI_Recv(&recvCount, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&result[offset], recvCount, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += recvCount;
        }
        result[strLen] = '\0';
        printf("Final Result: %s \n", result);
    } else {
        int recvLen, mode;
        char chunkStr[1000], chunkResult[1000];
        MPI_Recv(&recvLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&chunkStr, recvLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mode, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (mode == 1) {
            encrypt(chunkStr, chunkResult, recvLen);
        } else {
            decrypt(chunkStr, chunkResult, recvLen);
        }

        MPI_Send(&recvLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&chunkResult, recvLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
