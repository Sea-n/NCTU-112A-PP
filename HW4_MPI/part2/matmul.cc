#include <cstdio>
#include <mpi.h>

// Read size of matrix_a and matrix_b (n, m, l) and whole data of matrixes from stdin
//
// n_ptr:     pointer to n
// m_ptr:     pointer to m
// l_ptr:     pointer to l
// a_mat_ptr: pointer to matrix a (a should be a continuous memory space for placing n * m elements of int)
// b_mat_ptr: pointer to matrix b (b should be a continuous memory space for placing m * l elements of int)
void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
        int **a_mat_ptr, int **b_mat_ptr) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Read N, M, L and send to ALL */
    if (rank == 0)
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);
    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(m_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(l_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Allocate {A, B} Matrix for ALL */
    MPI_Alloc_mem(sizeof(int) * (*n_ptr) * (*m_ptr), MPI_INFO_NULL, a_mat_ptr);
    MPI_Alloc_mem(sizeof(int) * (*m_ptr) * (*l_ptr), MPI_INFO_NULL, b_mat_ptr);

    /* Read {A, B} Matrix */
    if (rank == 0) {
        for (int i=0; i<*n_ptr; i++)
            for (int j=0; j<*m_ptr; j++)
                scanf("%d", &(*a_mat_ptr)[i*(*m_ptr) + j]);

        for (int j=0; j<*m_ptr; j++)
            for (int k=0; k<*l_ptr; k++)
                scanf("%d", &(*b_mat_ptr)[j*(*l_ptr) + k]);
    }

    /* Send {A, B} Matrix from 0 to ALL */
    MPI_Bcast(*a_mat_ptr, (*n_ptr) * (*m_ptr), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(*b_mat_ptr, (*m_ptr) * (*l_ptr), MPI_INT, 0, MPI_COMM_WORLD);
}

// Just matrix multiplication (your should output the result in this function)
// 
// n:     row number of matrix a
// m:     col number of matrix a / row number of matrix b
// l:     col number of matrix b
// a_mat: a continuous memory placing n * m elements of int
// b_mat: a continuous memory placing m * l elements of int
void matrix_multiply(const int n, const int m, const int l,
        const int *a_mat, const int *b_mat) {
    int *c_mat;
    int size, rank;
    MPI_Request req[96];

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Alloc_mem(sizeof(int) * n * l, MPI_INFO_NULL, &c_mat);
    memset(c_mat, 0, sizeof(int) * n * l);

    for (int i = n * rank / size; i < n * (rank+1) / size; i++)
        for (int j=0; j<m; j++)
            for (int k=0; k<l; k++)
                c_mat[i*l + k] += a_mat[i*m + j] * b_mat[j*l + k];

    if (rank > 0) {
        MPI_Send(&c_mat[n * rank / size * l], (n*(rank+1)/size - n*rank/size) * l, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int k=1; k<size; k++)
            MPI_Irecv(&c_mat[n * k / size * l], (n*(k+1)/size - n*k/size) * l, MPI_INT, k, 0, MPI_COMM_WORLD, &req[k-1]);
        MPI_Waitall(size-1, req, MPI_STATUS_IGNORE);
    }

    if (rank == 0) {
        for (int i=0; i<n; i++) {
            for (int k=0; k<l; k++)
                printf("%d ", c_mat[i*l + k]);
            puts("");
        }
    }
    MPI_Free_mem(c_mat);
}

// Remember to release your allocated memory
void destruct_matrices(int *a_mat, int *b_mat) {
    MPI_Free_mem(a_mat);
    MPI_Free_mem(b_mat);
    return;
}
