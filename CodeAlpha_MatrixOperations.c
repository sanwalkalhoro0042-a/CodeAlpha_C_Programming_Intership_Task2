#include <stdio.h>

#define MAX 10

// Read and validate the dimensions of a matrix
int readDimensions(const char *name, int *rows, int *cols)
{
    printf("Enter rows for matrix %s (1-%d): ", name, MAX);

    if (scanf("%d", rows) != 1 ||
        *rows < 1 || *rows > MAX)
    {
        printf("Error: Invalid number of rows.\n");
        return 0;
    }

    printf("Enter columns for matrix %s (1-%d): ", name, MAX);

    if (scanf("%d", cols) != 1 ||
        *cols < 1 || *cols > MAX)
    {
        printf("Error: Invalid number of columns.\n");
        return 0;
    }

    return 1;
}

// Input matrix elements
int inputMatrix(long long matrix[MAX][MAX],
                 int rows, int cols, const char *name)
{
    int i, j;

    printf("\nEnter elements of matrix %s:\n", name);

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            printf("%s[%d][%d]: ", name, i + 1, j + 1);

            if (scanf("%lld", &matrix[i][j]) != 1)
            {
                printf("Error: Enter integer values only.\n");
                return 0;
            }
        }
    }

    return 1;
}

// Display a matrix
void displayMatrix(long long matrix[MAX][MAX],
                   int rows, int cols)
{
    int i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            printf("%8lld", matrix[i][j]);
        }

        printf("\n");
    }
}

// Matrix addition
void addMatrices(long long A[MAX][MAX],
                 long long B[MAX][MAX],
                 long long result[MAX][MAX],
                 int rows, int cols)
{
    int i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            result[i][j] = A[i][j] + B[i][j];
        }
    }
}

// Matrix multiplication
void multiplyMatrices(long long A[MAX][MAX],
                      long long B[MAX][MAX],
                      long long result[MAX][MAX],
                      int rowsA, int colsA, int colsB)
{
    int i, j, k;

    for (i = 0; i < rowsA; i++)
    {
        for (j = 0; j < colsB; j++)
        {
            result[i][j] = 0;

            for (k = 0; k < colsA; k++)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Matrix transpose
void transposeMatrix(long long original[MAX][MAX],
                     long long transposed[MAX][MAX],
                     int rows, int cols)
{
    int i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            transposed[j][i] = original[i][j];
        }
    }
}

int main(void)
{
    long long A[MAX][MAX] = {{0}};
    long long B[MAX][MAX] = {{0}};
    long long result[MAX][MAX] = {{0}};

    int rowsA, colsA, rowsB, colsB;
    int choice;

    printf("===== CodeAlpha Matrix Operations =====\n");

    // Input matrix A
    if (!readDimensions("A", &rowsA, &colsA))
        return 1;

    if (!inputMatrix(A, rowsA, colsA, "A"))
        return 1;

    // Input matrix B
    if (!readDimensions("B", &rowsB, &colsB))
        return 1;

    if (!inputMatrix(B, rowsB, colsB, "B"))
        return 1;

    // Display the menu until the user exits
    do
    {
        printf("\n========== MENU ==========\n");
        printf("1. Matrix Addition\n");
        printf("2. Matrix Multiplication\n");
        printf("3. Transpose of Matrix A\n");
        printf("4. Transpose of Matrix B\n");
        printf("0. Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Error: Enter a valid menu number.\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                if (rowsA != rowsB || colsA != colsB)
                {
                    printf("Error: Addition requires matrices "
                           "of equal dimensions.\n");
                }
                else
                {
                    addMatrices(A, B, result, rowsA, colsA);

                    printf("\nA + B =\n");
                    displayMatrix(result, rowsA, colsA);
                }
                break;

            case 2:
                if (colsA != rowsB)
                {
                    printf("Error: Columns of A must equal "
                           "rows of B.\n");
                }
                else
                {
                    multiplyMatrices(
                        A, B, result, rowsA, colsA, colsB
                    );

                    printf("\nA x B =\n");
                    displayMatrix(result, rowsA, colsB);
                }
                break;

            case 3:
                transposeMatrix(A, result, rowsA, colsA);

                printf("\nTranspose of A =\n");
                displayMatrix(result, colsA, rowsA);
                break;

            case 4:
                transposeMatrix(B, result, rowsB, colsB);

                printf("\nTranspose of B =\n");
                displayMatrix(result, colsB, rowsB);
                break;

            case 0:
                printf("Program terminated. Thank you!\n");
                break;

            default:
                printf("Invalid option. Please choose 0 to 4.\n");
        }

    } while (choice != 0);

    return 0;
}
