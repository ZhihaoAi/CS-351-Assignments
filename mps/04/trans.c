/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

int transpose_32x32(int A[32][32], int B[32][32]){
    int bRow, bCol, row, col;
    int dgnVal, dgn;
    for (bCol = 0; bCol < 32; bCol += 8){
        for (bRow = 0; bRow < 32; bRow += 8){
            for (row = bRow; row < bRow + 8; row ++){
                for (col = bCol; col < bCol + 8; col ++){
                    if (row != col) {
                        B[col][row] = A[row][col];
                    } else {
                        dgnVal = A[row][col];
                        dgn = row;
                    }
                }
                if (bRow == bCol) { B[dgn][dgn] = dgnVal; }
            }
        }
    }
    return 0;
}

void swap(int *a, int *b){
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

int transpose_64x64(int A[64][64], int B[64][64]){
    int row, col, i, j;
    int t0,t1,t2,t3,t4,t5,t6,t7;

    for (row = 0; row < 64; row += 8)
        for (col = 0; col < 64; col += 8)
            if (row != col) {
                for (i = 0; i < 4; i++)
                    for (j = 0; j < 4; j++)
                            B[col + j][row + i] = A[row + i][col + j];
                    
                t0 = A[row][col + 4];
                t1 = A[row][col + 5];
                t2 = A[row][col + 6];
                t3 = A[row][col + 7];
                
                t4 = A[row + 1][col + 4];
                t5 = A[row + 1][col + 5];
                t6 = A[row + 1][col + 6];
                t7 = A[row + 1][col + 7];

                for (i = 4; i < 8; i++)
                    for (j = 0; j < 4; j++)
                        B[col + j][row + i] = A[row + i][col + j];

                for (i = 4; i < 8; i++)
                    for (j = 4; j < 8; j++)
                        B[col + j][row + i] = A[row + i][col + j];
                
                B[col + 4][row] = t0;
                B[col + 5][row] = t1;
                B[col + 6][row] = t2;
                B[col + 7][row] = t3;

                B[col + 4][row + 1] = t4;
                B[col + 5][row + 1] = t5;
                B[col + 6][row + 1] = t6;
                B[col + 7][row + 1] = t7;

                for (i = 2; i < 4; i++)
                    for (j = 4; j < 8; j++)
                        B[col + j][row + i] = A[row + i][col + j];
            } else {
                t0 = A[row][col + 0];
                t1 = A[row][col + 1];
                t2 = A[row][col + 2];
                t3 = A[row][col + 3];
                t4 = A[row][col + 4];
                t5 = A[row][col + 5];
                t6 = A[row][col + 6];
                t7 = A[row][col + 7];
                for (i = 0; i < 7; i++) {
                    if (i < 4) {
                        B[row + i][col + 0] = t0;
                        B[row + i][col + 1] = t1;
                        B[row + i][col + 2] = t2;
                        B[row + i][col + 3] = t3;
                        
                        B[row + i + 4][col + 0] = t4;
                        B[row + i + 4][col + 1] = t5;
                        B[row + i + 4][col + 2] = t6;
                        B[row + i + 4][col + 3] = t7;
                    } else {
                        B[row + i - 4][col + 4] = t0;
                        B[row + i - 4][col + 5] = t1;
                        B[row + i - 4][col + 6] = t2;
                        B[row + i - 4][col + 7] = t3;

                        B[row + i][col + 4] = t4;
                        B[row + i][col + 5] = t5;
                        B[row + i][col + 6] = t6;
                        B[row + i][col + 7] = t7;
                    }
                    t0 = A[row + i + 1][col + 0];
                    t1 = A[row + i + 1][col + 1];
                    t2 = A[row + i + 1][col + 2];
                    t3 = A[row + i + 1][col + 3];
                    t4 = A[row + i + 1][col + 4];
                    t5 = A[row + i + 1][col + 5];
                    t6 = A[row + i + 1][col + 6];
                    t7 = A[row + i + 1][col + 7];
                }
                B[row + i - 4][col + 4] = t0;
                B[row + i - 4][col + 5] = t1;
                B[row + i - 4][col + 6] = t2;
                B[row + i - 4][col + 7] = t3;

                B[row + i][col + 4] = t4;
                B[row + i][col + 5] = t5;
                B[row + i][col + 6] = t6;
                B[row + i][col + 7] = t7;
                
                for (i = 0; i < 4; i++)
                    for (j = i + 1; j < 4; j++)
                        swap(&B[row + i][col + j], &B[row + j][col + i]);
                for (i = 0; i < 4; i++)
                    for (j = i + 5; j < 8; j++)
                        swap(&B[row + i][col + j], &B[row + j - 4][col + i + 4]);
                for (i = 4; i < 8; i++)
                    for (j = i - 3; j < 4; j++)
                        swap(&B[row + i][col + j], &B[row + j + 4][col + i - 4]);
                for (i = 4; i < 8; i++)
                    for (j = i + 1; j < 8; j++)
                        swap(&B[row + i][col + j], &B[row + j][col + i]);
            }
    return 0;
}

int transpose_61x67(int A[67][61], int B[61][67]){
    int bRow, bCol, row, col;
    for(bCol = 0; bCol < 61; bCol += 16)
        for(bRow = 0; bRow < 67; bRow += 16)
            for(row = bRow; row < bRow + 16 && row < 67; row++)
                for(col = bCol; col < bCol + 16 && col < 61; col++)
                    B[col][row] = A[row][col];
     return 0;
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if ((M == 32) && (N == 32))
        transpose_32x32(A, B);
    else if((M == 64) && (N == 64))
        transpose_64x64(A, B);
    else if((M == 61) && (N == 67))
        transpose_61x67(A, B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

